# data_4a549c

## Control flow

Jump table for [[sub_40188e]] option switch after `opt - 0x56`. Valid resolved cases:

- `V` -> version print block at [[sub_40188e]] `0x40192b`
- `d` -> stores [[data_4fa320]] as device/filter argument
- `e` -> stores [[data_4fa320]] as log/env argument
- `h` -> calls [[sub_4017a1]]
- `v` -> increments [[data_4f5310]]
- other entries -> invalid option path

```c
int32_t data_4a549c[] = {
    0xfff5c48f,
    0xfff5c4f6
};
```
