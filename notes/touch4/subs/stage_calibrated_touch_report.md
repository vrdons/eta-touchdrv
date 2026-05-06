# stage_calibrated_touch_report

Touch report staging transform. It prepares `len_state + 0x54` from `*len_state - 0x54`, rejects null `report`, `dst`, or context, and requires `report[0xa8] == ctx[0xd0]`. It loops over report slots, writes `ctx[0xd1]` into staged slot offset `0x50`, then evaluates calibration polynomials from context per point. If coefficient at slot coefficient block offset `+4` is positive, it stores forward `(x,y)` pairs; otherwise it stores them mirrored from end of slot point array.

Called by [[dispatch_report_to_staging_sink]].

```c
void stage_calibrated_touch_report(void *ctx, unsigned char *report, unsigned char *dst, int *len_state)
{
    short i;
    short j;

    len_state[0x15] = *len_state - 0x54;

    if (report == 0)
        return;
    if (dst == 0)
        return;
    if (ctx == 0)
        return;
    if (report[0xa8] != *(unsigned char *)((char *)ctx + 0xd0))
        return;

    for (i = 0; i < report[0xa8]; i++) {
        unsigned char *slot = dst + (uint64_t)(uint16_t)i * 0x54;
        float *points = (float *)(report + (uint64_t)(uint16_t)i * 0x2a);
        float *coeff = (float *)((char *)ctx + (uint64_t)(uint16_t)i * 0x34 + 0x30);
        unsigned char n;

        slot[0x50] = *(unsigned char *)((char *)ctx + 0xd1);
        n = slot[0x50];

        if (coeff[1] > 0.0f) {
            for (j = 0; j < n; j++) {
                float x = points[j * 2 + 0];
                float y = points[j * 2 + 1];
                ((float *)slot)[j * 2 + 0] = ((((((coeff[7] * x + coeff[6]) * x + coeff[5]) * x + coeff[4]) * x + coeff[3]) * x + coeff[2]) * x + coeff[1]) * x + coeff[0];
                ((float *)slot)[j * 2 + 1] = ((((((coeff[7] * y + coeff[6]) * y + coeff[5]) * y + coeff[4]) * y + coeff[3]) * y + coeff[2]) * y + coeff[1]) * y + coeff[0];
            }
        } else {
            for (j = 0; j < n; j++) {
                float x = points[j * 2 + 1];
                float y = points[j * 2 + 0];
                int k = n - j - 1;
                ((float *)slot)[k * 2 + 0] = ((((((coeff[7] * x + coeff[6]) * x + coeff[5]) * x + coeff[4]) * x + coeff[3]) * x + coeff[2]) * x + coeff[1]) * x + coeff[0];
                ((float *)slot)[k * 2 + 1] = ((((((coeff[7] * y + coeff[6]) * y + coeff[5]) * y + coeff[4]) * y + coeff[3]) * y + coeff[2]) * y + coeff[1]) * y + coeff[0];
            }
        }
    }
}
```
