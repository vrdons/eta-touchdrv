# descriptor_table_parser_pattern

Touch4 descriptor parsers use `(table, selector, ctx, dst, size)` lower parser calls.

Common shape:

```c
if (parse_descriptor_block(table, selector, ctx, dst + offset, size) == 0)
    return 0;
```

Some blocks are required and fail the caller when absent. Optional blocks install handler pointers when present and clear handler slots when absent.

Used by [[parse_two_part_descriptor]] and [[initialize_large_descriptor_table]].
