# OptTranslatePoint

Related common pattern: [[calibration_polynomial_transform]].

Debug symbol name: `OptTranslatePoint`. Converts raw/interpolated optical point data into screen coordinates using two cubic calibration polynomials and a distance scale. [[DeviceProcess]] clamps results to `0..0x7fff` after this call.

```c
void OptTranslatePoint(unsigned long raw, int distance, int *out_x, int *out_y, int *out_d, void *opt_ctx)
{
    float x;
    float y;
    float tx;
    float ty;

    if (opt_ctx == 0 || out_x == 0 || out_y == 0 || out_d == 0)
        return;

    x = (float)(int)raw;
    y = (float)(int)(raw >> 32);

    tx = cubic_calibration_eval((float *)((char *)opt_ctx + 0xc6), x, y);
    ty = cubic_calibration_eval((float *)((char *)opt_ctx + 0x100), x, y);

    *out_x = (int)tx;
    *out_y = (int)ty;
    *out_d = (int)((double)((float)distance * *(float *)((char *)opt_ctx + 0x13a)) + 0.5);
}
```
