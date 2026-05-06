# initialize_large_descriptor_table

Related common pattern: [[descriptor_table_parser_pattern]].

Large descriptor/table initializer. If destination or table pointer is null, returns `0`. It calls lower parser helper with fixed selectors, offsets, and sizes. Present blocks install handler pointers into destination; absent optional blocks clear their handler slots. Returns destination pointer after initializing known descriptor slots.

Called by [[read_base_report_descriptor_0x80]].

```c
void *initialize_large_descriptor_table(void *dst, void *table, void *ctx)
{
    if (dst == 0 || table == 0)
        return 0;

    if (parse_descriptor_block(table, 0, ctx, (char *)dst + 0x10, 0x19) != 0)
        *(void **)dst = (void *)0x41bec9;
    else
        *(void **)dst = 0;

    if (parse_descriptor_block(table, 0x0c, ctx, (char *)dst + 0x40, 0x22) != 0)
        *(void **)((char *)dst + 0x30) = (void *)0x41bf6c;
    else
        *(void **)((char *)dst + 0x30) = 0;

    initialize_remaining_descriptor_slots(dst, table, ctx);
    return dst;
}
```
