- [[sub_433180]] için ince sarmalayıcı fonksiyon. - `arg1` doğrudan aktarılır. 
- İkinci argüman sabit `0` verilir. - Üçüncü argüman olarak [[data_4f3108]] verilir. 
- Kalan vektör/register argümanları aynen [[sub_433180]] fonksiyonuna aktarılır. 
- Dönüş değeri doğrudan [[sub_433180]] dönüşüdür.
- Kontrol akışı yoktur, yalnız tailcall vardır. 


```c
int64_t sub_433bc0(int64_t arg1, uint128_t arg2, uint128_t arg3, uint128_t arg4, int128_t arg5, int128_t arg6, int128_t arg7, int128_t arg8, int128_t arg9) { 
return sub_433180(arg1, 0, data_4f3108, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9); 
}