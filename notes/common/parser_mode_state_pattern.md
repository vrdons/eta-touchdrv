# parser_mode_state_pattern

Touch4 parser stores mode bytes near the end of parser context and forwards normalized modes to sink sub-context `parser + 0x1c8`.

Report mode byte:

```c
*(uint8_t *)(parser + 0xa478) = value;
0xf0 -> sink mode 2;
0xf1 -> sink mode 1;
0xf2 -> sink mode 0;
```

Point mode byte:

```c
*(uint8_t *)(parser + 0xa479) = value;
0 -> sink mode 1;
1 -> sink mode 2;
2 -> sink mode 3;
```

Other values only update stored state byte.

Used by [[set_parser_report_mode]], [[set_parser_point_mode]], [[apply_report_mode_to_sink_context]], and [[apply_point_mode_to_sink_context]].
