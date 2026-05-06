# usb_feature_command_protocol

Touch2 command path uses 0x40-byte HID-like feature packets.

Packet layout:

```c
packet[0] = 0xaa;
packet[1] = type;          // 1=set, 2=get
packet[2] = command_id;
packet[3] = payload_len + 1;
packet[4] = index;
packet[5..] = payload;
packet[payload_len + 5] = additive_checksum_seeded_with_0x55;
```

Set/get requests are sent with `ioctl(fd, 0x100040, packet)`. Responses are read with `ioctl(fd, 0x110040, stack_packet)`, validated against header/command id, and payload bytes are copied from response offset `5`.

Used by [[packageBuild]], [[setCommand]], [[getCommand]], and [[deviceGetFeature]].
