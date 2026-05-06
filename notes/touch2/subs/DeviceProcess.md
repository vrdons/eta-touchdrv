# DeviceProcess

Debug symbol name: `DeviceProcess`. Takes device process context, raw USB buffer, and buffer length. If packet header `buf[2] != 1`, returns immediately. If `buf[4] == 0`, copies incoming chunk to context offset `+0x8`, stores length at `+0x48`, and waits for the next chunk.

When `buf[4] != 0`, it first checks whether previous chunk exists at `ctx+0x48`. If absent, returns. Otherwise it decodes previous chunk and current buffer through [[decode]] into two code planes. Depending on context flag, [[OptProcessCode]] is called with the two planes in one of two orders.

Then [[OptGetOutputPoints]] produces raw output points. Points pass through [[OptClickFilterSetPoint]] and [[OptInterpolatorSetPoint]], then filtered/interpolated point lists are fetched with [[OptClickFilterGetPoints]] and [[OptInterpolatorGetPoints]].

For each point, [[OptTranslatePoint]] converts to screen coordinates. X/Y are clamped to `0..0x7fff`. Result is packed into 9-byte HID-like report fields. Final stage sends request `0x220014` through `ioctl` on fd and updates last point state fields.

Called functions: [[decode]], `__stack_chk_fail`, `ioctl`, [[OptClickFilterGetPoints]], [[OptClickFilterSetPoint]], [[OptInterpolatorSetPoint]], [[OptInterpolatorGetPoints]], [[OptInterpolatorGetInsertCnt]], [[OptProcessCode]], [[OptTranslatePoint]], [[OptGetOutputPoints]].

```c
void DeviceProcess(void *ctx, unsigned char *buf, unsigned char len)
{
    if (buf[2] != 1)
        return;

    if (buf[4] == 0) {
        if (len != 0)
            memcpy((char *)ctx + 8, buf, len);
        *(unsigned char *)((char *)ctx + 0x48) = len;
        return;
    }

    if (*(unsigned char *)((char *)ctx + 0x48) == 0)
        return;

    decode((char *)ctx + 8, plane0);
    decode(buf, plane1);
    process_touch_planes_and_emit_reports(ctx, plane0, plane1);
}
```
