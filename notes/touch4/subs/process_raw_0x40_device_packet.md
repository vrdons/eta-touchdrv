# process_raw_0x40_device_packet

Related common pattern: [[multislot_report_assembly]].

0x40 byte raw device packet parser. [[main]] calls this when `read(fd, buf, 0x40)` returns `0x40`.

If `parser+0xa478` is `0xfe` or `0xff`, function returns immediately. If `buf[3] == 0`, function also returns. Otherwise it copies `buf+5` into a local decoded/expanded packet area with [[byte_copy]], then requires `parser+0xa470 != 0`. It passes packet fields `buf[2]`, `buf[4]`, copied payload, and output locals to [[assemble_multislot_packet]]. If returned local flag byte is nonzero, it increments `parser+0xa468` and feeds produced report at local `rbp-0xe0` into `parser+0x1c8` through [[dispatch_report_to_staging_sink]].

Called by [[main]]. Uses parser initialized by [[initialize_packet_parser]]. Called functions: [[byte_copy]], [[assemble_multislot_packet]], [[dispatch_report_to_staging_sink]].

```c
void process_raw_0x40_device_packet(void *parser, unsigned char *buf, unsigned char len)
{
    unsigned char state;
    unsigned short payload_len;
    unsigned char copied_payload[0x20];
    unsigned char report_valid;
    unsigned char report[0xc0];

    state = *(unsigned char *)((char *)parser + 0xa478);
    if (state == 0xfe)
        return;
    if (state == 0xff)
        return;
    if (buf[3] == 0)
        return;

    payload_len = (unsigned short)(buf[3] - 1);
    byte_copy(buf + 5, copied_payload, payload_len);

    if (*(uint64_t *)((char *)parser + 0xa470) == 0)
        return;

    assemble_multislot_packet(parser, buf[2], buf[4], copied_payload, &report_valid, report);
    if (report_valid == 0)
        return;

    *(unsigned char *)((char *)parser + 0xa468) =
        (unsigned char)(*(unsigned char *)((char *)parser + 0xa468) + 1);

    dispatch_report_to_staging_sink((char *)parser + 0x1c8, report,
               *(unsigned char *)((char *)parser + 0xa468));
}
```
