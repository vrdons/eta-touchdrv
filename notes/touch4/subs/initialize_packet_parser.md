# initialize_packet_parser

Packet/parser initialization wrapper. First parameter is parser/context substructure, second parameter is main device context. Clears `parser+0xa470`. Prepares `parser+0xa480` work area through [[initialize_device_work_reports]], [[initialize_parser_slot_state_from_work_callback]], and [[initialize_decoder_processor]]. Returns `0` if any step fails.

On success, sets `parser+0xa470 = 1`, applies parameter `0xf0` through [[set_parser_report_mode]], applies parameter `2` through [[set_parser_point_mode]], and returns `1`. [[main]] calls this function after opening the device.

Called functions: [[initialize_device_work_reports]], [[initialize_parser_slot_state_from_work_callback]], [[initialize_decoder_processor]], [[set_parser_report_mode]], [[set_parser_point_mode]].

```c
int initialize_packet_parser(void *parser, void *ctx)
{
    *(long *)((char *)parser + 0xa470) = 0;

    if (initialize_device_work_reports(ctx, (char *)parser + 0xa480) == 0)
        return 0;
    if (initialize_parser_slot_state_from_work_callback(parser, (char *)parser + 0xa480) == 0)
        return 0;
    if (initialize_decoder_processor((char *)parser + 0x1c8, ctx, (char *)parser + 0xa480) == 0)
        return 0;

    *(long *)((char *)parser + 0xa470) = 1;
    set_parser_report_mode(parser, 0xf0);
    set_parser_point_mode(parser, 2);
    return 1;
}
```
