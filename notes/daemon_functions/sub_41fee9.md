- Stack üzerinde bir `timeval` yapısı oluşturur.
- Bu yapının adresini ve `nullptr` değerini [[sub_4628a0]] fonksiyonuna yollar.
- Döndükten sonra:
    - `tv_sec * 1000`
    - `tv_usec / 1000`  
        değerlerini toplar.
- Sonuç olarak zamanı milisaniye cinsinden döndürür.


```
int64_t sub_41fee9() {
    struct timeval var_18;
    sub_4628a0(&var_18, nullptr);
    return (var_18.tv_usec / 0x3e8) + (var_18.tv_sec * 0x3e8);
}
```