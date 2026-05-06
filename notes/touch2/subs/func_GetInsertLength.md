# func_GetInsertLength

Debug symbol name: `func_GetInsertLength`. Computes path length for interpolator context. Runs two passes. Each pass requires validity bytes at `+0x26` and `+0x19`; otherwise contributes `0`. It chooses first segment start from either current point `+0/+4` or previous point `+0xd/+0x11`, measures distance to middle point `+0x1a/+0x1e`, adds constant double at `0x406628`, then chooses second segment end from either point `+0x27/+0x2b` or middle point and adds distance to previous point. It also adds a projected segment using five times middle-minus-previous vector. Returns float length.

Called by [[OptInterpolatorGetInsertCnt]]. Called functions: `sqrt`.

```c
float func_GetInsertLength(void *interp)
{
    double total = 0.0;
    int pass = 2;

    do {
        double x0;
        double y0;
        double x1;
        double y1;
        double dx;
        double dy;

        if (*(unsigned char *)((char *)interp + 0x26) != 0 &&
            *(unsigned char *)((char *)interp + 0x19) != 0) {
            if (*(unsigned char *)((char *)interp + 0x0c) != 0) {
                x0 = *(int *)((char *)interp + 0x00);
                y0 = *(int *)((char *)interp + 0x04);
            } else {
                x0 = *(int *)((char *)interp + 0x0d);
                y0 = *(int *)((char *)interp + 0x11);
            }

            dx = x0 - *(int *)((char *)interp + 0x1a);
            dy = y0 - *(int *)((char *)interp + 0x1e);
            total = sqrt(dx * dx + dy * dy) + *(double *)0x406628;

            if (*(unsigned char *)((char *)interp + 0x33) != 0) {
                x1 = *(int *)((char *)interp + 0x27);
                y1 = *(int *)((char *)interp + 0x2b);
            } else {
                x1 = *(int *)((char *)interp + 0x1a);
                y1 = *(int *)((char *)interp + 0x1e);
            }

            dx = x1 - *(int *)((char *)interp + 0x0d);
            dy = y1 - *(int *)((char *)interp + 0x11);
            total += sqrt(dx * dx + dy * dy);

            dx = x0 + (*(int *)((char *)interp + 0x1a) - *(int *)((char *)interp + 0x0d)) * 5 - x1;
            dy = y0 + (*(int *)((char *)interp + 0x1e) - *(int *)((char *)interp + 0x11)) * 5 - y1;
            total += sqrt(dx * dx + dy * dy);
        }
        pass--;
    } while (pass != 0);

    return (float)total;
}
```
