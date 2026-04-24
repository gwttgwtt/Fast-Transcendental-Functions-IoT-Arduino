"""
BRIC CORDIC Sinus Generator with Error Compensation and Quadrant Extension
-----------------------------------------------------------------------------
Автор: ChatGPT (по заявка на потребител)

Описание:
Този скрипт реализира генериране на синусоида в диапазона 0° до 360°, използвайки:
  • BRIC CORDIC алгоритъм с 6 итерации (приближение чрез завъртане на вектор)
  • Полиномен модел от 3-та степен за корекция на системната грешка ε(θ)
  • Отражения и знаци по квадранти (I–IV) за пълна 360° поддръжка

Стъпки на изпълнение:
1. BRIC CORDIC приближава sin(θ) в интервала [0°, 90°]
2. Изчислява се абсолютната грешка между CORDIC и math.sin
3. Строи се полиномен модел ε(θ) чрез Least Squares (numpy.polynomial)
4. Извежда се синус за 0–360° чрез:
   - нормализация на ъгъла към квадрант
   - корекция с ε(θ)
   - знакова симетрия

Изход:
• Графика със сравнение между math.sin и коригирания CORDIC синус
• Коефициенти на полинома, готови за пренасяне към C/Arduino код

Приложение:
• Генериране на синусоидален сигнал върху микроконтролери (Arduino, STM32)
• PWM DAC синус
• Вградени системи без хардуерен FPU

"""

import numpy as np
import matplotlib.pyplot as plt
from numpy.polynomial import Polynomial
import math

# --- Настройки ---
NUM_ITER = 6
angles_deg = np.arange(0, 91)
K = np.prod([1 / math.sqrt(1 + 2 ** (-2 * i)) for i in range(NUM_ITER)])
brick_angles = [math.degrees(math.atan(2 ** -i)) for i in range(NUM_ITER)]

# --- BRIC CORDIC функция ---
def cordic_sin_bric(theta_deg):
    """CORDIC синус в диапазона 0–90°"""
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

# --- Изчисляване на грешка и полином ---
true_sin = np.sin(np.radians(angles_deg))
bric_sin = np.array([cordic_sin_bric(a) for a in angles_deg])
eps = np.abs(true_sin - bric_sin)
fit_poly = Polynomial.fit(angles_deg, eps, deg=3).convert()
error_poly = fit_poly  # използваме директно модела

# --- Коригиран BRIC синус за 0°–360° ---
def corrected_sin_fixed_quadrants(theta_deg):
    """Синус за 0–360°, с правилна обработка на квадрантите"""
    quadrant = theta_deg // 90
    theta_mod = theta_deg % 90

    if quadrant == 0:
        θ = theta_mod
        sign = 1
    elif quadrant == 1:
        θ = 90 - theta_mod
        sign = 1
    elif quadrant == 2:
        θ = theta_mod
        sign = -1
    elif quadrant == 3:
        θ = 90 - theta_mod
        sign = -1

    base = cordic_sin_bric(θ)
    eps = error_poly(θ)
    sinθ = base - eps

    return sign * sinθ

# --- Генериране на синусоида за 0–360° ---
theta_vals = np.arange(0, 360)
cordic_fixed_vals = np.array([corrected_sin_fixed_quadrants(theta) for theta in theta_vals])
true_vals = np.sin(np.radians(theta_vals))

# --- Визуализация ---
plt.figure(figsize=(10, 5))
plt.plot(theta_vals, true_vals, label='math.sin(θ)', linewidth=2)
plt.plot(theta_vals, cordic_fixed_vals, label='CORDIC + корекция + квадранти', linestyle='--')
plt.title("Синус от 0° до 360° (BRIC + корекция + квадранти)")
plt.xlabel("Ъгъл (градуси)")
plt.ylabel("sin(θ)")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()

# --- Показване на коефициентите на ε(θ) ---
print("Коефициенти на полинома ε(θ):")
for i, c in enumerate(error_poly.coef):
    print(f"  a{i} = {c:.10f}")
