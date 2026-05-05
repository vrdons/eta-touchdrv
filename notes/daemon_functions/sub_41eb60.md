# sub_41eb60


- İşleyiş olarak [[sub_41eae5]] ile neredeyse aynıdır.
- Aynı şekilde SIMD verilerini alır ve [[sub_402713]] ile başlangıç kontrolü yapar.
- Temel fark; yerel stack yapısını hazırladıktan sonra,[[sub_41d9d8]] fonksiyonunu çağırırken iki farklı fonksiyon pointer'ı ([[sub_41e958]] ve [[sub_41e9a0]]) parametre olarak vermesidir.
- [[sub_41d9d8]] sonucuna göre işlemi başarılı (1) veya başarısız (0) olarak döndürür.

```
int64_t sub_41eb60(int32_t* arg1, char arg2, char* arg3, int128_t arg4, 
  int128_t arg5, int128_t arg6, int128_t arg7, int128_t arg8, 
  int128_t arg9, int128_t arg10, int128_t arg11) {
    
    void* var_30;
    
    if (sub_402713(arg1, arg2, &var_30, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) == 0) {
        return 0;
    }
    
    int32_t* var_28 = arg1;
    char var_20_1 = arg2;
    void* var_18_1 = &var_30;
    
    if (sub_41d9d8(arg3, sub_41e958, sub_41e9a0, &var_28) != 0) {
        return 1;
    }
    
    return 0;
}
```