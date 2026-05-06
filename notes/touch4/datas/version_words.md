# version_words

Version number words used by [[main]]. Raw values at [[version_words]] are four 16-bit fields.

Observed bytes: `01 00 07 00 15 1b b7 4f`

```c
unsigned short version_words[4] = { 1, 7, 0x1b15, 0x4fb7 };
```
