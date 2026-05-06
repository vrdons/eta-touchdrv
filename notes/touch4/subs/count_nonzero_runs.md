# count_nonzero_runs

Related common pattern: [[run_length_touch_encoding]].

Counts rising nonzero runs in packet bytes from offset `2` up to `*(uint16_t *)(packet+0x28)`. Only transitions from previous low two bits `0` to current low two bits nonzero increment count.

Called by [[merge_touch_report_records]].

```c
unsigned char count_nonzero_runs(unsigned char *packet)
{
    unsigned char prev = 0;
    unsigned char count = 0;
    unsigned short i;

    for (i = 2; i < *(uint16_t *)(packet + 0x28); i++) {
        unsigned char cur = packet[i] & 3;
        if (prev == 0 && cur != 0)
            count++;
        prev = cur;
    }

    return count;
}
```
