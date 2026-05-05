- Input:
  - `arg1` source pointer
  - `arg2` destination pointer
  - `arg3` byte count
- Local setup:
  - `i` gets `arg3`
  - `var_10` gets `arg1`
  - `result` and `result_1` get `arg2`
- Loop runs while `i != 0`
- Each iteration:
  - `result` gets current destination pointer `result_1`
  - `*result = *var_10`
  - `result_1` increments by 1
  - `var_10` increments by 1
  - `i` decrements by 1
- Return:
  - returns `result`
  - if `arg3 == 0`, returned pointer stays `arg2`
  - if `arg3 > 0`, returned pointer is address of last written byte in destination

```c
char* sub_402cc6(char* arg1, char* arg2, int64_t arg3)

{
    int64_t i = arg3;
    char* var_10 = arg1;
    char* result = arg2;
    char* result_1 = result;
    
    for (; i != 0; i -= 1)
    {
        result = result_1;
        *result = *var_10;
        result_1 = &result_1[1];
        var_10 = &var_10[1];
    }
    
    return result;
}
```