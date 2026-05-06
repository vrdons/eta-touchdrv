# cleanup_debug_log_config

If global pointer [[debug_log_config]] is non-null, frees it with `free` and clears [[debug_log_config]]. Called by [[process_exit_cleanup]] during process exit.

Called functions: `free`.
Global data: [[debug_log_config]].

```c
void cleanup_debug_log_config(void)
{
    if (debug_log_config != 0) {
        free(debug_log_config);
        debug_log_config = 0;
    }
}
```
