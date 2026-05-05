
- Ellipse fitting backend seçimini yapan başlatma fonksiyonudur.
    
- Backend seçimi öncelik sırası:
    
    1. CLI argümanı (`arg1`)
    2. `ELLIPSE_FITTER` ortam değişkeni
    3. Compile-time default backend: [[data_4ef6c0]]
    4. Fallback backend: [[data_4ef6c0]]
- Yerel değişkenler:
    
    - `var_10`: seçilen backend descriptor pointer'ı.
    - `var_18`: backend seçiminin kaynağını belirten string:
        - `"CLI argument"`
        - `"environment variable"`
        - `"compile-time default"`
        - `"fallback"`
        - `"fallback (init failure)"`
- `arg1 != 0` ise:
    
    - [[sub_41ecad]] çağrılır.
    - `arg1` backend adı olarak çözümlenir.
    - Geçerli backend bulunursa:
        - `var_10` seçilen backend'e atanır.
        - `var_18 = "CLI argument"` yapılır.
    - Geçersiz backend ise:
        - [[sub_4348b0]] ile [[data_4f3708]] üzerine uyarı yazdırılır.
- CLI üzerinden geçerli backend seçilemediyse:
    
    - `getenv("ELLIPSE_FITTER")` çağrılır.
    - Ortam değişkeni varsa:
        - [[sub_41ecad]] ile backend adı çözümlenir.
        - Geçerliyse:
            - `var_10` seçilen backend'e atanır.
            - `var_18 = "environment variable"` yapılır.
        - Geçersizse:
            - [[sub_4348b0]] ile [[data_4f3708]] üzerine uyarı yazdırılır.
- Hâlâ backend seçilmediyse:
    
    - `var_10 = &[[data_4ef6c0]]`
    - `var_18 = "compile-time default"`
- Ek güvenlik fallback kontrolü:
    
    - `var_10 == 0` ise tekrar [[data_4ef6c0]] atanır.
    - `var_18 = "fallback"` yapılır.
- Backend descriptor içindeki init fonksiyonu kontrol edilir:
    
    - `var_10[1] != 0` ise init callback çağrılır.
    - Init sonucu `0` değilse hata kabul edilir.
    - Hata durumunda:
        - [[sub_4348b0]] ile [[data_4f3708]] üzerine backend init hata mesajı yazdırılır.
        - `_IO_fwrite` ile [[data_4f3708]] üzerine fallback mesajı yazdırılır.
        - Backend [[data_4ef6c0]] olarak değiştirilir.
        - `var_18 = "fallback (init failure)"` yapılır.
        - [[sub_41f08f]] çağrılır.
- Seçilen backend global alana yazılır:
    
    - [[data_4f5320]] = `var_10`
- Son olarak [[sub_434970]] ile aktif backend ve seçim kaynağı yazdırılır:
    
    - `"OpticalTouchDevice: Using '%s' ellipse fitter (%s)\n"`

C

```
uint64_t sub_41ed21(const char *cli_backend)
{
    const void **backend = NULL;
    const char *source = NULL;

    if (cli_backend != NULL) {
        backend = sub_41ecad(cli_backend);

        if (backend == NULL) {
            sub_4348b0(
                data_4f3708,
                "OpticalTouchDevice: Warning: Invalid ellipse backend '%s' specified via CLI\n",
                cli_backend
            );
        } else {
            source = "CLI argument";
        }
    }

    if (backend == NULL) {
        const char *env_backend = getenv("ELLIPSE_FITTER");

        if (env_backend != NULL) {
            backend = sub_41ecad(env_backend);

            if (backend == NULL) {
                sub_4348b0(
                    data_4f3708,
                    "OpticalTouchDevice: Warning: Invalid ellipse backend '%s' in ELLIPSE_FITTER env var\n",
                    env_backend
                );
            } else {
                source = "environment variable";
            }
        }
    }

    if (backend == NULL) {
        backend = (const void **)&data_4ef6c0;
        source = "compile-time default";
    }

    if (backend == NULL) {
        backend = (const void **)&data_4ef6c0;
        source = "fallback";
    }

    if (backend[1] != NULL) {
        int init_result = ((int (*)(void))backend[1])();

        if (init_result != 0) {
            sub_4348b0(
                data_4f3708,
                "OpticalTouchDevice: Error: Backend '%s' initialization failed (code )\n",
                backend[0]
            );

            fwrite(
                "OpticalTouchDevice: Falling back to legacy backend\n",
                1,
                0x33,
                data_4f3708
            );

            backend = (const void **)&data_4ef6c0;
            source = "fallback (init failure)";

            sub_41f08f();
        }
    }

    data_4f5320 = backend;

    return sub_434970(
        "OpticalTouchDevice: Using '%s' ellipse fitter (%s)\n",
        backend[0],
        source
    );
}
```