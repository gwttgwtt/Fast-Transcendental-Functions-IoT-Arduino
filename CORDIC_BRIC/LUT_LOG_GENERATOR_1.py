import numpy as np

N = 1024
P_MIN = 0.0001
P_MAX = 1.0
SCALE = 256

# Генерирай равномерно по log(p) (log-скала за повече точност при малки p)
logp = np.linspace(np.log(P_MIN), np.log(P_MAX), N)
p_lut = np.exp(logp)
log2_lut = np.log2(p_lut)
log2_q88 = np.round(log2_lut * SCALE).astype(np.int16)

# Експорт за Arduino (.h файл)
print("const int16_t log2_lut1024_logscale[{}] = {{".format(N))
for i, val in enumerate(log2_q88):
    print("{:6d},".format(val), end=' ')
    if (i+1)%8 == 0:
        print()
print("};")
