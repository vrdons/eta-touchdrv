- Sadece [[data_4f5328]] değerini kontrol eder.
- Eğer [[data_4f5328]] != 0 ise `1` döndürür.
- Eğer [[data_4f5328]] == 0 ise `0` döndürür.
- Yani aktif log stream açık mı kapalı mı bilgisini verir.


```c
uint64_t sub_4202cc() {
    int64_t rax;
    rax.b = data_4f5328 != 0;
    return zx.q(rax.b);
}
```