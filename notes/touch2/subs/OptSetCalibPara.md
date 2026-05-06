# OptSetCalibPara

Related common pattern: [[calibration_polynomial_transform]].

Debug symbol name: `OptSetCalibPara`. Copies calibration coefficients supplied on the stack into optical context. It also computes calibration scale at `opt_ctx+0x13a` from two copied coefficients. Returns `0` when `opt_ctx` is null, otherwise returns `-1`.

```c
int OptSetCalibPara(void *opt_ctx, ...)
{
    float a;
    float b;

    if (opt_ctx == 0)
        return 0;

    copy_calibration_args_to_context((char *)opt_ctx + 0xc6);

    a = *(float *)((char *)opt_ctx + 0xe6);
    b = *(float *)((char *)opt_ctx + 0x11c);
    *(float *)((char *)opt_ctx + 0x13a) = sqrtf(a * a + b * b);
    return -1;
}
```
