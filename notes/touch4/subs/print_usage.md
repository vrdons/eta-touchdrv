# print_usage

Produces usage/help output. Takes program name as parameter. Writes multiple constant strings with `puts` and `printf`. Called from [[main]] for `-h` option.

Referenced data starts near [[help_title_string]] and continues usage text block through [[custom_backend_info_string]].

```c
void print_usage(char *argv0)
{
    puts(help_title_string);
    printf(usage_format_string, argv0);
    puts(options_heading_string);
    puts(ellipse_backend_help_row);
    puts(ellipse_backend_env_help_row);
    puts(debug_touch_pipeline_help_row);
    puts(debug_log_env_help_row);
    puts(verbose_help_row);
    puts(verbose_continuation_help_row);
    puts(version_help_row);
    puts(help_help_row);
    puts(backend_info_heading_string);
    puts(legacy_backend_info_string);
    puts(custom_backend_info_string);
}
```
