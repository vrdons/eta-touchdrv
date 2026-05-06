# decode

Related common pattern: [[run_length_touch_encoding]].

Debug symbol name: `decode`. Expands packed run-length bytes from raw touch packet into a plane/code array. It reads bytes from `src[5]` through `src[src[3] + 4]`; each byte stores bit value in bit 0 and repeat count in the upper bits.

```c
void decode(unsigned char *src, unsigned char *dst)
{
    int out = 0;
    int pos;

    for (pos = 5; pos < src[3] + 4; pos++) {
        unsigned char code = src[pos];
        unsigned char value = code & 1;
        unsigned char count = code >> 1;
        int start = out;

        do {
            dst[out++] = value;
        } while (count >= out - start);
    }
}
```
