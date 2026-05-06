# main

Related common pattern: [[device_daemon_reconnect_loop]].

Takes `argc` and `argv`. Handles `V`, `d:`, `e:`, `h`, and `v` options through `getopt_long`. Option table is [[long_option_table]], getopt string is [[short_option_string]], switch jump table is [[option_switch_jump_table]].

For `-d`, stores [[optarg]] as `device_filter`. For `-e`, stores the same `optarg` value as `log_env_override`. `-v` increments global counter [[verbose_level]]. `-V` prints the version with [[startup_version_format]] and exits. `-h` prints usage through [[print_usage]] and exits. Invalid options print an error to [[stderr]] with [[invalid_option_usage_hint_format]] and return `1`.

After option parsing, prints banner [[otd_log_prefix]] and version [[startup_version_format]]. Registers [[process_exit_cleanup]] with `atexit`. Installs [[signal_handler]] for `SIGTERM` and `SIGINT`. Applies the `-d` value through [[set_backend_or_filter_config]]. If `-e` is absent, reads environment variable [[touch_debug_log_env_name]] through `getenv`; then applies debug/log configuration through [[set_debug_log_config]].

Allocates `0xbbb0` bytes for the main context with `malloc`. On failure, writes [[malloc_failed_error_message]] to [[stderr]] and returns `1`. On success, clears local `read_buffer[0x40]`, `path[0x100]`, and context with `memset`.

If [[shutdown_requested]] is set, runs shutdown path: prints [[shutdown_cleanup_message]], closes open fd with `close`, frees context with `free`, and returns `0`.

Each main-loop iteration yields once with `usleep(0)`. If the device is not ready yet, builds `/dev/OtdUsbRaw%03d` into `path` with [[otd_usb_raw_device_path_format]], then tries to open device indices `0..30` with `open`. If no device opens, sleeps `1s` and restarts the loop.

After opening a device, prints banner and `open device "%s".` with [[device_open_log_format]]. Calls [[initialize_packet_parser]] for packet/parser region `context+0xbc8`. On failure, closes fd, prints [[device_init_failed_message]], sleeps `1s`, and restarts. On success, prints `ready.` through [[device_ready_message]], sets `ready`, and returns to shutdown check.

When device is ready, reads `0x40` bytes with `read(fd, read_buffer, 0x40)`. Full reads are processed through [[process_raw_0x40_device_packet]] and return to the main loop. Positive short reads are ignored. Negative reads close fd with `close`, clear ready flag, print [[device_close_message]], sleep `1s`, and return to device scanning.

Called functions: [[print_usage]], [[process_exit_cleanup]], [[signal_handler]], `memset`, [[initialize_packet_parser]], [[process_raw_0x40_device_packet]], [[set_backend_or_filter_config]], [[set_debug_log_config]], `getenv`, `atexit`, `printf`, `fprintf`, `snprintf`, `fwrite`, `puts`, `malloc`, `free`, `getopt_long`, `open`, `read`, `close`, `usleep`.

Global data: [[version_only_format]], [[invalid_option_usage_hint_format]], [[short_option_string]], [[otd_log_prefix]], [[startup_version_format]], [[touch_debug_log_env_name]], [[malloc_failed_error_message]], [[shutdown_cleanup_message]], [[otd_usb_raw_device_path_format]], [[device_open_log_format]], [[device_init_failed_message]], [[device_ready_message]], [[device_close_message]], [[option_switch_jump_table]], [[version_words]], [[long_option_table]], [[stderr]], [[verbose_level]], [[shutdown_requested]], [[optarg]].

```c
int main(int argc, char **argv)
{
    unsigned char ready = 0;
    int bytes_read = -1;
    char read_buffer[0x40];
    char path[0x100];
    void *device_filter = 0;
    void *log_env = 0;
    void *ctx;
    int opt;

    while ((opt = getopt_long(argc, argv, short_option_string, long_option_table, 0)) != -1) {
        switch (opt) {
        case 'V':
            printf(version_only_format,
                       *(unsigned short *)version_words,
                       *(unsigned short *)(version_words + 2),
                       *(unsigned short *)(version_words + 4),
                       *(unsigned short *)(version_words + 6));
            return 0;
        case 'd':
            device_filter = optarg;
            break;
        case 'e':
            log_env = optarg;
            break;
        case 'h':
            print_usage(argv[0]);
            return 0;
        case 'v':
            verbose_level++;
            break;
        default:
            fprintf(stderr, invalid_option_usage_hint_format, argv[0]);
            return 1;
        }
    }

    printf(otd_log_prefix);
    printf(startup_version_format,
               *(unsigned short *)version_words,
               *(unsigned short *)(version_words + 2),
               *(unsigned short *)(version_words + 4),
               *(unsigned short *)(version_words + 6));

    atexit(process_exit_cleanup);
    signal(15, signal_handler);
    signal(2, signal_handler);

    set_backend_or_filter_config(device_filter);
    if (log_env == 0)
        log_env = getenv(touch_debug_log_env_name);
    set_debug_log_config(log_env);

    ready = 0;
    bytes_read = -1;
    ctx = malloc(0xbbb0);
    if (ctx == 0) {
        fwrite(malloc_failed_error_message, 1, 0x29, stderr);
        return 1;
    }

    memset(read_buffer, 0, 0x40);
    memset(path, 0, 0x100);
    memset(ctx, 0, 0xbbb0);

    for (;;) {
        if (shutdown_requested) {
            printf(otd_log_prefix);
            puts(shutdown_cleanup_message);
            if (ready && *(int *)ctx >= 0)
                close(*(int *)ctx);
            free(ctx);
            return 0;
        }

        usleep(0);

        if (!ready) {
            int i;
            for (i = 0; i <= 30; i++) {
                snprintf(path, 0x100, otd_usb_raw_device_path_format, i);
                *(int *)ctx = open(path, 2);
                if (*(int *)ctx >= 0)
                    break;
            }

            if (*(int *)ctx < 0) {
                usleep(1000000);
                continue;
            }

            printf(otd_log_prefix);
            printf(device_open_log_format, path);

            if (initialize_packet_parser((char *)ctx + 0xbc8, ctx) == 0) {
                close(*(int *)ctx);
                printf(otd_log_prefix);
                puts(device_init_failed_message);
                usleep(1000000);
                continue;
            }

            ready = 1;
            printf(otd_log_prefix);
            puts(device_ready_message);
            continue;
        }

        bytes_read = read(*(int *)ctx, read_buffer, 0x40);
        if (bytes_read == 0x40) {
            process_raw_0x40_device_packet((char *)ctx + 0xbc8, read_buffer, 0x40);
            continue;
        }

        if (bytes_read < 0) {
            close(*(int *)ctx);
            ready = 0;
            printf(otd_log_prefix);
            puts(device_close_message);
            usleep(1000000);
        }
    }
}
```
