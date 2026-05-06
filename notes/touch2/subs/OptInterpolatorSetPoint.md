# OptInterpolatorSetPoint

Debug symbol name: `OptInterpolatorSetPoint`. Updates interpolator input point/state. Follows [[nonnull_wrapper_pattern]]. Called by [[DeviceProcess]].

```c
int OptInterpolatorSetPoint(void *point, void *interp)
{
    if (point == 0 || interp == 0)
        return 0;
    pro_InterpolatorSetPoint(point, interp);
    return -1;
}
```
