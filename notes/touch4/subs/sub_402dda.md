# sub_402dda

## Control flow

[[sub_403299]] içindeki parser setup adımı. `work + 0x68` içindeki function pointer null ise `0` döner. Null değilse callback `work` ve stack buffer ile çağrılır. Callback sonrası stack buffer offset `0x21` değerini alır ve [[sub_404373]] ile `parser`, sabit `2`, bu değer üzerinden kontrol yapar. [[sub_404373]] true dönerse `0`, false dönerse `1` döner.

Called functions: indirect callback at `work + 0x68`, [[sub_404373]].

```c
int sub_402dda(void *parser, void *work)
{
    unsigned char tmp[0xa0];
    void (*callback)(void *, unsigned char *);
    unsigned char value;

    callback = *(void (**)(void *, unsigned char *))((char *)work + 0x68);
    if (callback == 0)
        return 0;

    callback(work, tmp);
    value = tmp[0x21];

    if (sub_404373(parser, 2, value))
        return 0;

    return 1;
}
```
