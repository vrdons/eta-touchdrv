# OptClickFilterGetPoints

Debug symbol name: `OptClickFilterGetPoints`. Produces click-filter output point list. Follows [[nonnull_wrapper_pattern]]. Called by [[DeviceProcess]].

```c
int OptClickFilterGetPoints(void *out_points, int *count, void *filter)
{
    if (out_points == 0 || count == 0 || filter == 0)
        return 0;
    pro_ClickFilterGetPoints(out_points, count, filter);
    return -1;
}
```
