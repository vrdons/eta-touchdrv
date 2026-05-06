# OptGetOutputPoints

Debug symbol name: `OptGetOutputPoints`. If `opt_ctx == NULL`, returns without writes. If first output pointer is non-null, writes first output point from context offsets `0x42`, `0x4a`, `0x52`, valid byte `0x62`, using scale/bias floats at `0x13e`, `0x142`, `0x146`, `0x14a`, `0x14e`. If second output pointer is non-null, writes second point from offsets `0x63`, `0x6b`, `0x73`, valid byte `0x83`. Pressure/depth component is clamped at lower bound `0`.

Called by [[DeviceProcess]].

```c
void OptGetOutputPoints(void *out0, void *out1, void *opt_ctx)
{
    if (opt_ctx == 0)
        return;

    if (out0 != 0) {
        *(int *)((char *)out0 + 0x0) = (int)(*(float *)((char *)opt_ctx + 0x13e) * *(double *)((char *)opt_ctx + 0x42) + *(float *)((char *)opt_ctx + 0x142));
        *(int *)((char *)out0 + 0x4) = (int)(*(float *)((char *)opt_ctx + 0x146) * *(double *)((char *)opt_ctx + 0x4a) + *(float *)((char *)opt_ctx + 0x14a));
        *(int *)((char *)out0 + 0x8) = (int)(*(float *)((char *)opt_ctx + 0x14e) * *(double *)((char *)opt_ctx + 0x52));
        if (*(int *)((char *)out0 + 0x8) < 0)
            *(int *)((char *)out0 + 0x8) = 0;
        *(unsigned char *)((char *)out0 + 0xc) = *(unsigned char *)((char *)opt_ctx + 0x62);
    }

    if (out1 != 0) {
        *(int *)((char *)out1 + 0x0) = (int)(*(float *)((char *)opt_ctx + 0x13e) * *(double *)((char *)opt_ctx + 0x63) + *(float *)((char *)opt_ctx + 0x142));
        *(int *)((char *)out1 + 0x4) = (int)(*(float *)((char *)opt_ctx + 0x146) * *(double *)((char *)opt_ctx + 0x6b) + *(float *)((char *)opt_ctx + 0x14a));
        *(int *)((char *)out1 + 0x8) = (int)(*(float *)((char *)opt_ctx + 0x14e) * *(double *)((char *)opt_ctx + 0x73));
        if (*(int *)((char *)out1 + 0x8) < 0)
            *(int *)((char *)out1 + 0x8) = 0;
        *(unsigned char *)((char *)out1 + 0xc) = *(unsigned char *)((char *)opt_ctx + 0x83);
    }
}
```
