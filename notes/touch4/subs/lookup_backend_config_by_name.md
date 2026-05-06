# lookup_backend_config_by_name

Maps device filter/backend string to static config object. Null input returns `0`. If string equals literal at `0x4a5a90`, returns config object `0x4ef6c0`. If string equals literal at `0x4a5a97`, returns config object `0x4ef6e0`. Other strings return `0`.

Called by [[set_backend_or_filter_config]]. Called functions: `strcmp`.

```c
void *lookup_backend_config_by_name(char *name)
{
    if (name == 0)
        return 0;
    if (strcmp(name, (char *)0x4a5a90) == 0)
        return (void *)0x4ef6c0;
    if (strcmp(name, (char *)0x4a5a97) == 0)
        return (void *)0x4ef6e0;
    return 0;
}
```
