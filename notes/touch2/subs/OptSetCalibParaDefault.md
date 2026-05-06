# OptSetCalibParaDefault

Related common pattern: [[calibration_polynomial_transform]].

Debug symbol name: `OptSetCalibParaDefault`. Writes default calibration coefficients into optical context when device calibration is unavailable. Returns `0` when `opt_ctx` is null, otherwise returns `-1`.

```c
int OptSetCalibParaDefault(void *opt_ctx)
{
    float *x;
    float *y;

    if (opt_ctx == 0)
        return 0;

    x = (float *)((char *)opt_ctx + 0xc6);
    y = (float *)((char *)opt_ctx + 0x100);

    x[0] = 0.0f;
    x[1] = 0.0f;
    x[2] = 0.0f;
    x[3] = 0.0f;
    x[4] = 0.0f;
    x[5] = 0.0f;
    x[6] = 0.0f;
    x[7] = 0.0f;
    x[8] = 1.0f;
    x[9] = 0.0f;

    y[0] = 0.0f;
    y[1] = 0.0f;
    y[2] = 0.0f;
    y[3] = 0.0f;
    y[4] = 0.0f;
    y[5] = 0.0f;
    y[6] = 0.0f;
    y[7] = 1.0f;
    y[8] = 0.0f;
    y[9] = 0.0f;

    *(float *)((char *)opt_ctx + 0x13a) = 1.0f;
    return -1;
}
```
