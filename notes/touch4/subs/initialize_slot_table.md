# initialize_slot_table

Initializes parser slot state. If `state` is null or `count > 4`, returns `-1`. Otherwise writes mode byte and count byte, then clears active flags at each slot offsets `+0x10` and `+0x18` for `i = 0..count-1`. Returns `0` on success.

Called by [[initialize_parser_slot_state_from_work_callback]].

```c
int initialize_slot_table(void *state, unsigned char mode, unsigned char count)
{
    int i;

    if (state == 0)
        return -1;
    if (count > 4)
        return -1;

    *(unsigned char *)state = mode;
    *(unsigned char *)((char *)state + 1) = count;

    for (i = 0; i < count; i++) {
        *(uint64_t *)((char *)state + i * 0x70 + 0x10) = 0;
        *(uint64_t *)((char *)state + i * 0x70 + 0x18) = 0;
    }

    return 0;
}
```
