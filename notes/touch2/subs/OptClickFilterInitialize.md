# OptClickFilterInitialize

Debug symbol name: `OptClickFilterInitialize`. Returns `0` when `filter == NULL`. Otherwise calls internal `pro_ClickFilterInit(filter)` and returns `-1`. [[main]] treats this return as truthy success during init.

```c
int OptClickFilterInitialize(void *filter)
{
    if (filter == 0)
        return 0;
    pro_ClickFilterInit(filter);
    return -1;
}
```
