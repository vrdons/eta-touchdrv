# dispatch_touch_callback

Callback dispatch wrapper. If `ctx` is null or callback pointer at `ctx+0x8` is null, returns. If payload pointer is null, returns. Otherwise calls callback with callback-owned state pointers and packet data.

Called by [[dispatch_report_to_staging_sink]].

```c
void dispatch_touch_callback(void *ctx, void *payload, unsigned char seq, void *len_state)
{
    void (*callback)(void *, void *, unsigned char, void *, void *);

    if (ctx == 0)
        return;
    callback = *(void (**)(void *, void *, unsigned char, void *, void *))((char *)ctx + 0x8);
    if (callback == 0)
        return;
    if (payload == 0)
        return;

    callback((char *)ctx + 0x20, payload, seq, len_state, (char *)ctx + 0x18);
}
```
