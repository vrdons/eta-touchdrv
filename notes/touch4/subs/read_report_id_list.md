# read_report_id_list

Related common pattern: [[usb_control_descriptor_read]].

Wrapper around [[usb_control_read_short_descriptor]]. Passes first argument, fixed request byte `0x80`, second argument, and third argument. Returns `1` when [[usb_control_read_short_descriptor]] returns nonzero, otherwise returns `0`.

Called by [[initialize_device_work_reports]]. Called functions: [[usb_control_read_short_descriptor]].

```c
int read_report_id_list(void *ctx, void *out, void *arg)
{
    if (usb_control_read_short_descriptor(ctx, 0x80, out, arg) == 0)
        return 0;
    return 1;
}
```
