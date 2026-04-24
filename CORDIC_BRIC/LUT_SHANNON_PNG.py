import numpy as np
import matplotlib.pyplot as plt

# Конфигурация
N = 1024
P_MIN = 0.001
P_MAX = 1.0
SCALE = 256

# 1. Генериране на LUT (равномерно)
p_lut = np.linspace(P_MIN, P_MAX, N)
log2_lut = np.log2(p_lut)
log2_q88 = np.round(log2_lut * SCALE).astype(np.int16) / SCALE  # конвертирай до Q8.8 и обратно

# 2. Визуализация на LUT vs истинска log2(p)
p_test = np.linspace(P_MIN, P_MAX, 5000)
log2_true = np.log2(p_test)

# Без интерполация (най-близък индекс)
lut_idx = ((p_test - P_MIN) * (N - 1) / (P_MAX - P_MIN)).astype(int)
log2_lut_nearest = log2_q88[lut_idx]

# С линейна интерполация
fidx = (p_test - P_MIN) * (N - 1) / (P_MAX - P_MIN)
idx = np.floor(fidx).astype(int)
frac = fidx - idx
log2_lut_interp = log2_q88[idx] + frac * (log2_q88[np.clip(idx+1, 0, N-1)] - log2_q88[idx])

plt.figure(figsize=(10,6))
plt.plot(p_test, log2_true, label='log2(p) истинска', lw=2)
plt.plot(p_lut, log2_q88, 'o', markersize=3, label='LUT точки', alpha=0.6)
plt.plot(p_test, log2_lut_nearest, '--', label='LUT (най-близка стойност)')
plt.plot(p_test, log2_lut_interp, ':', label='LUT (интерполация)')
plt.xlabel('p')
plt.ylabel('log2(p)')
plt.title('LUT и истинска log2(p) функция')
plt.legend()
plt.grid()

plt.figure(figsize=(10,4))
plt.plot(p_test, log2_true - log2_lut_nearest, label='Грешка (най-близка)')
plt.plot(p_test, log2_true - log2_lut_interp, label='Грешка (интерполация)')
plt.title('Грешка между LUT и истинската log2(p)')
plt.xlabel('p')
plt.ylabel('Δ log2(p)')
plt.legend()
plt.grid()

plt.tight_layout()
plt.show()
