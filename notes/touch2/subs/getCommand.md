# getCommand

Related common pattern: [[usb_feature_command_protocol]].

Debug symbol name: `getCommand`. USB/HID feature read wrapper. Builds a 0x40-byte get-command packet through [[packageBuild]] with type `2`, sends it to fd with `ioctl` request `0x100040`, waits `10000us`, reads the response through [[deviceGetFeature]], waits again, and returns `1` on success. Any failed transfer returns `0`.

```c
int getCommand(void *dev, unsigned char cmd, int index, unsigned char *payload, int payload_len, unsigned char *out, int out_len)
{
    unsigned char packet[0x40];

    packageBuild(2, cmd, index, payload, payload_len, packet);
    if (ioctl(*(int *)dev, 0x100040, packet) < 0)
        return 0;
    usleep(10000);
    if (deviceGetFeature(dev, cmd, out, out_len) < 0)
        return 0;
    usleep(10000);
    return 1;
}
```
