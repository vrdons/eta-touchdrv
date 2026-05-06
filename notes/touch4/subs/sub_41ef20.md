# sub_41ef20

## Control flow

Global callback cleanup yapar. [[data_4f5320]] null değilse ve `[[data_4f5320]]->field_18` callback pointerı null değilse callback'i çağırır. Sonra [[data_4f5320]] global pointerını sıfırlar. [[sub_401770]] tarafından process exit sırasında çağrılır.

Global data: [[data_4f5320]].

```c
void sub_41ef20(void)
{
    if (data_4f5320 != 0) {
        if (*(void (**)(void))((char *)data_4f5320 + 0x18) != 0) {
            (*(void (**)(void))((char *)data_4f5320 + 0x18))();
        }
    }

    data_4f5320 = 0;
}
```
