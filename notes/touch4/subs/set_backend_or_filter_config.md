# set_backend_or_filter_config

Helper that sets device filter/config value. Called inside [[main]] with pointer from `-d` argument. If parameter is not `NULL`, it is processed by internal parser/registry function [[lookup_backend_config_by_name]] and global backend/filter settings are updated.

Called by [[main]].

```c
void set_backend_or_filter_config(char *value)
{
    void *parsed;

    if (value == 0)
        return;

    parsed = lookup_backend_config_by_name(value);
    if (parsed == 0)
        return;

    apply_backend_config(parsed);
}
```
