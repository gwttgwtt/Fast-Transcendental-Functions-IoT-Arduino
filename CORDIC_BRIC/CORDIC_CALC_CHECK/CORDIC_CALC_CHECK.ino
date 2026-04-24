#include <Arduino.h>

#define SCALE 16384
#define K_CORDIC 9949
#define ITERATIONS 16

// Таблица с atan(1/2^i) * 256
const int cordic_atan_table[ITERATIONS] = {
  11520, 6801, 3593, 1824, 916, 458, 229, 115, 57, 28, 14, 7, 4, 2, 1
};

// Предизчислени стойности на (1/2^i) * SCALE
const int shift_inv_table[ITERATIONS] = {
  16384, 8192, 4096, 2048, 1024, 512, 256, 128,
  64, 32, 16, 8, 4, 2, 1
};


// Градуси към вътрешен ъгъл в CORDIC (градуси * 256)
int degToCordicAngle(int deg) {
  return deg << 8;
}

// CORDIC алгоритъм: връща целочислени sin и cos, без float
void cordic_sin_cos(int theta_deg, int &cos_out, int &sin_out) {
  long x = K_CORDIC;
  long y = 0;
  long z = degToCordicAngle(theta_deg);

  bool flipX = false, flipY = false;

  if (theta_deg >= 90 && theta_deg < 180) {
    z = degToCordicAngle(180 - theta_deg);
    flipX = true;
  } else if (theta_deg >= 180 && theta_deg < 270) {
    z = degToCordicAngle(theta_deg - 180);
    flipX = true;
    flipY = true;
  } else if (theta_deg >= 270) {
    z = degToCordicAngle(360 - theta_deg);
    flipY = true;
  }

  for (int i = 0; i < ITERATIONS; i++) {
    long x_new, y_new;
    if (z >= 0) {
      x_new = x - (y >> i);
      y_new = y + (x >> i);
      z -= cordic_atan_table[i];
    } else {
      x_new = x + (y >> i);
      y_new = y - (x >> i);
      z += cordic_atan_table[i];
    }
    x = x_new;
    y = y_new;
  }

  if (flipX) x = -x;
  if (flipY) y = -y;

  cos_out = x;
  sin_out = y;
}

uint8_t cordicToPWM(int val) {
  // Клипваме стойностите, ако излязат от обхвата
  if (val > SCALE) val = SCALE;
  if (val < -SCALE) val = -SCALE;

  // Нормализиране към 0–255
  return (uint8_t)(((long)(val + SCALE) * 255) >> 15);
}


void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("CORDIC vs math.sin() [0–255] PWM values");
  Serial.println("=========================================");
}

void loop() {
  Serial.println("Angle\tCORDIC\tStandard\tDiff\tPWM");

  for (int angle_deg = 0; angle_deg <= 360; angle_deg++) {
    // === Стандартен синус (float) ===
    float sin_std = sin(angle_deg * PI / 180.0);
    uint8_t pwm_std = (uint8_t)((sin_std + 1.0) * 127.5);

    // === CORDIC ===
    int sin_cordic, cos_cordic;
    cordic_sin_cos(angle_deg, cos_cordic, sin_cordic);
    float sin_c = (float)sin_cordic / SCALE;
    uint8_t pwm_cordic = cordicToPWM(sin_cordic);

    // === Разлика ===
    float diff = sin_std - sin_c;

    // === Печат на ред ===
    Serial.print(angle_deg);
    Serial.print("\t");
    Serial.print(sin_c, 5);
    Serial.print("\t");
    Serial.print(sin_std, 5);
    Serial.print("\t");
    Serial.print(diff, 5);
    Serial.print("\t");
    Serial.println(pwm_cordic);
    
    delay(10); // малка пауза за четимост
  }

  while (true);  // Спира изпълнението след теста
}
