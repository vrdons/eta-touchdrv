# OptInterpolatorInitialize

Debug symbol name: `OptInterpolatorInitialize`. Returns `0` when `interp == NULL`. Otherwise calls internal `pro_InterpolatorInit(interp)` and returns `-1`. [[main]] calls it for three interpolators.

```c
int OptInterpolatorInitialize(void *interp)
{
    if (interp == 0)
        return 0;
    pro_InterpolatorInit(interp);
    return -1;
}
```
