# merge_touch_report_records

Related common patterns: [[multislot_report_assembly]], [[run_length_touch_encoding]].

Merges two 0x2a-byte touch report records into output record. It allocates local interval tables, counts nonzero edge runs with [[count_nonzero_runs]], orders the two inputs by that count, extracts edge intervals with [[extract_nonzero_spans]], compares intervals with [[range_separation_distance]], builds a dynamic match table, then emits merged packet bytes into third argument. If verbose global [[verbose_level]] is nonzero and either input has intervals, it logs diagnostic geometry through `printf`. Final output bytes are selected from first input, second input, or min/max comparisons over first/last interval endpoints.

Called by [[assemble_multislot_packet]]. Called functions: [[count_nonzero_runs]], [[extract_nonzero_spans]], [[range_separation_distance]], [[encode_merged_interval_packet]], `printf`.

```c
void merge_touch_report_records(unsigned char *a, unsigned char *b, unsigned char *out)
{
    uint16_t a_ranges[18][2];
    uint16_t b_ranges[18][2];
    uint16_t match[19][19][2];
    unsigned char a_count;
    unsigned char b_count;
    unsigned char ac;
    unsigned char bc;
    unsigned char i;
    unsigned char j;
    unsigned char best;
    unsigned char *first;
    unsigned char *second;
    uint16_t diff;

    ac = count_nonzero_runs(a);
    bc = count_nonzero_runs(b);
    if (bc < ac) {
        first = a;
        second = b;
    } else {
        first = b;
        second = a;
    }

    extract_nonzero_spans(first, a_ranges, &a_count);
    extract_nonzero_spans(second, b_ranges, &b_count);

    if (a_count != 0) {
        match[0][0][0] = range_separation_distance(a_ranges, b_ranges);
        match[0][0][1] = 0;
        for (i = 1; i < b_count; i++) {
            match[0][i][0] = match[0][i - 1][0];
            diff = range_separation_distance(a_ranges, &b_ranges[i]);
            if (diff < match[0][i][0]) {
                match[0][i][0] = diff;
                match[0][i][1] = i;
            }
        }
    }

    for (i = 1; i < a_count; i++) {
        best = i;
        diff = range_separation_distance(&a_ranges[i], &b_ranges[i]);
        match[i][i][0] = match[i - 1][i - 1][0] + diff;
        match[i][i][1] = i;
        for (j = i + 1; j < b_count; j++) {
            match[i][j] = match[i][j - 1];
            diff = match[i - 1][j - 1][0] + range_separation_distance(&a_ranges[i], &b_ranges[j]);
            if (diff < match[i][j][0]) {
                match[i][j][0] = diff;
                match[i][j][1] = j;
            }
        }
    }

    j = b_count;
    i = a_count;
    while (i != 0) {
        uint16_t idx = match[i - 1][j - 1][1];
        if (b_ranges[idx][0] < a_ranges[i - 1][0])
            a_ranges[i - 1][0] = b_ranges[idx][0];
        if (a_ranges[i - 1][1] < b_ranges[idx][1])
            a_ranges[i - 1][1] = b_ranges[idx][1];
        j = idx;
        i--;
    }

    if ((a_count != 0 || b_count != 0) && *(int *)0x4f5310 != 0)
        printf(0x4a55a8, 0x4a5678,
                   a_ranges[0][0], a_ranges[0][1], a_count,
                   b_ranges[0][0], b_ranges[0][1], b_count);

    encode_merged_interval_packet(a_ranges, a_count, b_ranges, b_count, out);

    if (a_count != 0 && b_count != 0) {
        if (a_ranges[0][0] < b_ranges[0][0])
            out[0] = first[0];
        else if (b_ranges[0][0] < a_ranges[0][0])
            out[0] = second[0];
        else
            out[0] = first[0] <= second[0] ? first[0] : second[0];

        if (b_ranges[b_count - 1][1] < a_ranges[a_count - 1][1])
            out[1] = first[1];
        else if (a_ranges[a_count - 1][1] < b_ranges[b_count - 1][1])
            out[1] = second[1];
        else
            out[1] = first[1] >= second[1] ? first[1] : second[1];
    } else {
        if (a_count != 0) {
            out[0] = first[0];
            out[1] = first[1];
        }
        if (b_count != 0) {
            out[0] = second[0];
            out[1] = second[1];
        }
    }
}
```
