"""
CORDIC синус (BRIC и Класически) с динамичен брой итерации и компенсация на грешка
-----------------------------------------------------------------------------------
- Позволява избор на брой стъпки (итерации)
- Автоматично преизчислява CORDIC gain (K) и ъгли atan(2^-i)
- Поддържа опционална корекция на грешката чрез полином
- Генерира sin(θ) за θ ∈ [0°, 360°]
- Сравнява BRIC, Класически CORDIC и math.sin()
"""

import numpy as np
import matplotlib.pyplot as plt
from numpy.polynomial import Polynomial
import math

# === Настройки ===
NUM_ITER = 4
USE_CORRECTION_BRIC = 0
USE_CORRECTION_CLASSIC = 0

# Автоматично преизчисляване на CORDIC gain и ъгли
def compute_cordic_constants(num_iter):
    K = np.prod([1 / math.sqrt(1 + 2 ** (-2 * i)) for i in range(num_iter)])
    brick_angles = [math.degrees(math.atan(2 ** -i)) for i in range(num_iter)]
    return K, brick_angles

K, brick_angles = compute_cordic_constants(NUM_ITER)

# === BRIC CORDIC функция ===
def cordic_sin_bric(theta_deg):
    z = 0.0
    x = 1.0
    y = 0.0
    for i in range(NUM_ITER):
        angle_deg = brick_angles[i]
        d = 1 if z < theta_deg else -1
        x_new = x - d * y * (2 ** -i)
        y_new = y + d * x * (2 ** -i)
        z += d * angle_deg
        x, y = x_new, y_new
    return y / K

# === Класически CORDIC функция ===
def cordic_sin_classic(theta_deg):
    z = math.radians(theta_deg)
    x = 1.0
    y = 0.0
    angles_rad = [math.atan(2 ** -i) for i in range(NUM_ITER)]
    for i in range(NUM_ITER):
        d = 1 if z >= 0 else -1
        x_new = x - d * y * (2 ** -i)
        y_new = y + d * x * (2 ** -i)
        z -= d * angles_rad[i]
        x, y = x_new, y_new
    return y / K

# === Полиномен модел на грешката ε(θ) ===
angles_deg = np.arange(0, 91)
true_sin = np.sin(np.radians(angles_deg))
bric_sin = np.array([cordic_sin_bric(a) for a in angles_deg])
eps = np.abs(true_sin - bric_sin)
fit_poly = Polynomial.fit(angles_deg, eps, deg=3).convert()
error_poly = fit_poly

# === Симетричен BRIC CORDIC с корекция ===
def corrected_sin_fixed_quadrants(theta_deg):
    quadrant = theta_deg // 90
    theta_mod = theta_deg % 90
    if quadrant == 0:
        θ, sign = theta_mod, 1
    elif quadrant == 1:
        θ, sign = 90 - theta_mod, 1
    elif quadrant == 2:
        θ, sign = theta_mod, -1
    elif quadrant == 3:
        θ, sign = 90 - theta_mod, -1
    base = cordic_sin_bric(θ)
    eps = error_poly(θ) if USE_CORRECTION_BRIC else 0
    return sign * (base - eps)

# === Симетричен Класически CORDIC с корекция ===
def corrected_classic_cordic(theta_deg):
    quadrant = theta_deg // 90
    theta_mod = theta_deg % 90
    if quadrant == 0:
        θ, sign = theta_mod, 1
    elif quadrant == 1:
        θ, sign = 90 - theta_mod, 1
    elif quadrant == 2:
        θ, sign = theta_mod, -1
    elif quadrant == 3:
        θ, sign = 90 - theta_mod, -1
    base = cordic_sin_classic(θ)
    eps = error_poly(θ) if USE_CORRECTION_CLASSIC else 0
    return sign * (base - eps)

# === Генериране на синуси за θ ∈ [0°, 360°] ===
theta_vals = np.arange(0, 360)
true_vals = np.sin(np.radians(theta_vals))
cordic_bric_corrected = np.array([corrected_sin_fixed_quadrants(t) for t in theta_vals])
cordic_classic_full = np.array([corrected_classic_cordic(t) for t in theta_vals])

# === Абсолютна грешка ===
error_bric = np.abs(cordic_bric_corrected - true_vals)
error_classic = np.abs(cordic_classic_full - true_vals)

# === Графика: синус функции ===
plt.figure(figsize=(10, 5))
plt.plot(theta_vals, true_vals, label='math.sin(θ)', linewidth=2, color='black')
plt.plot(theta_vals, cordic_bric_corrected,
         label='BRIC CORDIC' + (' + корекция' if USE_CORRECTION_BRIC else ''),
         linestyle='--', color='green')
plt.plot(theta_vals, cordic_classic_full,
         label='Класически CORDIC' + (' + корекция' if USE_CORRECTION_CLASSIC else ''),
         linestyle='-.', color='orange')
plt.title(f"CORDIC синус (итерации: {NUM_ITER})")
plt.xlabel("Ъгъл (градуси)")
plt.ylabel("sin(θ)")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()

# === Графика: абсолютна грешка ===
plt.figure(figsize=(10, 5))
plt.plot(theta_vals, error_bric, label='Грешка: BRIC', linestyle='--', color='green')
plt.plot(theta_vals, error_classic, label='Грешка: Класически', linestyle='-.', color='orange')
plt.title("Абсолютна грешка спрямо math.sin(θ)")
plt.xlabel("Ъгъл (градуси)")
plt.ylabel("Абсолютна грешка")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()

# === Извеждане на K и коефициентите на ε(θ) ===
print(f"\nCORDIC Gain (K) при {NUM_ITER} итерации: {K:.10f}")
print("Коефициенти на полинома ε(θ):")
for i, c in enumerate(error_poly.coef):
    print(f"  a{i} = {c:.10f}")
