# multislot_report_assembly

Touch4 packet path builds reports from multiple slot payloads.

Raw `0x40` reads are filtered by parser state, payload is copied from `buf+5`, then slot index and sequence are passed to the multislot assembler. The assembler stores per-slot payloads, waits until compatible sequence bytes are present, merges continuation records when needed, and emits a report only when slot state is coherent.

Key state concepts:

- slot count in state byte `state[1]`
- per-slot active flags
- current and previous sequence bytes
- continuation merge through [[merge_touch_report_records]]
- output count byte at `out[0xa8]`

Used by [[process_raw_0x40_device_packet]], [[assemble_multislot_packet]], [[merge_touch_report_records]], and [[dispatch_report_to_staging_sink]].
