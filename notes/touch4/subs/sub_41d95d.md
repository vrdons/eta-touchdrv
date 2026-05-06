# sub_41d95d

## Control flow

Two-step table/parser helper. It first parses or fills 0x19 bytes into destination using table descriptor and context. If that fails, returns `0`. Then it parses/fills `0x52` bytes at destination offset `0x19` with selector `1`. Returns `1` only if both lower parser calls return non-null.

Called by [[sub_41eae5]].

```c
int sub_41d95d(void *dst, void *table, void *ctx)
{
    if (sub_41bcb0(table, 0, ctx, dst, 0x19) == 0)
        return 0;
    if (sub_41bcb0(table, 1, ctx, (char *)dst + 0x19, 0x52) == 0)
        return 0;
    return 1;
}
```
