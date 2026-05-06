# set_parser_report_mode

Related common pattern: [[parser_mode_state_pattern]].

Sets parser state byte `parser + 0xa478` to the low byte of the second parameter. If value is `0xf0`, `0xf1`, or `0xf2`, calls [[apply_report_mode_to_sink_context]] for sub-context `parser + 0x1c8`. Mapping: `0xf0 -> 2`, `0xf1 -> 1`, `0xf2 -> 0`. Other values only write the state byte.

Called by [[initialize_packet_parser]]. Called functions: [[apply_report_mode_to_sink_context]].

```c
void set_parser_report_mode(void *parser, int value)
{
    unsigned char mode = (unsigned char)value;

    *(unsigned char *)((char *)parser + 0xa478) = mode;

    switch (mode) {
    case 0xf0:
        apply_report_mode_to_sink_context((char *)parser + 0x1c8, 2);
        break;
    case 0xf1:
        apply_report_mode_to_sink_context((char *)parser + 0x1c8, 1);
        break;
    case 0xf2:
        apply_report_mode_to_sink_context((char *)parser + 0x1c8, 0);
        break;
    default:
        break;
    }
}
```
