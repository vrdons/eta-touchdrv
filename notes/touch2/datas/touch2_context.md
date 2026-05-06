# touch2_context

Global `context` object from debug symbols. [[main]] stores fd at start of this object, initializes optical/click/interpolator subcontexts, and passes it to [[DeviceProcess]]. [[DeviceProcess]] uses offsets including `+0x48`, `+0x4c`, `+0x50`, `+0x58`, `+0x65`, `+0xf3f`, `+0x1265`, `+0x129d`, `+0x12d5`.

```c
struct touch2_context touch2_context;
```
