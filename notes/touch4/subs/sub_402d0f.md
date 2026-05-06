# sub_402d0f

## Control flow

[[sub_403299]] içindeki parser/device initialization adımı. Önce [[sub_41e9fa]] ile `ctx` ve `work` bağlanır. Başarısızsa `0` döner. Sonra [[sub_4027b4]] ile `work + 0xb60` alanına metadata/config okur. Başarısızsa `0` döner.

`work[0xb60]` count kadar döngü kurar. Her entry için `work + 0x800 + i * 0x6b + 8` hedefini hesaplar, stackteki config byte'ından id alır ve [[sub_41eae5]] çağırır. Her başarısızlıkta `0`, bütün entryler başarılıysa `1` döner.

Called functions: [[sub_41e9fa]], [[sub_4027b4]], [[sub_41eae5]].

```c
int sub_402d0f(void *ctx, unsigned char *work)
{
    unsigned char ids[16];
    long i;

    if (sub_41e9fa(ctx, work) == 0)
        return 0;

    if (sub_4027b4(ctx, ids, work + 0xb60) == 0)
        return 0;

    i = 0;
    while (i < work[0xb60]) {
        unsigned char *dst = work + 0x800 + i * 0x6b + 8;
        if (sub_41eae5(ctx, ids[i], dst) == 0)
            return 0;
        i++;
    }

    return 1;
}
```
