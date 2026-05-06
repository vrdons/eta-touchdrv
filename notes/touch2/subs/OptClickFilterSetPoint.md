# OptClickFilterSetPoint

Debug symbol name: `OptClickFilterSetPoint`. Writes raw output point list into click-filter state. Follows [[nonnull_wrapper_pattern]]. Called by [[DeviceProcess]].

```c
int OptClickFilterSetPoint(void *points, void *filter)
{
    if (points == 0 || filter == 0)
        return 0;
    pro_ClickFilterSetPoint(points, filter);
    return -1;
}
```
