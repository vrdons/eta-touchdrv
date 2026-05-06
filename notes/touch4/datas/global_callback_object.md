# global_callback_object

Global object pointer used by [[cleanup_global_callback_object]]. If non-null, [[cleanup_global_callback_object]] checks function pointer at offset `0x18`, calls it, then clears this global.

```c
void *global_callback_object;
```
