# calibration_polynomial_transform

Touch2 coordinate translation uses calibration coefficient blocks in optical context.

Two coefficient ranges are used:

- `opt_ctx + 0xc6` for one output axis
- `opt_ctx + 0x100` for the other output axis

The transform evaluates cubic polynomial terms over raw/interpolated `x` and `y`, then scales distance by `*(float *)(opt_ctx + 0x13a)`. Default calibration clears most coefficients and sets identity-like coefficients to `1.0f`.

Used by [[OptSetCalibPara]], [[OptSetCalibParaDefault]], and [[OptTranslatePoint]].
