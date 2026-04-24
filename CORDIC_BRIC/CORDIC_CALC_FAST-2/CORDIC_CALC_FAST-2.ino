#include <Arduino.h>

#define SCALE 16384
#define K_CORDIC 9949
#define ITERATIONS 16

int g_c_steps = ITERATIONS;  // по подразбиране — 16


int g_c_sin = 0;
int g_c_cos = 0;

int cordic_atan_table[ITERATIONS] = {
  11520, 6801, 3593, 1824, 916, 458, 229, 115,
  57, 28, 14, 7, 4, 2, 1, 0
};

// 🔁 Макрос за циклово разгръщане
#define CORDIC_STEP(i) { \
  long x_shift = x >> i; \
  long y_shift = y >> i; \
  long x_new, y_new; \
  if (z >= 0) { \
    x_new = x - y_shift; \
    y_new = y + x_shift; \
    z -= cordic_atan_table[i]; \
  } else { \
    x_new = x + y_shift; \
    y_new = y - x_shift; \
    z += cordic_atan_table[i]; \
  } \
  x = x_new; y = y_new; \
}

// === CORDIC ===
inline void cordic_sin_cos(int theta_deg) {
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

if (g_c_steps > 0)  CORDIC_STEP(0);
if (g_c_steps > 1)  CORDIC_STEP(1);
if (g_c_steps > 2)  CORDIC_STEP(2);
if (g_c_steps > 3)  CORDIC_STEP(3);
if (g_c_steps > 4)  CORDIC_STEP(4);
if (g_c_steps > 5)  CORDIC_STEP(5);
if (g_c_steps > 6)  CORDIC_STEP(6);
if (g_c_steps > 7)  CORDIC_STEP(7);
if (g_c_steps > 8)  CORDIC_STEP(8);
if (g_c_steps > 9)  CORDIC_STEP(9);
if (g_c_steps > 10) CORDIC_STEP(10);
if (g_c_steps > 11) CORDIC_STEP(11);
if (g_c_steps > 12) CORDIC_STEP(12);
if (g_c_steps > 13) CORDIC_STEP(13);
if (g_c_steps > 14) CORDIC_STEP(14);
if (g_c_steps > 15) CORDIC_STEP(15);


  if (flipX) x = -x;
  if (flipY) y = -y;

  g_c_cos = x;
  g_c_sin = y;
}

uint8_t cordicToPWM(int val) {
  if (val > SCALE) val = SCALE;
  if (val < -SCALE) val = -SCALE;
  return (uint8_t)(((long)(val + SCALE) * 255) >> 15);
}

void processSHOWCommand(int steps) {
  if (steps < 1 || steps > ITERATIONS) {
    Serial.println("Invalid iteration count. Use SHOW <1–16>");
    return;
  }

  Serial.print("Running with ");
  Serial.print(steps);
  Serial.println(" LUT iterations...\n");

  Serial.println("Angle\tCORDIC\tStandard\tDiff\tPWM_C\tPWM_STD\tTime_C(us)\tTime_STD(us)");
  Serial.println("====================================================================");
  
  g_c_steps = steps;
  
  for (int angle_deg = 0; angle_deg <= 360; angle_deg++) {
    unsigned long t1_std = micros();
    float sin_std = sin(angle_deg * PI / 180.0);
    unsigned long t2_std = micros();
    uint8_t pwm_std = (uint8_t)((sin_std + 1.0) * 127.5);

    unsigned long t1_cordic = micros();
    cordic_sin_cos(angle_deg);
    unsigned long t2_cordic = micros();
    float sin_c = (float)g_c_sin / SCALE;
    uint8_t pwm_cordic = cordicToPWM(g_c_sin);

    float diff = sin_std - sin_c;

    Serial.print(angle_deg); Serial.print("\t");
    Serial.print(sin_c, 5); Serial.print("\t");
    Serial.print(sin_std, 5); Serial.print("\t");
    Serial.print(diff, 5); Serial.print("\t");
    Serial.print(pwm_cordic); Serial.print("\t");
    Serial.print(pwm_std); Serial.print("\t");
    Serial.print(t2_cordic - t1_cordic); Serial.print("\t");
    Serial.println(t2_std - t1_std);

    delay(5);
  }

  Serial.println("\nDone.\n");
}

void handleSerialInput() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.startsWith("SHOW")) {
      int spaceIdx = input.indexOf(' ');
      if (spaceIdx != -1) {
        int steps = input.substring(spaceIdx + 1).toInt();
        processSHOWCommand(steps);
      } else {
        Serial.println("Usage: SHOW <1–16>");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Ready. Type SHOW <1–16> to begin calculation.");
}

void loop() {
  handleSerialInput();
}
