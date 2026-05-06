# packageBuild

Related common pattern: [[usb_feature_command_protocol]].

Debug symbol name: `packageBuild`. Builds the 0x40-byte command packet used by [[getCommand]] and [[setCommand]]. It writes header byte `0xaa`, command type, command id, payload length (`len + 1`), index byte, optional payload bytes, and a final additive checksum seeded with `0x55`.

```c
void packageBuild(int type, unsigned char cmd, int index, unsigned char *payload, int len, unsigned char *packet)
{
    int i;
    unsigned char sum = 0x55;

    packet[0] = 0xaa;
    packet[1] = (unsigned char)type;
    packet[2] = cmd;
    packet[3] = (unsigned char)(len + 1);
    packet[4] = (unsigned char)index;

    for (i = 0; i < len; i++)
        packet[5 + i] = payload[i];

    for (i = 0; i < len + 5; i++)
        sum += packet[i];
    packet[len + 5] = sum;
}
```
