import numpy as np

N = 1024
P_MIN = 0.001
P_MAX = 1.0
SCALE = 256

p_lut = np.linspace(P_MIN, P_MAX, N)
log2_lut = np.log2(p_lut)
log2_q88 = np.round(log2_lut * SCALE).astype(np.int16)

# ==== Генериране на C/C++ масив за копиране ====
print("// Universal log2 LUT in Q8.8 format (scaled by 256)")
print("const int16_t universal_log2_LUT[1024] = {")
for i in range(0, N, 8):
    print("   " + ", ".join(f"{v:6d}" for v in log2_q88[i:i+8]) + ",")
print("};")
