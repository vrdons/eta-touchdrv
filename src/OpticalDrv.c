#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/hid.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/usb.h>
#include <linux/usb/input.h>
#include <linux/version.h>

#include <OpticalDrv.h>

#define DRIVER_NAME "Optical touch device"

#define err(format, arg...) pr_err(KBUILD_MODNAME ": " format "\n", ##arg)
#define info(format, arg...) pr_info(KBUILD_MODNAME ": " format "\n", ##arg)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 8, 0))
#define strlcpy strscpy
#endif

#define OPTICAL_MINOR_BASE 0

static const struct optical_variant variant_irtouch = {
    .dev_node_fmt = "IRTouchOptical%03d",
    .touch_points = 2,
};

static const struct optical_variant variant_raw = {
    .dev_node_fmt = "OtdUsbRaw%03d",
    .touch_points = 10,
};

static struct usb_device_id const dev_table[] = {
    {USB_DEVICE(0x6615, 0x0084),
     .driver_info = (kernel_ulong_t)&variant_irtouch},
    {USB_DEVICE(0x6615, 0x0085),
     .driver_info = (kernel_ulong_t)&variant_irtouch},
    {USB_DEVICE(0x6615, 0x0086),
     .driver_info = (kernel_ulong_t)&variant_irtouch},
    {USB_DEVICE(0x6615, 0x0087),
     .driver_info = (kernel_ulong_t)&variant_irtouch},
    {USB_DEVICE(0x6615, 0x0088),
     .driver_info = (kernel_ulong_t)&variant_irtouch},
    {USB_DEVICE(0x6615, 0x0c20),
     .driver_info = (kernel_ulong_t)&variant_irtouch},
    {USB_DEVICE(0x2621, 0x2201), .driver_info = (kernel_ulong_t)&variant_raw},
    {USB_DEVICE(0x2621, 0x4501), .driver_info = (kernel_ulong_t)&variant_raw},
    {}};

static struct file_operations optical_fops;
static struct usb_driver optical_driver;
static DEFINE_MUTEX(optical_file_lock);

static void submit_urb(device_context *device, gfp_t gfp_mask) {
  int retval;

  retval = usb_submit_urb(device->interrupt_urb, gfp_mask);
  if (retval != 0) {
    err("%s: usb_submit_urb failed: %d", __func__, retval);
    return;
  }
}
static void cancel_urb(device_context *device) {
  usb_kill_urb(device->interrupt_urb);
}

static ssize_t optical_read(struct file *filp, char __user *buffer,
                            size_t count, loff_t *ppos) {
  size_t avail;
  device_context *device;
  unsigned char local_buf[256];
  int r;

  device = filp->private_data;
  if (device == NULL)
    return -EFAULT;

  info("%s: count=%zu buffer_length=%u", __func__, count,
       READ_ONCE(device->buffer_length));

  if (count == 0)
    return 0;

  if ((filp->f_flags & O_NONBLOCK) != 0 &&
      READ_ONCE(device->buffer_length) == 0)
    return -EAGAIN;

  r = wait_event_interruptible(device->read_wait,
                               READ_ONCE(device->buffer_length) > 0 ||
                                   READ_ONCE(device->disconnected));
  if (r != 0)
    return -EINTR;
  if (READ_ONCE(device->disconnected))
    return -ENODEV;

  spin_lock_irq(&device->lock);
  avail = device->buffer_length;
  if (avail == 0) {
    spin_unlock_irq(&device->lock);
    return 0;
  }
  if (count > avail)
    count = avail;
  if (count > sizeof(local_buf))
    count = sizeof(local_buf);
  memcpy(local_buf, device->buffer, count);
  device->buffer_length = 0;
  spin_unlock_irq(&device->lock);

  if (copy_to_user(buffer, local_buf, count) != 0)
    return -EFAULT;

  return (ssize_t)count;
}

static ssize_t optical_write(struct file *filp, const char *user_buffer,
                             size_t count, loff_t *ppos) {
  device_context *device;

  device = filp->private_data;
  if (device == NULL) {
    return -EFAULT;
  }

  return -EFAULT;
}

static long set_report(device_context *device, unsigned short length,
                       void const *data) {
  void *kernel_data;
  int r;

  kernel_data = kzalloc(length, GFP_KERNEL);
  if (kernel_data == NULL) {
    return -ENOMEM;
  }
  do {
    r = copy_from_user(kernel_data, data, length);
    if (r != 0) {
      r = -EFAULT;
      break;
    }
    if (length < 1) {
      r = -EINVAL;
      break;
    }
    r = usb_control_msg(device->usb_device,
                        usb_sndctrlpipe(device->usb_device, 0), 0, 0x40, 0, 0,
                        kernel_data, length, 1000);
    kfree(kernel_data);
    return r;
  } while (false);
  kfree(kernel_data);
  return r;
}
static long get_report(device_context *device, unsigned short length,
                       void *data) {
  void *kernel_data;
  int r;

  kernel_data = kzalloc(length, GFP_KERNEL);
  if (kernel_data == NULL) {
    return -ENOMEM;
  }
  do {
    if (length < 1) {
      r = -EINVAL;
      break;
    }
    r = usb_control_msg(device->usb_device,
                        usb_rcvctrlpipe(device->usb_device, 0), 0, 0xc0, 0, 0,
                        kernel_data, length, 1000);
    if (r >= 0) {
      if (copy_to_user(data, kernel_data, r) != 0) {
        r = -EFAULT;
        break;
      }
    }
    kfree(kernel_data);
    return r;
  } while (false);
  kfree(kernel_data);
  return r;
}
static long sync_absolute_mouse(device_context *device, unsigned short length,
                                void const *data) {
  // TODO
  return 0;
}

static long sync_touch_points(device_context *device,
                              const OpticalReportTouchPoint *touch_points,
                              size_t touch_point_count) {
  size_t i;
  bool any_touch = false;

  for (i = 0; i < touch_point_count; i++) {
    bool touched;

    input_mt_slot(device->input_dev, i);

    if ((touch_points[i].state & OpticalReportTouchPointStateFlag_IsValid) ==
        0) {
      input_mt_report_slot_state(device->input_dev, MT_TOOL_FINGER, false);
      continue;
    }

    touched = (touch_points[i].state & OpticalReportTouchPointStateFlag_IsTouched) !=
              0;
    input_mt_report_slot_state(device->input_dev, MT_TOOL_FINGER, touched);

    if (touched) {
      any_touch = true;
      input_report_abs(device->input_dev, ABS_MT_TOUCH_MAJOR,
                       touch_points[i].width);
      input_report_abs(device->input_dev, ABS_MT_TOUCH_MINOR,
                       touch_points[i].height);
      input_report_abs(device->input_dev, ABS_MT_POSITION_X, touch_points[i].x);
      input_report_abs(device->input_dev, ABS_MT_POSITION_Y, touch_points[i].y);
    }
  }

  input_mt_sync_frame(device->input_dev);
  input_report_key(device->input_dev, BTN_TOUCH, any_touch);
  input_sync(device->input_dev);
  return 0;
}

static long sync_singletouch(device_context *device, unsigned short length,
                             void const *data) {
  OpticalReportPacketSingleTouch value;
  int r;

  info("%s: length=%u expected=%zu", __func__, length, sizeof(value));

  if (length < sizeof(value)) {
    return -EINVAL;
  }

  r = copy_from_user(&value, data, sizeof(value));
  if (r != 0) {
    return -EFAULT;
  }
  sync_touch_points(device, &value.touchPoint, 1);
  return (long)sizeof(value);
}

static long sync_multitouch(device_context *device, unsigned short length,
                            void const *data) {
  OpticalReportTouchPoint *points;
  size_t touch_points;
  size_t packet_size;
  int r;

  touch_points = device->variant->touch_points;
  packet_size = OPTICAL_MULTITOUCH_PACKET_SIZE(touch_points);

  info("%s: length=%u expected=%zu touch_points=%zu", __func__, length,
       packet_size, touch_points);

  if (length < packet_size) {
    return -EINVAL;
  }

  points = kzalloc(packet_size, GFP_KERNEL);
  if (points == NULL) {
    return -ENOMEM;
  }
  r = copy_from_user(points, data, packet_size);
  if (r != 0) {
    kfree(points);
    return -EFAULT;
  }
  sync_touch_points(device, points, touch_points);
  kfree(points);
  return (long)packet_size;
}

static long sync_keyboard(device_context *device, unsigned short length,
                          void const *data) {
  // TODO
  return 0;
}
static long sync_diagnosis(device_context *device, unsigned short length,
                           void const *data) {
  // TODO
  return 0;
}
static long sync_rawtouch(device_context *device, unsigned short length,
                          void const *data) {
  // TODO
  return 0;
}
static long sync_touch(device_context *device, unsigned short length,
                       void const *data) {
  // TODO
  return 0;
}
static long sync_virtualkey(device_context *device, unsigned short length,
                            void const *data) {
  // TODO
  return 0;
}
static long optical_unlocked_ioctl(struct file *filp, unsigned int ctl_code,
                                   unsigned long ctl_param) {
  device_context *device;

  device = filp->private_data;
  if (device == NULL) {
    return -EFAULT;
  }

  info("%s: code=0x%x type=0x%x len=%u", __func__, ctl_code,
       ctl_code & OPTICAL_IOCTL_CODE_TYPE_MASK,
       ctl_code & OPTICAL_IOCTL_CODE_LENGTH_MASK);

  switch (ctl_code & OPTICAL_IOCTL_CODE_TYPE_MASK) {
  case OPTICAL_IOCTL_CODE_TYPE_SET_REPORT:
    return set_report(device, ctl_code & OPTICAL_IOCTL_CODE_LENGTH_MASK,
                      (void const *)ctl_param);
  case OPTICAL_IOCTL_CODE_TYPE_GET_REPORT:
    return get_report(device, ctl_code & OPTICAL_IOCTL_CODE_LENGTH_MASK,
                      (void *)ctl_param);
  case OPTICAL_IOCTL_CODE_TYPE_SYNC_ABSOLUTEMOUSE:
    return sync_absolute_mouse(device,
                               ctl_code & OPTICAL_IOCTL_CODE_LENGTH_MASK,
                               (void const *)ctl_param);
  case OPTICAL_IOCTL_CODE_TYPE_SYNC_SINGLETOUCH:
    return sync_singletouch(device, ctl_code & OPTICAL_IOCTL_CODE_LENGTH_MASK,
                            (void const *)ctl_param);
  case OPTICAL_IOCTL_CODE_TYPE_SYNC_MULTITOUCH:
    return sync_multitouch(device, ctl_code & OPTICAL_IOCTL_CODE_LENGTH_MASK,
                           (void const *)ctl_param);
  case OPTICAL_IOCTL_CODE_TYPE_SYNC_KEYBOARD:
    return sync_keyboard(device, ctl_code & OPTICAL_IOCTL_CODE_LENGTH_MASK,
                         (void const *)ctl_param);
  case OPTICAL_IOCTL_CODE_TYPE_SYNC_DIAGNOSIS:
    return sync_diagnosis(device, ctl_code & OPTICAL_IOCTL_CODE_LENGTH_MASK,
                          (void const *)ctl_param);
  case OPTICAL_IOCTL_CODE_TYPE_SYNC_RAWTOUCH:
    return sync_rawtouch(device, ctl_code & OPTICAL_IOCTL_CODE_LENGTH_MASK,
                         (void const *)ctl_param);
  case OPTICAL_IOCTL_CODE_TYPE_SYNC_TOUCH:
    return sync_touch(device, ctl_code & OPTICAL_IOCTL_CODE_LENGTH_MASK,
                      (void const *)ctl_param);
  case OPTICAL_IOCTL_CODE_TYPE_SYNC_VIRTUALKEY:
    return sync_virtualkey(device, ctl_code & OPTICAL_IOCTL_CODE_LENGTH_MASK,
                           (void const *)ctl_param);
  }
  return 0;
}

static int optical_open(struct inode *inode, struct file *filp) {
  device_context *device;
  struct usb_interface *interface;
  int subminor;

  subminor = iminor(inode);

  info("%s: subminor=%d", __func__, subminor);

  interface = usb_find_interface(&optical_driver, subminor);

  if (interface == NULL) {
    err("%s: interface ptr is NULL.", __func__);
    return -ENODEV;
  }
  mutex_lock(&optical_file_lock);
  device = usb_get_intfdata(interface);
  if (device == NULL) {
    mutex_unlock(&optical_file_lock);
    err("%s: device context is NULL.", __func__);
    return -ENODEV;
  }
  if (device->disconnected) {
    mutex_unlock(&optical_file_lock);
    return -ENODEV;
  }
  if (device->file_private_data != NULL) {
    mutex_unlock(&optical_file_lock);
    return -EBUSY;
  }
  device->file_private_data = filp;
  filp->private_data = device;
  mutex_unlock(&optical_file_lock);

  return 0;
}

static int optical_release(struct inode *inode, struct file *filp) {
  device_context *device;

  info("%s", __func__);

  mutex_lock(&optical_file_lock);
  device = filp->private_data;
  if (device != NULL) {
    device->file_private_data = NULL;
  }
  filp->private_data = NULL;
  mutex_unlock(&optical_file_lock);

  return 0;
}

static struct file_operations optical_fops = {
    .owner = THIS_MODULE,
    .read = optical_read,
    .write = optical_write,
    .unlocked_ioctl = optical_unlocked_ioctl,
    .open = optical_open,
    .release = optical_release,
};

static void on_interrupt(struct urb *interrupt_urb) {
  device_context *device;
  unsigned int length;

  device = interrupt_urb->context;

  info("%s: status=%d actual_length=%u", __func__, interrupt_urb->status,
       interrupt_urb->actual_length);

  switch (interrupt_urb->status) {
  case -ECONNRESET:
  case -ENOENT:
  case -ESHUTDOWN:
    return;
  }

  spin_lock(&device->lock);
  if (interrupt_urb->status == 0) {
    if (interrupt_urb->actual_length > 0) {
      length = min_t(unsigned int, interrupt_urb->actual_length,
                     device->max_packet_size);
      memcpy(device->buffer, device->ongoing_buffer, length);
      device->buffer_length = length;
    }
  }
  spin_unlock(&device->lock);

  if (interrupt_urb->status == 0 && interrupt_urb->actual_length > 0)
    wake_up_interruptible(&device->read_wait);

  submit_urb(device, GFP_ATOMIC);
}

static int optical_open_device(struct input_dev *input_dev) {
  device_context *device;

  device = input_get_drvdata(input_dev);
  info("%s", __func__);

  submit_urb(device, GFP_KERNEL);
  return 0;
}

static void optical_close_device(struct input_dev *input_dev) {
  device_context *device;

  device = input_get_drvdata(input_dev);
  info("%s", __func__);

  cancel_urb(device);
}

static int device_context_init(device_context *obj,
                               struct usb_interface *intf) {
  int i;

  obj->usb_device = interface_to_usbdev(intf);

  for (i = 0; i < intf->cur_altsetting->desc.bNumEndpoints; i++) {
    if (intf->cur_altsetting->endpoint[i].desc.bEndpointAddress & USB_DIR_IN) {
      obj->pipe_input = usb_rcvintpipe(
          obj->usb_device,
          intf->cur_altsetting->endpoint[i].desc.bEndpointAddress);
      obj->max_packet_size =
          usb_endpoint_maxp(&intf->cur_altsetting->endpoint[i].desc);
      obj->pipe_interval = intf->cur_altsetting->endpoint[i].desc.bInterval;
      info("%s: endpoint found, max_packet_size=%u interval=%u", __func__,
           obj->max_packet_size, obj->pipe_interval);
      return 0;
    }
  }

  err("%s: no IN endpoint found", __func__);
  return -ENODEV;
}

static void input_dev_init(struct input_dev *obj, device_context *device,
                           device_context_pool *pool,
                           struct usb_device *usb_device,
                           struct device *parent) {
  if (usb_device->manufacturer != NULL) {
    strlcpy(pool->name, usb_device->manufacturer, sizeof(pool->name));
  } else {
    pool->name[0] = 0;
  }

  if (usb_device->product != NULL) {
    strlcat(pool->name, " ", sizeof(pool->name));
    strlcat(pool->name, usb_device->product, sizeof(pool->name));
  }

  if (strlen(pool->name) == 0) {
    snprintf(pool->name, sizeof(pool->name), "Optical touch device %04x:%04x",
             le16_to_cpu(usb_device->descriptor.idVendor),
             le16_to_cpu(usb_device->descriptor.idProduct));
  }

  snprintf(pool->class_name, sizeof(pool->class_name), "%s",
           device->variant->dev_node_fmt);

  usb_make_path(usb_device, pool->phys, sizeof(pool->phys));
  strlcat(pool->phys, "/input0", sizeof(pool->phys));

  obj->name = pool->name;
  obj->phys = pool->phys;

  usb_to_input_id(usb_device, &obj->id);
  obj->dev.parent = parent;

  // seems useless
  // input_set_drvdata(obj, device);

  obj->open = optical_open_device;
  obj->close = optical_close_device;

  obj->evbit[0] = BIT(EV_KEY) | BIT(EV_ABS);
  __set_bit(INPUT_PROP_DIRECT, obj->propbit);
  set_bit(BTN_TOUCH, obj->keybit);
  set_bit(EV_SYN, obj->evbit);
  set_bit(EV_KEY, obj->evbit);
  set_bit(EV_ABS, obj->evbit);
  obj->absbit[0] = BIT(ABS_MT_PRESSURE) | BIT(ABS_MT_POSITION_X) |
                   BIT(ABS_MT_POSITION_Y) | BIT(ABS_MT_TOUCH_MAJOR) |
                   BIT(ABS_MT_TOUCH_MINOR);

  input_set_abs_params(obj, ABS_MT_PRESSURE, 0, 1, 0, 0);
  input_set_abs_params(obj, ABS_MT_POSITION_X, 0, 32767, 0, 0);
  input_set_abs_params(obj, ABS_MT_POSITION_Y, 0, 32767, 0, 0);
  input_set_abs_params(obj, ABS_MT_TOUCH_MAJOR, 0, 32767, 0, 0);
  input_set_abs_params(obj, ABS_MT_TOUCH_MINOR, 0, 32767, 0, 0);
  input_mt_init_slots(obj, device->variant->touch_points, INPUT_MT_DIRECT);
}

static int optical_probe(struct usb_interface *intf,
                         const struct usb_device_id *id) {
  int retval;
  device_context *device;
  bool input_dev_registered = false;

  do {
    device = kzalloc(sizeof(device_context), GFP_KERNEL);
    if (device == NULL) {
      err("%s: Out of memory.", __func__);
      break;
    }
    info("%s: probe vid=0x%04x pid=0x%04x", __func__,
         le16_to_cpu(interface_to_usbdev(intf)->descriptor.idVendor),
         le16_to_cpu(interface_to_usbdev(intf)->descriptor.idProduct));
    memset(&device->class, 0, sizeof(device->class));
    device->file_private_data = NULL;
    device->disconnected = false;
    device->variant = (const struct optical_variant *)id->driver_info;
    device->class.name = device->pool.class_name;
    device->class.fops = &optical_fops;
    device->class.minor_base = OPTICAL_MINOR_BASE;

    do {
      retval = device_context_init(device, intf);
      if (retval != 0) {
        break;
      }
      device->input_dev = input_allocate_device();
      if (device->input_dev == NULL) {
        break;
      }
      do {
        spin_lock_init(&device->lock);
        init_waitqueue_head(&device->read_wait);
        device->report_packet_size =
            (unsigned int)OPTICAL_MULTITOUCH_PACKET_SIZE(
                device->variant->touch_points);
        device->buffer_capacity = max(device->max_packet_size,
                                      device->report_packet_size);
        info("%s: max_packet_size=%u report_packet_size=%u buffer_capacity=%u",
             __func__, device->max_packet_size, device->report_packet_size,
             device->buffer_capacity);
        device->buffer = kzalloc(device->buffer_capacity, GFP_KERNEL);
        if (device->buffer == NULL) {
          break;
        }
        device->ongoing_buffer =
            usb_alloc_coherent(device->usb_device, device->max_packet_size,
                               GFP_ATOMIC, &device->ongoing_buffer_dma);
        if (device->ongoing_buffer == NULL) {
          kfree(device->buffer);
          device->buffer = NULL;
          break;
        }
        do {
          device->interrupt_urb = usb_alloc_urb(0, GFP_KERNEL);
          if (device->interrupt_urb == NULL) {
            break;
          }
          do {
            usb_fill_int_urb(device->interrupt_urb, device->usb_device,
                             device->pipe_input, device->ongoing_buffer,
                             device->max_packet_size, on_interrupt, device,
                             device->pipe_interval);
            device->interrupt_urb->transfer_dma = device->ongoing_buffer_dma;
            device->interrupt_urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
            device->buffer_length = 0;
            device->interrupt_urb->dev = device->usb_device;
            input_dev_init(device->input_dev, device, &device->pool,
                           device->usb_device, &intf->dev);
            input_set_drvdata(device->input_dev, device);
            retval = input_register_device(device->input_dev);
            if (retval != 0) {
              break;
            }
            input_dev_registered = true;
            do {
              usb_set_intfdata(intf, device);
              do {
                msleep(500);
                if (usb_register_dev(intf, &device->class) != 0) {
                  break;
                }
                return 0;

              } while (false);
              usb_set_intfdata(intf, NULL);
            } while (false);
          } while (false);
          usb_free_urb(device->interrupt_urb);
        } while (false);
        usb_free_coherent(device->usb_device, device->max_packet_size,
                          device->ongoing_buffer, device->ongoing_buffer_dma);
        kfree(device->buffer);
      } while (false);
      if (device->input_dev != NULL) {
        if (input_dev_registered) {
          input_unregister_device(device->input_dev);
        } else {
          input_free_device(device->input_dev);
        }
        device->input_dev = NULL;
      }
    } while (false);
    kfree(device);
  } while (false);
  return -ENOMEM;
}

static void optical_disconnect(struct usb_interface *intf) {
  device_context *device = usb_get_intfdata(intf);

  if (device == NULL) {
    return;
  }

  info("%s", __func__);

  mutex_lock(&optical_file_lock);
  usb_deregister_dev(intf, &device->class);
  usb_set_intfdata(intf, NULL);
  device->disconnected = true;
  if (device->file_private_data != NULL) {
    device->file_private_data->private_data = NULL;
  }
  device->file_private_data = NULL;
  mutex_unlock(&optical_file_lock);

  wake_up_interruptible(&device->read_wait);

  input_unregister_device(device->input_dev);
  device->input_dev = NULL;
  usb_free_urb(device->interrupt_urb);
  usb_free_coherent(device->usb_device, device->max_packet_size,
                    device->ongoing_buffer, device->ongoing_buffer_dma);
  kfree(device->buffer);
  kfree(device);
}

static struct usb_driver optical_driver = {
    .name = DRIVER_NAME,
    .probe = optical_probe,
    .disconnect = optical_disconnect,
    .id_table = dev_table,
};

module_usb_driver(optical_driver);

MODULE_DESCRIPTION("USB driver for Optical touch screen");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Optical touch screen");
MODULE_DEVICE_TABLE(usb, dev_table);
