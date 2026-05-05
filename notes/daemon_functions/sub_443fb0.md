- Global init yapar. malloc/heap allocator başlangıç rutini gibi durur.
- Başta `fsbase + 0x28` stack canary alır.
- [[data_4fa2e0]]` = 1` yazar. Init-done/in-init flag.
- [[sub_4762a0]]`(&`[[data_4fa2d0]]`, 8, 1)` çağırır.
  - Amaç: 8 byte rastgele/entropy almak.
  - Sonuç `8` değilse fallback yol girer.
- Fallback yol:
  - [[sub_462920]]`(1, &var_38)` iki kez çağrılır.
  - Her çağrıda `var_38.d ^ var_30` hesaplanır.
  - Sonuç ve 8-bit rotate edilmiş hali XOR yapılır.
  - İlk sonuç üst/alt 32-bit için ara değer olur.
  - İkinci sonuç ile birleşip [[data_4fa2d0]] içine 64-bit seed yazar.
- `*(fsbase - 0x30) = &`[[data_4f3800]]
  - Thread-local arena/heap state pointer kurar gibi.
- `for (i = &`[[data_4f3860]]`; i != &`[[data_4f4050]]`; )`
  - Liste/bin başlıklarını dolaşır.
  - Her elemanı kendine işaret edecek şekilde çift bağlı liste head yapar.
  - `i = &i[2]` ile 16-byte stride ilerler.
  - `*i = unpacklo(i, i)` etkisi: `fd = bk = self`.
- Sonra başlangıç değerleri yazar:
  - [[data_4fa2f8]]` = 0x80`
  - [[data_4f3808]]` = 0`
  - [[data_4f3860]]` = &`[[data_4f3860]]
- Çok sayıda `__tunable_get_val` çağrısı yapar.
  - Çeşitli allocator tunable değerlerini yükler.
  - Callback fonksiyonlar ilgili global ayarları set eder.
- Yüklenen tunable alanları:
  - top pad
  - perturb byte
  - trim threshold
  - mmaps max
  - arena max
  - arena test
  - tcache max
  - tcache count
  - tcache unsorted limit
  - mxfast
  - ek ayar için [[data_443710]]
- [[data_4f3790]]` != 0` ise [[data_4fa300]]` = 1`
  - Muhtemel feature/secure mode/extra allocator flag.
- Sonunda stack canary kontrol eder.
- Canary bozuksa `sub_466a60(...)` çağırır. Bu `__stack_chk_fail` tipi noreturn yol.

```c
int64_t sub_443fb0()
{
    uint64_t canary = *(uint64_t *)(fsbase + 0x28);
    data_4fa2e0 = 1;

    if (sub_4762a0(&data_4fa2d0, 8, 1) != 8)
    {
        int32_t a, b;
        sub_462920(1, &a);
        int32_t x = a ^ b;
        uint64_t high = (uint32_t)x ^ (uint32_t)ror32(x, 8);

        data_4fa2d0 = high;

        sub_462920(1, &a);
        x = a ^ b;
        uint64_t low = (uint32_t)x ^ (uint32_t)ror32(x, 8);

        data_4fa2d0 = (high << 32) | low;
    }

    *(void **)(fsbase - 0x30) = &data_4f3800;

    for (int64_t *i = &data_4f3860; i != &data_4f4050; i += 2)
    {
        i[0] = (int64_t)i;
        i[1] = (int64_t)i;
    }

    data_4fa2f8 = 0x80;
    data_4f3808 = 0;
    data_4f3860 = (int64_t)&data_4f3860;

    __tunable_get_val(0xe, &var_38, _dl_tunable_set_top_pad);
    __tunable_get_val(3, &var_38, _dl_tunable_set_perturb_byte);
    __tunable_get_val(0x1c, &var_38, &data_4433c0);
    __tunable_get_val(2, &var_38, _dl_tunable_set_trim_threshold);
    __tunable_get_val(0x15, &var_38, _dl_tunable_set_mmaps_max);
    __tunable_get_val(0x1b, &var_38, _dl_tunable_set_arena_max);
    __tunable_get_val(0x1f, &var_38, _dl_tunable_set_arena_test);
    __tunable_get_val(0x23, &var_38, _dl_tunable_set_tcache_max);
    __tunable_get_val(0x1e, &var_38, _dl_tunable_set_tcache_count);
    __tunable_get_val(0x17, &var_38, _dl_tunable_set_tcache_unsorted_limit);
    __tunable_get_val(0xb, &var_38, _dl_tunable_set_mxfast);
    __tunable_get_val(9, &var_38, &data_443710);

    if (data_4f3790 != 0)
        data_4fa300 = 1;

    if (canary == *(uint64_t *)(fsbase + 0x28))
        return 0;

    sub_466a60();
}
```
