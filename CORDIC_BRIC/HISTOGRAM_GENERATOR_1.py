import numpy as np
import matplotlib.pyplot as plt

HIST_BINS = 256
TOTAL_EVENTS = 100000  # Общо събития

# --- Избери тип на разпределението ---
# Варианти: 'uniform', 'gauss', 'expo', 'bimodal'
distribution_type = "bimodal"    # <-- промени тук за тест!

# --- Генерирай тестови проби според избрания тип ---
if distribution_type == "uniform":
    samples = np.random.uniform(0, 1, TOTAL_EVENTS)
elif distribution_type == "gauss":
    samples = np.clip(np.random.normal(0.5, 0.15, TOTAL_EVENTS), 0, 1)
elif distribution_type == "expo":
    samples = np.clip(np.random.exponential(0.2, TOTAL_EVENTS), 0, 1)
elif distribution_type == "bimodal":
    s1 = np.clip(np.random.normal(0.3, 0.1, TOTAL_EVENTS//2), 0, 1)
    s2 = np.clip(np.random.normal(0.7, 0.1, TOTAL_EVENTS//2), 0, 1)
    samples = np.concatenate((s1, s2))
else:
    raise ValueError(f"Невалиден тип разпределение: {distribution_type}")

# --- Генерирай хистограма ---
hist, edges = np.histogram(samples, bins=HIST_BINS, range=(0, 1))

# --- Принтирай като масив за Arduino ---
print("// Разпределение:", distribution_type)
print("uint32_t hist[{}] = {{".format(HIST_BINS))
for i in range(HIST_BINS):
    print("  {:6d},".format(hist[i]), end="")
    if (i+1)%8 == 0: print()
print("};")
print("const uint32_t total_samples = {};\n".format(np.sum(hist)))

# --- Визуализация (по желание) ---
plt.bar(range(HIST_BINS), hist, color='royalblue')
plt.title(f"Histogram ({distribution_type})")
plt.xlabel("bin")
plt.ylabel("count")
plt.tight_layout()
plt.show()
