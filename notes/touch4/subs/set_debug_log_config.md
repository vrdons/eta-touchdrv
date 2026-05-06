# set_debug_log_config

Applies debug/log environment value into global state. Called inside [[main]] with `-e` value or value from [[touch_debug_log_env_name]] (`TOUCH_DEBUG_LOG`) environment variable. Reads old global pointer [[debug_log_config]]; if parameter is `NULL`, clears global, otherwise stores the new value. Previous config is cleaned when ownership changes.

Called by [[main]].

```c
void set_debug_log_config(char *value)
{
    char *old = debug_log_config;

    if (value == 0)
        debug_log_config = 0;
    else
        debug_log_config = value;

    if (old != 0 && old != debug_log_config)
        free(old);
}
```
