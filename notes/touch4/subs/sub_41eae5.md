# sub_41eae5

## Control flow

Builds a temporary request descriptor for caller-selected device report id. Calls [[sub_402713]] with `(ctx, id, &tmp)`. If that fails, returns `0`. Then fills descriptor `{ ctx, id, &tmp }` and passes it with callback table at `0x41e8fe` into [[sub_41d95d]]. Returns `1` only when that call returns non-null.

Called by [[sub_402d0f]]. Called functions: [[sub_402713]], [[sub_41d95d]].

```c
int sub_41eae5(void *ctx, unsigned char id, void *dst)
{
    unsigned char tmp[0x10];
    struct {
        void *ctx;
        unsigned char id;
        void *tmp;
    } desc;

    if (sub_402713(ctx, id, tmp) == 0)
        return 0;

    desc.ctx = ctx;
    desc.id = id;
    desc.tmp = tmp;

    if (sub_41d95d(dst, (void *)0x41e8fe, &desc) == 0)
        return 0;
    return 1;
}
```
