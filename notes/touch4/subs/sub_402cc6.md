# sub_402cc6

## Control flow

Byte copy helper. Copies exactly `len` bytes from `src` to `dst`, one byte per loop, and returns no value. Used by [[sub_403370]].

```c
void sub_402cc6(unsigned char *src, unsigned char *dst, uint64_t len)
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
