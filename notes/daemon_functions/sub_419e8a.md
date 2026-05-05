- Input state:
  - `arg1` = context/table with callbacks and per-slot state
  - `arg2` = array of 10 records, each `0x20` bytes
  - `arg3` copied into `result` and `result_1`
  - `var_18` = change flag, init `0`

- Main loop:
  - Iterate `i` from `0` to `9`

- Per-slot branch 1:
  - If `*(arg2 + (i << 5)) == 0`
  - Then slot input invalid/empty by first field
  - If `arg1[i * 3 + 4] == 0`
    - Store local valid flag `0`
  - Else
    - Store local valid flag `1`
    - Set `var_18 = 1`
    - Store local status `0`
    - Copy coordinates from:
      - `arg1[i * 3 + 5]`
      - `arg1[i * 3 + 6]`
    - Clear `arg1[i * 3 + 4] = 0`

- Per-slot branch 2:
  - Else if `*(arg2 + (i << 5) + 8) == 0`
  - Same handling as branch 1
  - If previous slot state active, mark local valid flag `1`, set `var_18 = 1`, store status `0`, copy coordinates, clear `arg1[i * 3 + 4] = 0`
  - Else local valid flag `0`

- Per-slot branch 3:
  - Else
  - Build temporary object with [[sub_419ac6]] using `arg2 + (i << 5) + 8`
  - Call callback `(*arg1)(arg1[1], &var_168, arg1 + i * 0x18 + 0x20 + 8, &var_168)`
  - Set `arg1[i * 3 + 4] = 1`
  - Store local valid flag `1`
  - Set `var_18 = 1`
  - Store local status `1`
  - Copy coordinates from:
    - `arg1[i * 3 + 5]`
    - `arg1[i * 3 + 6]`

- Loop update:
  - `result = i + 1`
  - After loop, `result` ends as `10`

- Post-loop:
  - If `var_18 == 0`
    - Return `result`

- Debug logging:
  - If [[data_4f5310]] != 0
    - Print `"TpcToMultiTouch: frame=%u send multiTouch: "`
  - Iterate 10 local slot summaries
    - If local valid flag `0`
      - If [[data_4f5310]] != 0, print `[%u:inv] `
    - Else
      - If [[data_4f5310]] != 0, print `[%u:valid status=%d x=%u y=%u] `
  - If [[data_4f5310]] != 0
    - Print newline via `putchar(0xa, ...)`

- Final dispatch:
  - Call callback `arg1[2](arg1[3], &var_158, zx.q(result_1), &var_158)`
  - Return callback result

```c
int64_t sub_419e8a(int64_t* arg1, void* arg2, char arg3, int512_t arg4 @ zmm0, 
    int512_t arg5 @ zmm1, int512_t arg6 @ zmm2, int512_t arg7 @ zmm3, 
    int512_t arg8 @ zmm4, int512_t arg9 @ zmm5, int512_t arg10 @ zmm6, 
    int512_t arg11 @ zmm7)
{
    char result = arg3;
    char result_1 = result;
    int64_t var_18 = 0;
    int64_t __saved_rbp;
    
    for (char i = 0; i u<= 9; i = result)
    {
        if (*(arg2 + (sx.q(zx.d(i)) << 5)) == 0)
        {
            if (arg1[sx.q(zx.d(i)) * 3 + 4] == 0)
                *(&(&__saved_rbp)[sx.q(zx.d(i)) * 4] - 0x150) = 0;
            else
            {
                *(&(&__saved_rbp)[sx.q(zx.d(i)) * 4] - 0x150) = 1;
                var_18 = 1;
                *(&(&__saved_rbp)[sx.q(zx.d(i)) * 4] - 0x148) = 0;
                int64_t rdx_8 = sx.q(zx.d(i));
                int64_t rdx_9 = arg1[rdx_8 * 3 + 6];
                *(&(&__saved_rbp)[sx.q(zx.d(i)) * 4] - 0x144) =
                    arg1[rdx_8 * 3 + 5];
                *(&(&__saved_rbp)[sx.q(zx.d(i)) * 4] - 0x13c) = rdx_9;
                arg1[sx.q(zx.d(i)) * 3 + 4] = 0;
            }
        }
        else if (*(arg2 + (sx.q(zx.d(i)) << 5) + 8) == 0)
        {
            if (arg1[sx.q(zx.d(i)) * 3 + 4] == 0)
                *(&(&__saved_rbp)[sx.q(zx.d(i)) * 4] - 0x150) = 0;
            else
            {
                *(&(&__saved_rbp)[sx.q(zx.d(i)) * 4] - 0x150) = 1;
                var_18 = 1;
                *(&(&__saved_rbp)[sx.q(zx.d(i)) * 4] - 0x148) = 0;
                int64_t rdx_31 = sx.q(zx.d(i));
                int64_t rdx_32 = arg1[rdx_31 * 3 + 6];
                *(&(&__saved_rbp)[sx.q(zx.d(i)) * 4] - 0x144) =
                    arg1[rdx_31 * 3 + 5];
                *(&(&__saved_rbp)[sx.q(zx.d(i)) * 4] - 0x13c) = rdx_32;
                arg1[sx.q(zx.d(i)) * 3 + 4] = 0;
            }
        }
        else
        {
            void var_168;
            sub_419ac6(&var_168, arg2 + (sx.q(zx.d(i)) << 5) + 8);
            arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11 = (*arg1)(arg1[1], 
                &var_168, arg1 + sx.q(zx.d(i)) * 0x18 + 0x20 + 8, &var_168);
            arg1[sx.q(zx.d(i)) * 3 + 4] = 1;
            *(&(&__saved_rbp)[sx.q(zx.d(i)) * 4] - 0x150) = 1;
            var_18 = 1;
            *(&(&__saved_rbp)[sx.q(zx.d(i)) * 4] - 0x148) = 1;
            int64_t rdx_24 = sx.q(zx.d(i));
            int64_t rdx_25 = arg1[rdx_24 * 3 + 6];
            *(&(&__saved_rbp)[sx.q(zx.d(i)) * 4] - 0x144) = arg1[rdx_24 * 3 + 5];
            *(&(&__saved_rbp)[sx.q(zx.d(i)) * 4] - 0x13c) = rdx_25;
        }
        
        result = i + 1;
    }
    
    if (var_18 == 0)
        return result;
    
    if (data_4f5310 != 0)
        arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11 = sub_434970(
            "TpcToMultiTouch: frame=%u send multiTouch: ", arg4, arg5, arg6, 
            arg7, arg8, arg9, arg10, arg11, 0);
    
    for (char i_1 = 0; i_1 u<= 9; i_1 += 1)
    {
        if (*(&(&__saved_rbp)[sx.q(zx.d(i_1)) * 4] - 0x150) == 0)
        {
            if (data_4f5310 != 0)
                arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11 = sub_434970(
                    "[%u:inv] ", arg4, arg5, arg6, arg7, arg8, arg9, arg10, 
                    arg11, 0);
        }
        else if (data_4f5310 != 0)
        {
            *(&(&__saved_rbp)[sx.q(zx.d(i_1)) * 4] - 0x140);
            *(&(&__saved_rbp)[sx.q(zx.d(i_1)) * 4] - 0x144);
            *(&(&__saved_rbp)[sx.q(zx.d(i_1)) * 4] - 0x148);
            arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11 = sub_434970(
                "[%u:valid status=%d x=%u y=%u] ", arg4, arg5, arg6, arg7, arg8, 
                arg9, arg10, arg11, 0);
        }
    }
    
    if (data_4f5310 != 0)
        putchar(0xa, arg4.o, arg5.o, arg6.o, arg7.o, arg8.o, arg9.o, arg10.o, 
            arg11.o);
    
    void var_158;
    return arg1[2](arg1[3], &var_158, zx.q(result_1), &var_158);
}
```