# initialize_parser_slot_state_from_work_callback

Parser setup step inside [[initialize_packet_parser]]. If function pointer at `work + 0x68` is null, returns `0`. Otherwise calls callback with `work` and stack buffer. After callback, reads stack buffer offset `0x21` and checks `parser`, constant `2`, and this value through [[initialize_slot_table]]. Returns `0` if [[initialize_slot_table]] is true, `1` if false.

Called functions: indirect callback at `work + 0x68`, [[initialize_slot_table]].

```c
int initialize_parser_slot_state_from_work_callback(void *parser, void *work)
{
    unsigned char tmp[0xa0];
    void (*callback)(void *, unsigned char *);
    unsigned char value;

    callback = *(void (**)(void *, unsigned char *))((char *)work + 0x68);
    if (callback == 0)
        return 0;

    callback(work, tmp);
    value = tmp[0x21];

    if (initialize_slot_table(parser, 2, value))
        return 0;

    return 1;
}
```
