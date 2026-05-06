# dispatch_report_to_staging_sink

Related common pattern: [[multislot_report_assembly]].

Report sink wrapper for `ctx+0x42a0` staging state. It updates `*(int *)(ctx+0x42a0+0x154)` from first word minus `0x154`. If `ctx` is non-null, [[stage_calibrated_touch_report]] prepares staging buffer `(ctx+0x42a0+4, ctx+0x42a0+0x154)`, then [[dispatch_touch_callback]] consumes `ctx+0xd8`, staged bytes, sequence byte, and length pointer.

Called by [[process_raw_0x40_device_packet]]. Called functions: [[stage_calibrated_touch_report]], [[dispatch_touch_callback]].

```c
void dispatch_report_to_staging_sink(void *ctx, void *report, unsigned char seq)
{
    char *stage = (char *)ctx + 0x42a0;

    *(int *)(stage + 0x154) = *(int *)stage - 0x154;

    if (ctx == 0)
        return;

    stage_calibrated_touch_report(ctx, report, stage + 4, stage + 0x154);
    dispatch_touch_callback((char *)ctx + 0xd8, stage + 4, seq, stage + 0x154);
}
```
