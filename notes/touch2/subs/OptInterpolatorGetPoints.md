# OptInterpolatorGetPoints

Debug symbol name: `OptInterpolatorGetPoints`. Produces interpolator output point list. Follows [[nonnull_wrapper_pattern]]. Called by [[DeviceProcess]].

```c
int OptInterpolatorGetPoints(void *out_points, int *count, void *interp)
{
    if (out_points == 0 || count == 0 || interp == 0)
        return 0;
    pro_InterpolatorGetInterpolation(out_points, count, interp);
    return -1;
}
```
