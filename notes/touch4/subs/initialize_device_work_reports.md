# initialize_device_work_reports

Parser/device initialization step inside [[initialize_packet_parser]]. First binds `ctx` and `work` through [[read_base_report_descriptor_0x80]]. Returns `0` on failure. Then reads metadata/config into `work + 0xb60` through [[read_report_id_list]]. Returns `0` on failure.

Loops for `work[0xb60]` count entries. For each entry, computes target `work + 0x800 + i * 0x6b + 8`, takes id from config byte on stack, and calls [[read_selected_report_descriptor]]. Returns `0` on any failure, `1` if all entries succeed.

Called functions: [[read_base_report_descriptor_0x80]], [[read_report_id_list]], [[read_selected_report_descriptor]].

```c
int initialize_device_work_reports(void *ctx, unsigned char *work)
{
    unsigned char ids[16];
    long i;

    if (read_base_report_descriptor_0x80(ctx, work) == 0)
        return 0;

    if (read_report_id_list(ctx, ids, work + 0xb60) == 0)
        return 0;

    i = 0;
    while (i < work[0xb60]) {
        unsigned char *dst = work + 0x800 + i * 0x6b + 8;
        if (read_selected_report_descriptor(ctx, ids[i], dst) == 0)
            return 0;
        i++;
    }

    return 1;
}
```
