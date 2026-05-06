# OptInitialize

Debug symbol name: `OptInitialize`. Returns `0` when `opt_ctx == NULL`. Otherwise initializes screen properties, default calibration, camera properties, and two smoothing contexts. Any failed sub-init clears the success flag. Success path returns `-1`; [[main]] treats that as truthy success.

Called by [[main]]. Called functions include [[OptSetCalibParaDefault]].

```c
int OptInitialize(void *opt_ctx)
{
    int ok;

    if (opt_ctx == 0)
        return 0;

    ok = pro_InitScrprops((char *)opt_ctx + 0xc6, stack_scrprops);
    if (OptSetCalibParaDefault(opt_ctx) == 0)
        ok = 0;
    else if (ok != 0)
        ok = -1;

    if (pro_InitCamprops((char *)opt_ctx + 0x156, stack_camprops) == 0)
        ok = 0;
    if (pro_InitSCtx((char *)opt_ctx + 0x1aa) == 0)
        ok = 0;
    if (pro_InitSCtx((char *)opt_ctx + 0x820) == 0)
        ok = 0;

    *(unsigned char *)((char *)opt_ctx + 0xeb6) = 0;
    *(unsigned char *)((char *)opt_ctx + 0xeb7) = 0;
    *(unsigned char *)((char *)opt_ctx + 0xed8) = 0;
    *(unsigned char *)((char *)opt_ctx + 0xed9) = 0;
    *(unsigned char *)((char *)opt_ctx + 0x41) = 0;
    *(unsigned char *)((char *)opt_ctx + 0x20) = 0;
    *(unsigned char *)((char *)opt_ctx + 0x83) = 0;
    *(unsigned char *)((char *)opt_ctx + 0x62) = 0;
    *(unsigned char *)((char *)opt_ctx + 0xc5) = 0;
    *(unsigned char *)((char *)opt_ctx + 0xa4) = 0;
    *(int *)((char *)opt_ctx + 0x1a6) = 0;

    return ok;
}
```
