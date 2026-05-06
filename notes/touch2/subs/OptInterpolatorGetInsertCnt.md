# OptInterpolatorGetInsertCnt

Debug symbol name: `OptInterpolatorGetInsertCnt`. Returns `0` when `interp == NULL`. Otherwise multiplies [[func_GetInsertLength]] result by `*(float *)(interp+0x34)`, converts the float result to signed int, and returns it.

Called by [[DeviceProcess]]. Called functions: [[func_GetInsertLength]].

```c
int OptInterpolatorGetInsertCnt(void *interp)
{
    float length;

    if (interp == 0)
        return 0;
    length = func_GetInsertLength(interp);
    return (int)(length * *(float *)((char *)interp + 0x34));
}
```
