# main

Related common pattern: [[device_daemon_reconnect_loop]].

Debug symbol name: `main`. Uses global device context [[touch2_context]]. It prints [[irtouch_log_prefix]] and [[irtouch_version_format]], clears path/config stack buffers, and clears [[touch2_context]].

Main loop has two modes based on `ready`. When `ready == 0`, it builds `/dev/IRTouchOptical%03d` with [[irtouch_device_path_format]] and opens the device with `open`. If open fails, it sleeps `1s` and retries. After open, it prints `open device "%s".` with [[device_open_log_format]].

After device open, it sends command `0x71` twice through [[setCommand]], then reads camera/screen/config blocks through [[getCommand]]. Those blocks feed [[OptInitialize]], [[OptClickFilterInitialize]], and three [[OptInterpolatorInitialize]] calls. Any initialization failure closes fd with `close`, prints [[device_init_failed_message]], sleeps `1s`, and retries from device scan.

On successful init, click-filter parameters are configured through [[OptClickFilterSetParameter]]. If calibration is available, [[OptSetCalibPara]] is called; otherwise [[OptSetCalibParaDefault]] is called. Context flags are set, [[device_ready_message]] (`ready.`) is printed, and `ready = 1`.

When `ready == 1`, it reads `0x80` bytes from fd with `read`. Positive reads pass buffer to [[DeviceProcess]]. Negative reads close fd with `close`, print [[device_close_message]] (`close device.`), set `ready = 0`, and return to device scanning.

Called functions: `snprintf`, `puts`, `close`, `read`, `printf`, `open`, `usleep`, [[getCommand]], [[setCommand]], [[OptClickFilterSetParameter]], [[OptClickFilterInitialize]], [[OptInterpolatorInitialize]], [[OptSetCalibPara]], [[OptSetCalibParaDefault]], [[OptInitialize]], [[DeviceProcess]].

Global data: [[irtouch_log_prefix]], [[irtouch_version_format]], [[irtouch_device_path_format]], [[device_open_log_format]], [[device_close_message]], [[device_ready_message]], [[device_init_failed_message]], [[touch2_context]].

```c
int main(int argc, char **argv)
{
    unsigned char ready = 0;
    char path[0x80];
    unsigned char buf[0x80];

    printf(irtouch_log_prefix);
    printf(irtouch_version_format, 1, 0, 0x1b54, 0x4df9);
    memset(path, 0, sizeof(path));
    memset(&touch2_context, 0, sizeof(touch2_context));

    for (;;) {
        usleep(0);
        if (!ready) {
            snprintf(path, sizeof(path), irtouch_device_path_format, 0);
            touch2_context.fd = open(path, 2);
            if (touch2_context.fd < 0) {
                usleep(1000000);
                continue;
            }
            printf(irtouch_log_prefix);
            printf(device_open_log_format, path);
            if (!initialize_touch2_device(&touch2_context)) {
                close(touch2_context.fd);
                puts(device_init_failed_message);
                usleep(1000000);
                continue;
            }
            puts(device_ready_message);
            ready = 1;
        }

        if (read(touch2_context.fd, buf, sizeof(buf)) < 0) {
            close(touch2_context.fd);
            puts(device_close_message);
            ready = 0;
            continue;
        }
        DeviceProcess(&touch2_context, buf, sizeof(buf));
    }
}
```
