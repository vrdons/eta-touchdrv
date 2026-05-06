# sub_41ed21

## Control flow

Device filter/config değerini ayarlayan helper. [[sub_40188e]] içinde `-d` argümanından gelen pointer ile çağrılır. Parametre `NULL` değilse iç parser/registry fonksiyonu [[sub_41ecad]] ile işlenir ve global ayarlar güncellenir.

Called by [[sub_40188e]].

```c
void sub_41ed21(char *value)
{
    void *parsed = 0;
    void *tmp = 0;

    if (value != 0) {
        parsed = sub_41ecad(value);
        if (parsed != 0) {
            ;
        }
    }
}
```
