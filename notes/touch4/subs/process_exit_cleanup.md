# process_exit_cleanup

Process exit cleanup callback. Calls [[cleanup_global_callback_object]] first, then [[cleanup_debug_log_config]]. [[main]] registers this as exit callback through `atexit`.

```c
void process_exit_cleanup(void)
{
    cleanup_global_callback_object();
    cleanup_debug_log_config();
}
```
