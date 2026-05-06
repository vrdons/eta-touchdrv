# read_base_report_descriptor_0x80

Builds a temporary request descriptor for fixed device report id `0x80`. First calls [[usb_control_read_8byte_descriptor]] with `(ctx, 0x80, &tmp)`. If that fails, returns `0`. Then fills descriptor `{ ctx, 0x80, &tmp }` and passes it with callback table at `0x41e8fe` into [[initialize_large_descriptor_table]]. Returns `1` only when that call returns non-null.

Called by [[initialize_device_work_reports]]. Called functions: [[usb_control_read_8byte_descriptor]], [[initialize_large_descriptor_table]].

```c
int read_base_report_descriptor_0x80(void *ctx, void *dst)
{
    unsigned char tmp[0x10];
    struct {
        void *ctx;
        unsigned char id;
        void *tmp;
    } desc;

    if (usb_control_read_8byte_descriptor(ctx, 0x80, tmp) == 0)
        return 0;

    desc.ctx = ctx;
    desc.id = 0x80;
    desc.tmp = tmp;

    if (initialize_large_descriptor_table(dst, (void *)0x41e8fe, &desc) == 0)
        return 0;
    return 1;
}
```
