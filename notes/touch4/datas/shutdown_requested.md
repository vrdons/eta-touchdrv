# shutdown_requested

Shutdown flag. Set to `1` by signal handler [[signal_handler]] on `SIGTERM`/`SIGINT`. Polled by main loop [[main]].

```c
int shutdown_requested;
```
