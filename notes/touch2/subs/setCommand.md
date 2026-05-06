# setCommand

Related common pattern: [[usb_feature_command_protocol]].

Debug symbol name: `setCommand`. USB/HID feature write wrapper. Builds a type `1` packet through [[packageBuild]], then sends the command to fd with `ioctl` request `0x100040`.

Returns `0` when send fails. On success, waits `10000us`, reads command result through [[deviceGetFeature]], waits another `10000us`, and returns `1`.

Called by [[main]]. Called functions: [[packageBuild]], `ioctl`, `usleep`, [[deviceGetFeature]].

```c
int setCommand(void *ctx, unsigned char cmd, int index, int a3, void *in, unsigned char in_len)
{
    unsigned char packet[0x40];

    packageBuild(1, cmd, index, a3, in_len, packet);
    if (ioctl(*(int *)ctx, 0x100040, packet) < 0)
        return 0;

    usleep(10000);
    deviceGetFeature(ctx, cmd, 0, 0);
    usleep(10000);
    return 1;
}
```
