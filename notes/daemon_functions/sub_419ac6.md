- Read 32-bit value from `arg2 + 8`
- Store into `arg1[0]`

- Read 32-bit value from `arg2 + 0xc`
- Store into `arg1[1]`

- Read float from `arg2 + 0x10`
- Double value
- Store result into `arg1[2]`

- Copy `arg1[2]` into `arg1[3]`

- Return `arg1`

```c
int32_t* sub_419ac6(int32_t* arg1, void* arg2)
{
    *arg1 = *(arg2 + 8);
    arg1[1] = *(arg2 + 0xc);
    float zmm0 = *(arg2 + 0x10);
    arg1[2] = zmm0 + zmm0;
    arg1[3] = arg1[2];
    return arg1;
}
```