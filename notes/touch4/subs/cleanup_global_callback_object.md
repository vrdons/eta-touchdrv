# cleanup_global_callback_object

Performs global callback cleanup. If [[global_callback_object]] is non-null and `[[global_callback_object]]->field_18` callback pointer is non-null, calls the callback. Then clears global pointer [[global_callback_object]]. Called by [[process_exit_cleanup]] during process exit.

Global data: [[global_callback_object]].

```c
void cleanup_global_callback_object(void)
{
    if (global_callback_object != 0) {
        if (*(void (**)(void))((char *)global_callback_object + 0x18) != 0) {
            (*(void (**)(void))((char *)global_callback_object + 0x18))();
        }
    }

    global_callback_object = 0;
}
```
