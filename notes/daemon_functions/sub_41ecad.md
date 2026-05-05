- Check `arg1`.
- If `arg1 == 0`, return `nullptr`.
- Call [[sub_4010c0]] with `arg1`, `"legacy"`, `"legacy"`.
- If [[sub_4010c0]] returns `0`, return [[data_4ef6c0]].
- Else call [[sub_4010c0]] with `arg1`, `"custom"`, `"custom"`.
- If result not `0`, return `nullptr`.
- Else return [[data_4ef6e0]].

```c
char const (** const )[0x7] sub_41ecad(int64_t arg1)
{
    if (arg1 == 0)
        return nullptr;
    
    if (sub_4010c0(arg1, "legacy", "legacy") == 0)
        return &data_4ef6c0;
    
    if (sub_4010c0(arg1, "custom", "custom") != 0)
        return nullptr;
    
    return &data_4ef6e0;
}
```