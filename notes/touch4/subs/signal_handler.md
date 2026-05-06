# signal_handler

Signal handler. Takes signal number as parameter. If signal is `15` (`SIGTERM`) or `2` (`SIGINT`), sets global shutdown flag [[shutdown_requested]] to `1`. Does nothing for other signals.

Called by [[main]]. Registered through `signal`.

```c
void signal_handler(int sig)
{
    if (sig == 15 || sig == 2)
        shutdown_requested = 1;
}
```
