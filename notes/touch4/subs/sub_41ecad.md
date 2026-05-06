# sub_41ecad

## Control flow

Maps device filter/backend string to static config object. Null input returns `0`. If string equals literal at `0x4a5a90`, returns config object `0x4ef6c0`. If string equals literal at `0x4a5a97`, returns config object `0x4ef6e0`. Other strings return `0`.

Called by [[sub_41ed21]]. Called functions: [[sub_4010c0]].

```c
void *sub_41ecad(char *name)
{
    if (name == 0)
        return 0;
    if (sub_4010c0(name, (char *)0x4a5a90) == 0)
        return (void *)0x4ef6c0;
    if (sub_4010c0(name, (char *)0x4a5a97) == 0)
        return (void *)0x4ef6e0;
    return 0;
}
```
