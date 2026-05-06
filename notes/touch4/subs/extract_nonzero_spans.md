# extract_nonzero_spans

Related common pattern: [[run_length_touch_encoding]].

Extracts low-two-bit nonzero spans from packet bytes. Output entries are 4 bytes each: `uint16_t start`, `uint16_t end`. It scans from offset `2` to packet length at offset `0x28`, accumulates position by `(byte >> 2) + 1`, writes start on zero-to-nonzero transition and end on nonzero-to-zero transition. If final state is nonzero, final end is last accumulated position.

Called by [[merge_touch_report_records]].

```c
void extract_nonzero_spans(unsigned char *packet, unsigned short *ranges, unsigned char *count)
{
    unsigned char prev = 0;
    unsigned short pos = 0;
    unsigned short i;

    *count = 0;
    for (i = 2; i < *(uint16_t *)(packet + 0x28); i++) {
        unsigned char cur = packet[i] & 3;
        if (prev == 0 && cur != 0) {
            ranges[*count * 2 + 0] = pos;
            (*count)++;
        } else if (prev != 0 && cur == 0) {
            ranges[(*count - 1) * 2 + 1] = pos;
        }
        pos += (packet[i] >> 2) + 1;
        prev = cur;
    }
    if (prev != 0)
        ranges[(*count - 1) * 2 + 1] = pos;
}
```
