# usb_control_read_8byte_descriptor

Related common pattern: [[usb_control_descriptor_read]].

Initializes 0x3a-byte stack buffer with byte `0x3a`, sends control transfer command `0xb0` through lower USB helper, requires success and returned length exactly `8`, then copies 8 bytes from local buffer to caller output. Returns `1` on success, otherwise `0`.

Called by [[read_base_report_descriptor_0x80]] and [[read_selected_report_descriptor]].

```c
int usb_control_read_8byte_descriptor(void *dev, unsigned char id, void *out)
{
    unsigned char buf[0x3a];
    uint64_t got = 0;

    fill_buffer_with_length_byte(buf, 0x3a);
    if (usb_control_transfer(dev, 0x1e, 0xb0, id, buf, 0, 0, 0x3a, &got, 1) == 0)
        return 0;
    if (got != 8)
        return 0;
    copy_bytes(buf, out, 8);
    return 1;
}
```
