import numpy as np
import matplotlib.pyplot as plt

N = 1024
P_MIN = 0.001
P_MAX = 1.0

# --- 1. Класически LUT, равномерно по p
p_lut_uniform = np.linspace(P_MIN, P_MAX, N)
log2_lut_uniform = np.log2(p_lut_uniform)

# --- 2. Логаритмично-квантован LUT, равномерно по log(p)
log_p_min = np.log(P_MIN)
log_p_max = np.log(P_MAX)
log_p_lut = np.linspace(log_p_min, log_p_max, N)
p_lut_log = np.exp(log_p_lut)
log2_lut_log = np.log2(p_lut_log)

# --- Тестови точки
p_test = np.linspace(P_MIN, P_MAX, 5000)
log2_true = np.log2(p_test)

# --- Интерполация по равномерен LUT
fidx_uni = (p_test - P_MIN) * (N - 1) / (P_MAX - P_MIN)
idx_uni = np.floor(fidx_uni).astype(int)
frac_uni = fidx_uni - idx_uni
log2_interp_uniform = log2_lut_uniform[idx_uni] + frac_uni * (log2_lut_uniform[np.clip(idx_uni+1, 0, N-1)] - log2_lut_uniform[idx_uni])

# --- Интерполация по логаритмично-квантован LUT
# Тук индексът е по log(p)
fidx_log = (np.log(p_test) - log_p_min) * (N - 1) / (log_p_max - log_p_min)
idx_log = np.floor(fidx_log).astype(int)
frac_log = fidx_log - idx_log
log2_interp_log = log2_lut_log[idx_log] + frac_log * (log2_lut_log[np.clip(idx_log+1, 0, N-1)] - log2_lut_log[idx_log])

# --- Визуализация
plt.figure(figsize=(12, 6))
plt.plot(p_test, log2_true, label='Истинска log2(p)', lw=2)
plt.plot(p_test, log2_interp_uniform, '--', label='LUT (равномерно p)', alpha=0.7)
plt.plot(p_test, log2_interp_log, ':', label='LUT (равномерно log(p))', alpha=0.7)
plt.xscale('log')
plt.xlabel('p')
plt.ylabel('log2(p)')
plt.title('Сравнение: истинска и LUT апроксимации на log2(p)')
plt.legend()
plt.grid()

plt.figure(figsize=(12, 4))
plt.plot(p_test, log2_true - log2_interp_uniform, label='Грешка: LUT Uniform p')
plt.plot(p_test, log2_true - log2_interp_log, label='Грешка: LUT Uniform log(p)')
plt.xscale('log')
plt.xlabel('p')
plt.ylabel('Грешка')
plt.title('Грешки на двете LUT апроксимации')
plt.legend()
plt.grid()
plt.tight_layout()
plt.show()
