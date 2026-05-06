# set_parser_point_mode

Related common pattern: [[parser_mode_state_pattern]].

Sets parser state byte `parser + 0xa479` to the low byte of the second parameter. If value is `0`, `1`, or `2`, calls [[apply_point_mode_to_sink_context]] for sub-context `parser + 0x1c8`. Mapping: `0 -> 1`, `1 -> 2`, `2 -> 3`. Other values only write the state byte.

Called by [[initialize_packet_parser]]. Called functions: [[apply_point_mode_to_sink_context]].

```c
void set_parser_point_mode(void *parser, int value)
{
    unsigned char mode = (unsigned char)value;

    *(unsigned char *)((char *)parser + 0xa479) = mode;

    switch (mode) {
    case 0:
        apply_point_mode_to_sink_context((char *)parser + 0x1c8, 1);
        break;
    case 1:
        apply_point_mode_to_sink_context((char *)parser + 0x1c8, 2);
        break;
    case 2:
        apply_point_mode_to_sink_context((char *)parser + 0x1c8, 3);
        break;
    default:
        break;
    }
}
```
