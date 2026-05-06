# range_separation_distance

Computes separation distance between two `uint16_t start/end` ranges. If second starts after or at first end, returns `second.start - first.end`. Else if first starts after or at second end, returns `first.start - second.end`. Overlapping ranges return `0`.

Called by [[merge_touch_report_records]].

```c
int range_separation_distance(unsigned short *a, unsigned short *b)
{
    if (b[0] >= a[1])
        return b[0] - a[1];
    if (a[0] >= b[1])
        return a[0] - b[1];
    return 0;
}
```
