# option_switch_jump_table

Jump table for [[main]] option switch after `opt - 0x56`. Valid resolved cases:

- `V` -> version print block at [[main]] `0x40192b`
- `d` -> stores [[optarg]] as device/filter argument
- `e` -> stores [[optarg]] as log/env argument
- `h` -> calls [[print_usage]]
- `v` -> increments [[verbose_level]]
- other entries -> invalid option path

```c
int32_t option_switch_jump_table[] = {
    0xfff5c48f,
    0xfff5c4f6
};
```
