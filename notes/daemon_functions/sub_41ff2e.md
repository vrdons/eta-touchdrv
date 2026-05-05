- `arg1` kaynak string adresi, `arg2` çıktı buffer'ı, `arg3` ise çıktı buffer boyutu olarak kullanılır.
- Başta iki yerel değişkeni `0` yapar:
    - biri kaynak string içinde ilerlemek için,
    - biri çıktı buffer'ına kaç byte yazıldığını takip etmek için.
- Eğer `arg1 != 0` ise kaynak string üzerinde `0` byte'ı görünene kadar döner.
- Her iterasyonda önce çıktı buffer'ında yeterli yer kalıp kalmadığını kontrol eder.
    - Sınır olarak `arg3 - 7` kullanır.
    - Yer yetmezse döngüyü kırar.
- Okunan karaktere göre özel durumları kontrol eder:
    - `0x08` için `\\b`
    - `0x09` için `\\t`
    - `0x0a` için `\\n`
    - `0x0c` için `\\f`
    - `0x0d` için `\\r`
    - `0x22` için `\\\"`
    - `0x5c` için `\\\\`
- Yukarıdaki özel karakterlerden biri değilse:
    - karakter `0x1f`'ten büyükse olduğu gibi çıktı buffer'ına kopyalar,
    - değilse [[sub_434a40]] ile `"\u%04x"` formatında yazar.
- Her karakter işlendiğinde kaynak pointer/index bir ileri alınır.
- Döngü bittikten sonra çıktı stringini `0` ile sonlandırır.
- Eğer `arg1 == 0` ve `arg3 != 0` ise boş string üretmek için `*arg2 = 0` yazar.
- [[sub_4202e2]] içinde string verisini yazmadan önce kaçışlı/uyumlu hale getirmek için kullanılır.

```
void sub_41ff2e(int64_t arg1, char* arg2, int64_t arg3) {
    int64_t var_10 = 0;
    int64_t var_18 = 0;
    
    if (arg1 != 0) {
        while (*(char*)(arg1 + var_10) != 0) {
            if (var_18 >= arg3 - 7)
                break;
            
            char rax_3 = *(char*)(arg1 + var_10);
            uint32_t rax_4 = (uint8_t)rax_3;
            
            if (rax_4 <= 0x22) {
                if (rax_4 >= 8 && (rax_4 - 8) <= 0x1a) {
                    switch (rax_4) {
                        case 8:
                            *(arg2 + var_18) = 0x5c;
                            var_18 += 1;
                            *(arg2 + var_18) = 0x62;
                            var_18 += 1;
                            break;
                        
                        case 9:
                            *(arg2 + var_18) = 0x5c;
                            var_18 += 1;
                            *(arg2 + var_18) = 0x74;
                            var_18 += 1;
                            break;
                        
                        case 0xa:
                            *(arg2 + var_18) = 0x5c;
                            var_18 += 1;
                            *(arg2 + var_18) = 0x6e;
                            var_18 += 1;
                            break;
                        
                        case 0xc:
                            *(arg2 + var_18) = 0x5c;
                            var_18 += 1;
                            *(arg2 + var_18) = 0x66;
                            var_18 += 1;
                            break;
                        
                        case 0xd:
                            *(arg2 + var_18) = 0x5c;
                            var_18 += 1;
                            *(arg2 + var_18) = 0x72;
                            var_18 += 1;
                            break;
                        
                        case 0x22:
                            *(arg2 + var_18) = 0x5c;
                            var_18 += 1;
                            *(arg2 + var_18) = 0x22;
                            var_18 += 1;
                            break;
                        
                        default:
                            if ((uint8_t)rax_3 > 0x1f) {
                                *(arg2 + var_18) = rax_3;
                                var_18 += 1;
                            } else {
                                sub_434a40(arg2 + var_18, arg3 - var_18, "\\u%04x", 0);
                                var_18 += 6;
                            }
                            break;
                    }
                } else {
                    if ((uint8_t)rax_3 > 0x1f) {
                        *(arg2 + var_18) = rax_3;
                        var_18 += 1;
                    } else {
                        sub_434a40(arg2 + var_18, arg3 - var_18, "\\u%04x", 0);
                        var_18 += 6;
                    }
                }
            } else if (rax_4 == 0x5c) {
                *(arg2 + var_18) = 0x5c;
                var_18 += 1;
                *(arg2 + var_18) = 0x5c;
                var_18 += 1;
            } else {
                if ((uint8_t)rax_3 > 0x1f) {
                    *(arg2 + var_18) = rax_3;
                    var_18 += 1;
                } else {
                    sub_434a40(arg2 + var_18, arg3 - var_18, "\\u%04x", 0);
                    var_18 += 6;
                }
            }
            
            var_10 += 1;
        }
        
        *(arg2 + var_18) = 0;
    } else if (arg3 != 0) {
        *arg2 = 0;
    }
}
```