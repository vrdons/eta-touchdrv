# sub_41eae5

- Çeşitli veri tiplerinde argümanlar ve SIMD (zmm0-zmm7) register verileri alır.
- Aldığı verileri ve yerel bir değişkenin (var_30) adresini [[sub_402713]] fonksiyonuna argüman olarak göndererek kontrol eder.
- Eğer [[sub_402713]] 0 döndürürse fonksiyon başarısız olur ve 0 döner.
- Başarılı olursa, yerel stack üzerinde bir yapı oluşturur (var_28, var_20_1, var_18_1).
- Ardından arg3, [[sub_41e8fe]] fonksiyon pointer'ı ve stack yapısının adresini (&var_28) [[sub_41d95d]] fonksiyonuna gönderir.
- [[sub_41d95d]]'den dönen sonuca göre 1 veya 0 (başarı durumu) döndürür.

```
int64_t sub_41eae5(int32_t* arg1, char arg2, char* arg3, int128_t arg4, 
  int128_t arg5, int128_t arg6, int128_t arg7, int128_t arg8, 
  int128_t arg9, int128_t arg10, int128_t arg11) {
    
    void* var_30;
    
    if (sub_402713(arg1, arg2, &var_30, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) == 0) {
        return 0;
    }
    
    int32_t* var_28 = arg1;
    char var_20_1 = arg2;
    void* var_18_1 = &var_30;
    
    if (sub_41d95d(arg3, sub_41e8fe, &var_28) != 0) {
        return 1;
    }
    
    return 0;
}
```