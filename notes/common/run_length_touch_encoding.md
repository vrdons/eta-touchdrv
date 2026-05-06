# run_length_touch_encoding

Both touch2 and touch4 encode touch line state as compact run-length bytes.

Touch2 decode reads each byte as:

```c
value = code & 1;
count = code >> 1;
repeat value for count + 1 positions;
```

Touch4 merge path uses start/end interval lists and emits compact gap/side bytes. Large gaps use marker bytes (`0xfc`/`0xfd`), small gaps encode `((gap - 1) * 4) | side_bit`.

Used by [[decode]], [[extract_nonzero_spans]], [[count_nonzero_runs]], and [[encode_merged_interval_packet]].
