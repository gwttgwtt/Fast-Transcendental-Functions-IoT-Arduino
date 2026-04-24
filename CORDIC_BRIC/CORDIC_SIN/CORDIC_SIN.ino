#include <Arduino.h>

#define SCALE 16384
#define K_CORDIC 9949
#define ITERATIONS 16

int cordic_atan_table[ITERATIONS] = {
  11520, 6801, 3593, 1824, 916, 458, 229, 115,
  57, 28, 14, 7, 4, 2, 1, 0
};

int LUT_STEPS = 16;      // глобална: брой итерации на CORDIC (1–16)
int ANGLE_STEP = 6;      // глобална: стъпка на ъгъла (например 1, 2, 4, 8...)

int angle = 0;           // текущ ъгъл

// === CORDIC ===
void cordic_sin_cos(int theta_deg, int &cos_out, int &sin_out, int steps) {
  long x = K_CORDIC, y = 0;
  long z = theta_deg << 8;
  bool flipX = false, flipY = false;

  if (theta_deg >= 90 && theta_deg < 180) {
    z = (180 - theta_deg) << 8;
    flipX = true;
  } else if (theta_deg >= 180 && theta_deg < 270) {
    z = (theta_deg - 180) << 8;
    flipX = true;
    flipY = true;
  } else if (theta_deg >= 270) {
    z = (360 - theta_deg) << 8;
    flipY = true;
  }

  for (int i = 0; i < steps; i++) {
    long x_shift = x >> i;
    long y_shift = y >> i;
    long x_new, y_new;

    if (z >= 0) {
      x_new = x - y_shift;
      y_new = y + x_shift;
      z -= cordic_atan_table[i];
    } else {
      x_new = x + y_shift;
      y_new = y - x_shift;
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
  if (val > SCALE) val = SCALE;
  if (val < -SCALE) val = -SCALE;
  return (uint8_t)(((long)(val + SCALE) * 255) >> 15);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("CORDIC PWM Generator ready.");
  pinMode(9, OUTPUT);
}

void loop() {
  int sin_val, cos_val;

  cordic_sin_cos(angle, cos_val, sin_val, LUT_STEPS);
  uint8_t pwm = cordicToPWM(sin_val);
  analogWrite(9, pwm);

  angle += ANGLE_STEP;
  if (angle >= 360) angle = 0;

  //delayMicroseconds(500); // настрой скоростта по желание
}
