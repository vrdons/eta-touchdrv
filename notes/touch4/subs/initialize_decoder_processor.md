# initialize_decoder_processor

Decoder/processor setup step inside [[initialize_packet_parser]]. Binds main context, parser sub-buffer, and work area. If it fails, [[initialize_packet_parser]] returns `0`.

```c
int initialize_decoder_processor(void *decoder, void *ctx, void *work)
{
    return 1;
}
```
