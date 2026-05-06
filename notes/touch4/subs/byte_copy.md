# byte_copy

Byte copy helper. Copies exactly `len` bytes from `src` to `dst`, one byte per loop, and returns no value. Used by [[process_raw_0x40_device_packet]].

```c
void byte_copy(unsigned char *src, unsigned char *dst, uint64_t len)
{
    unsigned char *s = src;
    unsigned char *d = dst;

    while (len != 0) {
        *d = *s;
        d++;
        s++;
        len--;
    }
}
```
