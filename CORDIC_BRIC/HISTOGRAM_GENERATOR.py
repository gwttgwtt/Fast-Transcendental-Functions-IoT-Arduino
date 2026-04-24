import numpy as np
import matplotlib.pyplot as plt

# Настройки
N_SAMPLES = 100000
HIST_BINS = 256

# 1. Равномерно разпределение
data_uniform = np.random.uniform(0, 1, N_SAMPLES)
hist_uniform, _ = np.histogram(data_uniform, bins=HIST_BINS, range=(0,1))

# 2. Нормално (Гаусово) разпределение
data_gauss = np.random.normal(0.5, 0.15, N_SAMPLES)
data_gauss = np.clip(data_gauss, 0, 1)  # Ограничаваме в [0,1]
hist_gauss, _ = np.histogram(data_gauss, bins=HIST_BINS, range=(0,1))

# 3. Експоненциално разпределение
data_exp = np.random.exponential(0.2, N_SAMPLES)
data_exp = np.clip(data_exp, 0, 1)
hist_exp, _ = np.histogram(data_exp, bins=HIST_BINS, range=(0,1))

# 4. Бимодално разпределение (2 върха)
data_bimodal = np.concatenate([
    np.random.normal(0.3, 0.07, N_SAMPLES // 2),
    np.random.normal(0.7, 0.07, N_SAMPLES // 2)
])
data_bimodal = np.clip(data_bimodal, 0, 1)
hist_bimodal, _ = np.histogram(data_bimodal, bins=HIST_BINS, range=(0,1))

# Визуализация
plt.figure(figsize=(12,6))
plt.subplot(2,2,1)
plt.bar(range(HIST_BINS), hist_uniform)
plt.title("Uniform (равномерно)")

plt.subplot(2,2,2)
plt.bar(range(HIST_BINS), hist_gauss)
plt.title("Gaussian (нормално)")

plt.subplot(2,2,3)
plt.bar(range(HIST_BINS), hist_exp)
plt.title("Exponential (експоненциално)")

plt.subplot(2,2,4)
plt.bar(range(HIST_BINS), hist_bimodal)
plt.title("Bimodal (двувърхово)")
plt.tight_layout()
plt.show()

# Експорт за Arduino (пример с гаусово)
print("uint32_t hist[HIST_BINS] = {")
for i, val in enumerate(hist_gauss):
    end = "," if i < HIST_BINS - 1 else ""
    print(f"  {val}{end}")
print("};")
