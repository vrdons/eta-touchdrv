# nonnull_wrapper_pattern

Several exported touch2 helpers are thin validation wrappers around internal `pro_*` routines.

They return `0` when required pointers are null. Otherwise they call the internal implementation and return `-1`, which callers treat as truthy success.

```c
int wrapper(void *required_a, void *required_b, void *required_c)
{
    if (required_a == 0 || required_b == 0 || required_c == 0)
        return 0;
    pro_impl(required_a, required_b, required_c);
    return -1;
}
```
