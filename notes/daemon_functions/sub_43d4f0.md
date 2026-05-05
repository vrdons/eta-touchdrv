- `arg1 & 1` saved in local flag `rdI`.
- Initializes fragment count `r14 = 0`, linked fragment stack `r13 = nullptr`, current format pointer `rbx = arg2`.
- Saves variadic register args into local area. Uses `var_88` and `var_80` as va-arg state.
- Reads first byte of format string.

- If format string non-empty:
  - Enters loop scanning `arg2`.
  - Inner scan searches for next `%s` pair by calling [[sub_401070]] with delimiter `0x25` (`'%'`).
  - Two cases per loop:
    - If current position starts with `%s`:
      - Fetches next variadic argument string from va-arg state.
      - If register save area exhausted (`var_88 > 0x2f`), pulls from overflow area via `var_80`.
      - Else pulls from local saved register area and advances `var_88` by 8.
      - Gets string length by calling [[sub_4010f0]].
      - Advances format pointer by 2.
      - Current fragment becomes substituted string.
    - Else:
      - Current fragment becomes literal slice from current format pointer up to next `%` or end.
      - Advances format pointer to slice end.
  - Stores each fragment as small node on stack: pointer, length, previous-node link.
  - Repeats until end of format string.

- After loop:
  - Builds contiguous `iovec` array from last fragment plus linked prior fragments.
  - Accumulates total output length while copying nodes into final `iovec` array in forward order.
  - Calls `syscall(writev, 2, iov, iovcnt)` in loop.
  - Retries if result is `-4`.

- If `rdI != 0` after write:
  - Reads alignment/granularity from [[data_4f4178]].
  - Rounds allocation size from total output length plus 4.
  - Allocates shared/mapped buffer by calling [[sub_465080]].
  - If allocation succeeds:
    - Stores buffer size header.
    - Iterates across all fragments, copying them into buffer by repeated calls to [[sub_4010d0]].
    - Writes trailing null byte.
    - Swaps global pointer [[data_4f5dc0]] with new buffer.
    - If old global buffer existed, frees/unmaps it via [[sub_4650e0]].
  - Calls `abort()` unconditionally after this path.

- If format string empty:
  - If `rdI != 0`, calls `abort()`.

- Before normal return:
  - Checks stack canary.
  - Returns success-path value if canary matches.
  - Else calls [[sub_466a60]].

```c
int64_t sub_43d4f0(int32_t arg1, char* arg2, int128_t arg3 @ zmm0,
    int128_t arg4 @ zmm1, int128_t arg5 @ zmm2, int128_t arg6 @ zmm3,
    int128_t arg7 @ zmm4, int128_t arg8 @ zmm5, int128_t arg9 @ zmm6,
    int128_t arg10 @ zmm7)
{
    int32_t rdI = arg1 & 1;
    int32_t r14 = 0;
    char** r13 = nullptr;
    char* rbx = arg2;
    void var_98;
    void* rsp = &var_98;
    int64_t rdx;
    int64_t var_58 = rdx;
    int64_t rcx;
    int64_t var_50 = rcx;
    int64_t r8;
    int64_t var_48 = r8;
    int64_t r9;
    int64_t var_40 = r9;
    char i_5 = *arg2;
    void* fsbase;
    int64_t rax = *(fsbase + 0x28);
    int32_t var_88 = 0x10;
    void arg_8;
    int64_t* var_80 = &arg_8;
    
    if (i_5 != 0)
    {
        char* rcx_1;
        void* r8_2;
        
        while (true)
        {
            char i = i_5;
            char* rax_1 = rbx;
            
            while (i != 0x25 || rax_1[1] != 0x73)
            {
                rax_1 = sub_401070(&rax_1[1], 0x25);
                i = *rax_1;
                
                if (i == 0)
                    break;
            }
            
            if (i_5 == 0x25 && rbx[1] == 0x73)
            {
                int64_t* rdx_3;
                
                if (var_88 u> 0x2f)
                {
                    rdx_3 = var_80;
                    var_80 = &rdx_3[1];
                }
                else
                {
                    void var_68;
                    rdx_3 = zx.q(var_88) + &var_68;
                    var_88 += 8;
                }
                
                char* rcx_2 = *rdx_3;
                rbx = &rbx[2];
                void* rax_6 = sub_4010f0(rcx_2);
                rcx_1 = rcx_2;
                r8_2 = rax_6;
            }
            else
            {
                rcx_1 = rbx;
                r8_2 = rax_1 - rbx;
                rbx = rax_1;
            }
            
            rsp -= 0x20;
            char** rax_3 = (rsp + 0xf) & 0xfffffffffffffff0;
            *rax_3 = rcx_1;
            rax_3[1] = r8_2;
            rax_3[2] = r13;
            i_5 = *rbx;
            
            if (i_5 == 0)
                break;
            
            r14 += 1;
            r13 = rax_3;
        }
        
        int64_t rdx_4 = sx.q(r14 + 1);
        int64_t rsI_1 = rdx_4 << 4;
        struct iovec* i_4 = (rsp - (rsI_1 + 0x10) + 0xf) & 0xfffffffffffffff0;
        char** i_1 = &i_4[rdx_4 - 1];
        *i_1 = rcx_1;
        i_1[1] = r8_2;
        
        if (r14 != 0)
        {
            do
            {
                int64_t rsI_4 = r13[1];
                char* rdI_3 = *r13;
                i_1 -= 0x10;
                r13 = r13[2];
                r8_2 += rsI_4;
                *i_1 = rdI_3;
                i_1[1] = rsI_4;
            } while (i_1 != i_4 + rsI_1 - 0x20 - (zx.q(r14 - 1) << 4));
        }
        
        int64_t i_2;
        
        do
            i_2 = syscall(sys_writev {0x14}, fd: 2, iov: i_4, iovcnt: rdx_4.d);
        while (i_2 == -4);
        
        if (rdI != 0)
        {
            int64_t rax_11 = data_4f4178;
            void* r13_2 = (rax_11 + r8_2 + 4) & neg.q(rax_11);
            int32_t* rax_13 = sub_465080(nullptr, r13_2, 3, 0x22, 0xffffffff, 0);
            
            if (rax_13 != -1)
            {
                *rax_13 = r13_2.d;
                struct iovec* i_3 = i_4;
                char* rax_14;
                
                do
                {
                    i_3 = &i_3[1];
                    rax_14 = sub_4010d0();
                } while (i_3 != &i_3[sx.q(r14) + 1]);
                
                *rax_14 = 0;
                int32_t* temp0_1 = data_4f5dc0;
                data_4f5dc0 = rax_13;
                
                if (temp0_1 != 0)
                    sub_4650e0(temp0_1, zx.q(*temp0_1));
            }
            
            abort();
        }
    }
    else if (rdI != 0)
        abort();
    
    if (rax == *(fsbase + 0x28))
        return rax - *(fsbase + 0x28);
    
    sub_466a60(arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}
```