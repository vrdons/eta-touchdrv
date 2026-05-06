# OptClickFilterSetParameter

Debug symbol name: `OptClickFilterSetParameter`. Configures click-filter thresholds from screen/camera dimensions. It derives a point window, clamps it to `1..60`, clears per-slot state entries, resets count/state at `filter+0x321`, and stores squared radius threshold at `filter+4`.

```c
void OptClickFilterSetParameter(int width, int height, int radius, int scale, int divisor, void *filter)
{
    int window;
    int threshold;
    int i;

    if (scale == 0 || divisor == 0 || filter == 0)
        return;

    window = width / height;
    threshold = (radius << 15) / scale;

    if (window > 60) {
        *(int *)filter = 60;
        *(unsigned char *)((char *)filter + 0x325) = 1;
    } else if (window <= 0) {
        *(int *)filter = 1;
        *(unsigned char *)((char *)filter + 0x325) = 0;
    } else {
        *(int *)filter = window;
    }

    for (i = 0; i < *(int *)filter; i++)
        *(unsigned char *)((char *)filter + 0x14 + i * 0x0d) = 0;

    *(int *)((char *)filter + 0x321) = 0;
    *(int *)((char *)filter + 4) = threshold * threshold;
}
```
