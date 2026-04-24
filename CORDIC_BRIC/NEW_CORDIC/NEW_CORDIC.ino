#include <Arduino.h>
#include <math.h>

// --- Константи за CORDIC ---
#define cordic_1K 0x26DD3B6A        // 1/K ≈ 0.6072529 в Q2.30
#define MUL 1073741824.0           // 2^30
#define half_pi 0x6487ED51         // π/2 в Q2.30
#define pi 0xC90FDAA2              // π в Q2.30
#define CORDIC_NTAB 32

// Таблица с arctan(1/2^i) в Q2.30
const int cordic_ctab[] = {
  0x3243F6A8, 0x1DAC6705, 0x0FADBAFC, 0x07F56EA6, 0x03FEAB76, 0x01FFD55B,
  0x00FFFAAA, 0x007FFF55, 0x003FFFEA, 0x001FFFFD, 0x000FFFFF, 0x0007FFFF,
  0x0003FFFF, 0x0001FFFF, 0x0000FFFF, 0x00007FFF, 0x00003FFF, 0x00001FFF,
  0x00000FFF, 0x000007FF, 0x000003FF, 0x000001FF, 0x000000FF, 0x0000007F,
  0x0000003F, 0x0000001F, 0x0000000F, 0x00000008, 0x00000004, 0x00000002,
  0x00000001, 0x00000000
};

// --- CORDIC функция ---
void cordic(int theta, int *s_out, int *c_out, int n) {
  int k, d, tx, ty, tz;
  int x = cordic_1K;
  int y = 0;
  int z = theta;

  n = (n > CORDIC_NTAB) ? CORDIC_NTAB : n;

  for (k = 0; k < n; ++k) {
    d = z >> 31;
    tx = x - (((y >> k) ^ d) - d);
    ty = y + (((x >> k) ^ d) - d);
    tz = z - ((cordic_ctab[k] ^ d) - d);
    x = tx;
    y = ty;
    z = tz;
  }

  *c_out = x;
  *s_out = y;
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  // Примерен ъгъл
  float angle_deg = 30.0;
  float angle_rad = angle_deg * 3.1415926535 / 180.0;
  int theta_fixed = angle_rad * MUL;

  // Променливи за резултата от CORDIC
  int sin_cordic_raw, cos_cordic_raw;
  cordic(theta_fixed, &sin_cordic_raw, &cos_cordic_raw, 31);

  // Преобразуване обратно във float
  float sin_cordic = (float)sin_cordic_raw / MUL;
  float cos_cordic = (float)cos_cordic_raw / MUL;

  // Стандартни стойности с math.h
  float sin_math = sin(angle_rad);
  float cos_math = cos(angle_rad);

  // --- Извеждане ---
  Serial.println("=== CORDIC Тест ===");
  Serial.print("Ъгъл (градуси): "); Serial.println(angle_deg);
  Serial.print("CORDIC sin(θ):  "); Serial.println(sin_cordic, 6);
  Serial.print("CORDIC cos(θ):  "); Serial.println(cos_cordic, 6);
  Serial.print("math.h sin(θ):  "); Serial.println(sin_math, 6);
  Serial.print("math.h cos(θ):  "); Serial.println(cos_math, 6);
}

void loop() {
  // Нищо
}
