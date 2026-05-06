# deviceGetFeature

Related common pattern: [[usb_feature_command_protocol]].

Debug symbol name: `deviceGetFeature`. Calls `ioctl(fd, 0x110040, stack_packet)`. Negative ioctl returns `-1`. Otherwise validates feature packet header: accepts when `packet[0] == 0xaa` or `packet[1] == 0x12` or `packet[2] == cmd`. If output pointer is non-null and length byte is nonzero, copies `out_len` bytes from `packet+5` to output. Internal success flag is inverted at end: success returns `0`, failure returns `-1`.

Called by [[getCommand]] and [[setCommand]]. Called functions: `ioctl`, `__stack_chk_fail`.

```c
int deviceGetFeature(void *ctx, unsigned char cmd, unsigned char *out, unsigned char out_len)
{
    unsigned char packet[0x40];
    int ok;
    int i;

    if (ioctl(*(int *)ctx, 0x110040, packet) < 0)
        return -1;

    ok = 1;
    if (packet[0] != 0xaa && packet[1] != 0x12) {
        ok = 0;
        if (cmd != packet[2])
            goto done;
    }

    if (out != 0 && out_len != 0) {
        for (i = 0; i < out_len; i++)
            out[i] = packet[i + 5];
    }
    ok = 1;

done:
    return ok ? 0 : -1;
}
```
