# device_daemon_reconnect_loop

Both daemons follow same high-level service loop:

```c
for (;;) {
    if (!ready) {
        build_device_path(index);
        fd = open(path, flags);
        if (fd < 0) { sleep; continue; }
        if (!initialize_device()) { close(fd); sleep; continue; }
        ready = 1;
    }

    n = read(fd, buf, size);
    if (n < 0) { close(fd); ready = 0; sleep; continue; }
    process_packet(buf, n);
}
```

Touch2 scans `/dev/IRTouchOptical%03d` and reads `0x80` bytes. Touch4 scans `/dev/OtdUsbRaw%03d` and reads `0x40` bytes.

Used by touch2 [[main]] and touch4 [[main]].
