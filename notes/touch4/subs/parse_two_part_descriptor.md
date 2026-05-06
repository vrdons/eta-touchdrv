# parse_two_part_descriptor

Related common pattern: [[descriptor_table_parser_pattern]].

Two-step table/parser helper. It first parses or fills 0x19 bytes into destination using table descriptor and context. If that fails, returns `0`. Then it parses/fills `0x52` bytes at destination offset `0x19` with selector `1`. Returns `1` only if both lower parser calls return non-null.

Called by [[read_selected_report_descriptor]].

```c
int parse_two_part_descriptor(void *dst, void *table, void *ctx)
{
    if (parse_descriptor_block(table, 0, ctx, dst, 0x19) == 0)
        return 0;
    if (parse_descriptor_block(table, 1, ctx, (char *)dst + 0x19, 0x52) == 0)
        return 0;
    return 1;
}
```
