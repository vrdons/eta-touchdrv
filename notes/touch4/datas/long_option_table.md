# long_option_table

Long option table passed to `getopt_long` from [[main]]. Exact entries live in `.data.rel.ro`; [[main]] combines it with short option string [[short_option_string]].

```c
struct option long_option_table[] = {
    /* long options for e, d, v, V, h */
};
```
