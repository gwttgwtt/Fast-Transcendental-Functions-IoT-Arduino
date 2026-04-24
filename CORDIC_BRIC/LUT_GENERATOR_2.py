import numpy as np

LUT_SIZE = 1024
LUT_MIN = 0.0001
LUT_MAX = 1.0
LUT_SCALE = 256.0

p = np.linspace(LUT_MIN, LUT_MAX, LUT_SIZE)
log2p = (np.log2(p) * LUT_SCALE).round().astype(int)

print("const int16_t log2_lut1024_lin[] = {")
print(", ".join(map(str, log2p)))
print("};")
