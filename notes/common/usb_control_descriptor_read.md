# usb_control_descriptor_read

Touch4 descriptor initialization uses fixed-size USB control reads.

Short descriptor read:

```c
usb_control_transfer(dev, 0x1e, 0xae, id, buf, 0, 0, 0x3a, &got, 1);
```

Selected 8-byte descriptor read:

```c
fill_buffer_with_length_byte(buf, 0x3a);
usb_control_transfer(dev, 0x1e, 0xb0, id, buf, 0, 0, 0x3a, &got, 1);
```

Successful reads require bounded returned length. Data is copied from local transfer buffer into caller output.

Used by [[usb_control_read_short_descriptor]], [[usb_control_read_8byte_descriptor]], [[read_report_id_list]], and [[read_selected_report_descriptor]].
