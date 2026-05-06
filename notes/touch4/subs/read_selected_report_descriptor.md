# read_selected_report_descriptor

Related common patterns: [[usb_control_descriptor_read]], [[descriptor_table_parser_pattern]].

Builds a temporary request descriptor for caller-selected device report id. Calls [[usb_control_read_8byte_descriptor]] with `(ctx, id, &tmp)`. If that fails, returns `0`. Then fills descriptor `{ ctx, id, &tmp }` and passes it with callback table at `0x41e8fe` into [[parse_two_part_descriptor]]. Returns `1` only when that call returns non-null.

Called by [[initialize_device_work_reports]]. Called functions: [[usb_control_read_8byte_descriptor]], [[parse_two_part_descriptor]].

```c
int read_selected_report_descriptor(void *ctx, unsigned char id, void *dst)
{
    unsigned char tmp[0x10];
    struct {
        void *ctx;
        unsigned char id;
        void *tmp;
    } desc;

    if (usb_control_read_8byte_descriptor(ctx, id, tmp) == 0)
        return 0;

    desc.ctx = ctx;
    desc.id = id;
    desc.tmp = tmp;

    if (parse_two_part_descriptor(dst, (void *)0x41e8fe, &desc) == 0)
        return 0;
    return 1;
}
```
