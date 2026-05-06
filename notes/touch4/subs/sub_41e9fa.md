# sub_41e9fa

## Control flow

Builds a temporary request descriptor for fixed device report id `0x80`. First calls [[sub_402713]] with `(ctx, 0x80, &tmp)`. If that fails, returns `0`. Then fills descriptor `{ ctx, 0x80, &tmp }` and passes it with callback table at `0x41e8fe` into [[sub_41d272]]. Returns `1` only when that call returns non-null.

Called by [[sub_402d0f]]. Called functions: [[sub_402713]], [[sub_41d272]].

```c
int sub_41e9fa(void *ctx, void *dst)
{
    unsigned char tmp[0x10];
    struct {
        void *ctx;
        unsigned char id;
        void *tmp;
    } desc;

    if (sub_402713(ctx, 0x80, tmp) == 0)
        return 0;

    desc.ctx = ctx;
    desc.id = 0x80;
    desc.tmp = tmp;

    if (sub_41d272(dst, (void *)0x41e8fe, &desc) == 0)
        return 0;
    return 1;
}
```
