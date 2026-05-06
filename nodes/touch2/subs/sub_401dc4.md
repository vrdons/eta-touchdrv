# sub_401dc4

## Control flow

Debug symbol name: `OptInterpolatorInitialize`. `interp == NULL` ise `0` döner. Null değilse internal `pro_InterpolatorInit(interp)` çağırır ve `-1` döner. [[sub_405d74]] üç interpolator için çağırır.

```c
int sub_401dc4(void *interp)
{
    if (interp == 0)
        return 0;
    pro_InterpolatorInit(interp);
    return -1;
}
```
