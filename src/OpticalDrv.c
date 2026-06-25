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

static unsigned int touch_antishake_radius = 24;
module_param_named(touch_antishake_radius, touch_antishake_radius, uint, 0644);
MODULE_PARM_DESC(touch_antishake_radius,
                 "Deadband radius for touch jitter suppression");

static unsigned int touch_smoothing_alpha = 128;
module_param_named(touch_smoothing_alpha, touch_smoothing_alpha, uint, 0644);
MODULE_PARM_DESC(touch_smoothing_alpha,
                 "Low-pass EMA factor in fixed-point (0-256): "
                 "256=off, 128=0.5 blend, 0=max smooth");

static bool touch_adaptive_enabled = true;
module_param_named(touch_adaptive_enabled, touch_adaptive_enabled, bool, 0644);
MODULE_PARM_DESC(touch_adaptive_enabled,
                 "Enable adaptive alpha: outlier snap + speed-based smoothing");

static unsigned int touch_outlier_factor = 3;
module_param_named(touch_outlier_factor, touch_outlier_factor, uint, 0644);
MODULE_PARM_DESC(touch_outlier_factor,
                 "Outlier multiplier: snap when delta > k * moving_avg_speed");

static unsigned int touch_drop_threshold_ms = 80;
module_param_named(touch_drop_threshold_ms, touch_drop_threshold_ms, uint, 0644);
MODULE_PARM_DESC(touch_drop_threshold_ms,
                 "Packet-drop threshold in ms: freeze when event gap exceeds this");

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

static int submit_urb(device_context *device, gfp_t gfp_mask) {
  int retval;

  retval = usb_submit_urb(device->interrupt_urb, gfp_mask);
  if (retval != 0) {
    err("%s: usb_submit_urb failed: %d", __func__, retval);
  }
  return retval;
}
static void cancel_urb(device_context *device) {
  usb_kill_urb(device->interrupt_urb);
}

static bool optical_has_active_consumer(device_context *device) {
  return device->file_private_data != NULL || device->input_device_open;
}

static int optical_ensure_urb_started(device_context *device, gfp_t gfp_mask) {
  int retval;

  mutex_lock(&optical_file_lock);
  if (device->disconnected) {
    mutex_unlock(&optical_file_lock);
    return -ENODEV;
  }
  if (device->urb_submitted) {
    mutex_unlock(&optical_file_lock);
    return 0;
  }
  device->urb_submitted = true;
  mutex_unlock(&optical_file_lock);

  retval = submit_urb(device, gfp_mask);
  if (retval == 0)
    return 0;

  mutex_lock(&optical_file_lock);
  device->urb_submitted = false;
  mutex_unlock(&optical_file_lock);
  return retval;
}

static void optical_maybe_stop_urb(device_context *device) {
  bool need_cancel;

  mutex_lock(&optical_file_lock);
  need_cancel = !device->disconnected && device->urb_submitted &&
                !optical_has_active_consumer(device);
  if (need_cancel)
    device->urb_submitted = false;
  mutex_unlock(&optical_file_lock);

  if (need_cancel)
    cancel_urb(device);
}

static void reset_touch_filter(device_context *device, size_t slot) {
  device->touch_filter[slot].primed = false;
  device->touch_filter[slot].x = 0;
  device->touch_filter[slot].y = 0;
  device->touch_filter[slot].last_event_jiffies = 0;
  device->touch_filter[slot].ring_idx = 0;
  device->touch_filter[slot].ring_count = 0;
  device->touch_filter[slot].sum_dx = 0;
  device->touch_filter[slot].sum_dy = 0;
}

static void touch_filter_push_sample(device_context *device, size_t slot,
                                     int dx, int dy) {
  int old_dx;
  int old_dy;
  unsigned int idx;

  idx = device->touch_filter[slot].ring_idx;
  if (device->touch_filter[slot].ring_count == OPTICAL_FILTER_RING_SIZE) {
    old_dx = device->touch_filter[slot].dx_ring[idx];
    old_dy = device->touch_filter[slot].dy_ring[idx];
    device->touch_filter[slot].sum_dx -= old_dx;
    device->touch_filter[slot].sum_dy -= old_dy;
  } else {
    device->touch_filter[slot].ring_count++;
  }
  device->touch_filter[slot].dx_ring[idx] = dx;
  device->touch_filter[slot].dy_ring[idx] = dy;
  device->touch_filter[slot].sum_dx += dx;
  device->touch_filter[slot].sum_dy += dy;
  device->touch_filter[slot].ring_idx = (idx + 1) & (OPTICAL_FILTER_RING_SIZE - 1);
}

static int touch_filter_adaptive_alpha(device_context *device, size_t slot,
                                       int dx, int dy, bool *freeze) {
  unsigned long now;
  unsigned long elapsed;
  long avg_dx;
  long avg_dy;
  long avg_speed_sq;
  long delta_sq;
  long k_sq;
  int base;

  *freeze = false;
  base = (int)touch_smoothing_alpha;

  if (!touch_adaptive_enabled)
    return base;

  now = jiffies;

  if (device->touch_filter[slot].last_event_jiffies != 0 &&
      touch_drop_threshold_ms != 0) {
    elapsed = jiffies_to_msecs(
        now - device->touch_filter[slot].last_event_jiffies);
    if (elapsed > touch_drop_threshold_ms) {
      *freeze = true;
      return 0;
    }
  }

  if (device->touch_filter[slot].ring_count == 0)
    return base;

  avg_dx = device->touch_filter[slot].sum_dx /
           (long)device->touch_filter[slot].ring_count;
  avg_dy = device->touch_filter[slot].sum_dy /
           (long)device->touch_filter[slot].ring_count;
  avg_speed_sq = avg_dx * avg_dx + avg_dy * avg_dy;
  delta_sq = (long)dx * dx + (long)dy * dy;

  k_sq = (long)touch_outlier_factor * touch_outlier_factor;
  if (avg_speed_sq > 0 && delta_sq > k_sq * avg_speed_sq)
    return 256;

  if (avg_speed_sq > 64 * 64)
    return min(base + 64, 256);
  if (avg_speed_sq < 4 * 4)
    return max(base - 64, 0);
  return base;
}

static void apply_touch_filter(device_context *device, size_t slot,
                               signed short *x, signed short *y,
                               bool touched) {
  int dx;
  int dy;
  unsigned int radius_sq;
  unsigned int delta_sq;
  int prev_x;
  int prev_y;
  int alpha;
  bool freeze;

  if (!touched) {
    reset_touch_filter(device, slot);
    return;
  }

  if (!device->touch_filter[slot].primed) {
    device->touch_filter[slot].primed = true;
    device->touch_filter[slot].x = *x;
    device->touch_filter[slot].y = *y;
    device->touch_filter[slot].last_event_jiffies = jiffies;
    return;
  }

  prev_x = device->touch_filter[slot].x;
  prev_y = device->touch_filter[slot].y;
  dx = (int)*x - prev_x;
  dy = (int)*y - prev_y;
  radius_sq = touch_antishake_radius * touch_antishake_radius;
  delta_sq = (unsigned int)(dx * dx + dy * dy);
  if (touch_antishake_radius != 0 && delta_sq <= radius_sq) {
    *x = (signed short)prev_x;
    *y = (signed short)prev_y;
    device->touch_filter[slot].last_event_jiffies = jiffies;
    return;
  }

  alpha = touch_filter_adaptive_alpha(device, slot, dx, dy, &freeze);
  if (freeze) {
    *x = (signed short)prev_x;
    *y = (signed short)prev_y;
    device->touch_filter[slot].last_event_jiffies = jiffies;
    return;
  }
  if (alpha < 256) {
    int filtered_x = (alpha * (int)*x + (256 - alpha) * prev_x) / 256;
    int filtered_y = (alpha * (int)*y + (256 - alpha) * prev_y) / 256;
    *x = (signed short)filtered_x;
    *y = (signed short)filtered_y;
  }

  touch_filter_push_sample(device, slot, dx, dy);
  device->touch_filter[slot].x = *x;
  device->touch_filter[slot].y = *y;
  device->touch_filter[slot].last_event_jiffies = jiffies;
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
  if (count > sizeof(local_buf)) {
    count = sizeof(local_buf);
    dev_warn_ratelimited(&device->usb_device->dev,
                         "%s: read truncated to %zu bytes (had %zu available)",
                         __func__, count, avail);
  }
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

  if (length < 1) {
    return -EINVAL;
  }

  kernel_data = kzalloc(length, GFP_KERNEL);
  if (kernel_data == NULL) {
    return -ENOMEM;
  }
  r = copy_from_user(kernel_data, data, length);
  if (r != 0) {
    kfree(kernel_data);
    return -EFAULT;
  }
  r = usb_control_msg(device->usb_device,
                      usb_sndctrlpipe(device->usb_device, 0), 0, 0x40, 0, 0,
                      kernel_data, length, 1000);
  kfree(kernel_data);
  return r;
}
static long get_report(device_context *device, unsigned short length,
                       void *data) {
  void *kernel_data;
  int r;

  if (length < 1) {
    return -EINVAL;
  }

  kernel_data = kzalloc(length, GFP_KERNEL);
  if (kernel_data == NULL) {
    return -ENOMEM;
  }
  r = usb_control_msg(device->usb_device,
                      usb_rcvctrlpipe(device->usb_device, 0), 0, 0xc0, 0, 0,
                      kernel_data, length, 1000);
  if (r >= 0) {
    if (copy_to_user(data, kernel_data, r) != 0) {
      r = -EFAULT;
    }
  }
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

  for (i = 0; i < touch_point_count; i++) {
    bool touched;

    if ((touch_points[i].state & OpticalReportTouchPointStateFlag_IsValid) ==
        0) {
      // Daemons can send partial updates; invalid slots mean no change.
      continue;
    }

    input_mt_slot(device->input_dev, i);

    touched = (touch_points[i].state & OpticalReportTouchPointStateFlag_IsTouched) !=
              0;
    input_mt_report_slot_state(device->input_dev, MT_TOOL_FINGER, touched);

    if (touched) {
      signed short x = touch_points[i].x;
      signed short y = touch_points[i].y;

      apply_touch_filter(device, i, &x, &y, true);
      device->active_touch_slots |= BIT(i);
      input_report_abs(device->input_dev, ABS_MT_TOUCH_MAJOR,
                       touch_points[i].width);
      input_report_abs(device->input_dev, ABS_MT_TOUCH_MINOR,
                       touch_points[i].height);
      input_report_abs(device->input_dev, ABS_MT_POSITION_X, x);
      input_report_abs(device->input_dev, ABS_MT_POSITION_Y, y);
    } else {
      device->active_touch_slots &= ~BIT(i);
      reset_touch_filter(device, i);
    }
  }

  input_report_key(device->input_dev, BTN_TOUCH,
                   device->active_touch_slots != 0);
  input_sync(device->input_dev);
  return 0;
}

static void release_touch_points(device_context *device) {
  size_t i;

  if (device->input_dev == NULL)
    return;

  for (i = 0; i < device->variant->touch_points; i++) {
    input_mt_slot(device->input_dev, i);
    input_mt_report_slot_state(device->input_dev, MT_TOOL_FINGER, false);
    reset_touch_filter(device, i);
  }

  device->active_touch_slots = 0;
  input_report_key(device->input_dev, BTN_TOUCH, 0);
  input_sync(device->input_dev);
}

static long sync_singletouch(device_context *device, unsigned short length,
                             void const *data) {
  OpticalReportPacketSingleTouch value;
  int r;

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
  int retval;
  int subminor;

  subminor = iminor(inode);

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

  retval = optical_ensure_urb_started(device, GFP_KERNEL);
  if (retval == 0)
    return 0;

  mutex_lock(&optical_file_lock);
  if (device->file_private_data == filp)
    device->file_private_data = NULL;
  filp->private_data = NULL;
  mutex_unlock(&optical_file_lock);
  return retval;
}

static int optical_release(struct inode *inode, struct file *filp) {
  device_context *device;

  mutex_lock(&optical_file_lock);
  device = filp->private_data;
  if (device != NULL) {
    release_touch_points(device);
    device->file_private_data = NULL;
  }
  filp->private_data = NULL;
  mutex_unlock(&optical_file_lock);

  if (device != NULL)
    optical_maybe_stop_urb(device);

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

  mutex_lock(&optical_file_lock);
  if (device->disconnected || !device->urb_submitted ||
      !optical_has_active_consumer(device)) {
    device->urb_submitted = false;
    mutex_unlock(&optical_file_lock);
    return;
  }
  mutex_unlock(&optical_file_lock);

  if (submit_urb(device, GFP_ATOMIC) != 0) {
    mutex_lock(&optical_file_lock);
    device->urb_submitted = false;
    mutex_unlock(&optical_file_lock);
  }
}

static int optical_open_device(struct input_dev *input_dev) {
  device_context *device;

  device = input_get_drvdata(input_dev);

  mutex_lock(&optical_file_lock);
  device->input_device_open = true;
  mutex_unlock(&optical_file_lock);

  return optical_ensure_urb_started(device, GFP_KERNEL);
}

static void optical_close_device(struct input_dev *input_dev) {
  device_context *device;

  device = input_get_drvdata(input_dev);

  mutex_lock(&optical_file_lock);
  device->input_device_open = false;
  mutex_unlock(&optical_file_lock);

  optical_maybe_stop_urb(device);
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
  device_context *device;
  int retval;

  device = kzalloc(sizeof(device_context), GFP_KERNEL);
  if (device == NULL) {
    err("%s: Out of memory.", __func__);
    return -ENOMEM;
  }

  device->file_private_data = NULL;
  device->disconnected = false;
  device->variant = (const struct optical_variant *)id->driver_info;
  device->class.name = device->pool.class_name;
  device->class.fops = &optical_fops;
  device->class.minor_base = OPTICAL_MINOR_BASE;

  retval = device_context_init(device, intf);
  if (retval != 0)
    goto err_free_device;

  device->input_dev = input_allocate_device();
  if (device->input_dev == NULL) {
    retval = -ENOMEM;
    goto err_free_device;
  }

  spin_lock_init(&device->lock);
  init_waitqueue_head(&device->read_wait);
  device->urb_submitted = false;
  device->input_device_open = false;
  device->report_packet_size =
      (unsigned int)OPTICAL_MULTITOUCH_PACKET_SIZE(
          device->variant->touch_points);
  device->buffer_capacity = max(device->max_packet_size,
                                device->report_packet_size);
  device->buffer = kzalloc(device->buffer_capacity, GFP_KERNEL);
  if (device->buffer == NULL) {
    retval = -ENOMEM;
    goto err_free_input;
  }
  device->ongoing_buffer =
      usb_alloc_coherent(device->usb_device, device->max_packet_size,
                         GFP_ATOMIC, &device->ongoing_buffer_dma);
  if (device->ongoing_buffer == NULL) {
    retval = -ENOMEM;
    goto err_free_buffer;
  }

  device->interrupt_urb = usb_alloc_urb(0, GFP_KERNEL);
  if (device->interrupt_urb == NULL) {
    retval = -ENOMEM;
    goto err_free_coherent;
  }

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
  if (retval != 0)
    goto err_free_urb;

  usb_set_intfdata(intf, device);
  msleep(500);
  retval = usb_register_dev(intf, &device->class);
  if (retval != 0)
    goto err_unreg_input;

  return 0;

err_unreg_input:
  usb_set_intfdata(intf, NULL);
  input_unregister_device(device->input_dev);
  device->input_dev = NULL;
err_free_urb:
  usb_free_urb(device->interrupt_urb);
err_free_coherent:
  usb_free_coherent(device->usb_device, device->max_packet_size,
                    device->ongoing_buffer, device->ongoing_buffer_dma);
err_free_buffer:
  kfree(device->buffer);
err_free_input:
  if (device->input_dev != NULL)
    input_free_device(device->input_dev);
err_free_device:
  kfree(device);
  return retval;
}

static void optical_disconnect(struct usb_interface *intf) {
  device_context *device = usb_get_intfdata(intf);

  if (device == NULL) {
    return;
  }


  mutex_lock(&optical_file_lock);
  usb_deregister_dev(intf, &device->class);
  usb_set_intfdata(intf, NULL);
  device->disconnected = true;
  device->urb_submitted = false;
  device->input_device_open = false;
  if (device->file_private_data != NULL) {
    device->file_private_data->private_data = NULL;
  }
  device->file_private_data = NULL;
  mutex_unlock(&optical_file_lock);

  wake_up_interruptible(&device->read_wait);

  release_touch_points(device);
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
