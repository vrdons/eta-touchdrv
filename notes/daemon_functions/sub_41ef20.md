# sub_41ef20

- Global bir veri alanı olan [[data_4f5320]]'yi kontrol eder.
- Eğer bu değer 0 değilse, bu bellek adresinin 0x18 offsetindeki değeri (muhtemelen bir fonksiyon pointer'ı) kontrol eder.
- Eğer offsetteki değer de 0 değilse, bu adresteki fonksiyonu çalıştırır (bir callback veya temizleme/kapanış fonksiyonu tetikleyicisi olabilir).
- İşlem bittikten sonra [[data_4f5320]] değerini 0'a eşitler (bir daha çalışmasını engellemek için) ve sonucu döndürür.

```c
int64_t sub_41ef20() {
    int64_t result = data_4f5320;
    
    if (result != 0) {
        result = *(data_4f5320 + 0x18);
        
        if (result != 0) {
            result = (*(data_4f5320 + 0x18))();
        }
    }
    
    data_4f5320 = 0;
    return result;
}
```