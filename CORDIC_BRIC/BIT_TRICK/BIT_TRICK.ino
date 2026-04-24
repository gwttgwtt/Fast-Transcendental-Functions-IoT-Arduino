#include <math.h>
#include <stdint.h>

float fast_log2(float x) {
    union { float f; uint32_t i; } vx = { x };
    return (float)((vx.i >> 23) & 255) - 127;
}
float fast_log2_lin(float x) {
    union { float f; uint32_t i; } vx = { x };
    float y = (float)((vx.i >> 23) & 255) - 127;
    vx.i = (vx.i & 0x7FFFFF) | 0x3F800000;
    float f = vx.f;
    return y + (f - 1.0f);
}
// Schraudolph, Fast Approximations for Logarithm..., 1999
float fast_log2_poly(float x) {
    union { float f; uint32_t i; } vx = { x };
    float y = (float)((vx.i >> 23) & 255) - 127;
    vx.i = (vx.i & 0x7FFFFF) | 0x3F800000;
    float f = vx.f - 1.0f;
    return y + f * (1.3465558f + f * (-0.3606741f + f * 0.2136235f));
}

const int N = 100;
float x[N], y_math[N], y_bit[N], y_lin[N], y_poly[N];

void setup() {
  Serial.begin(115200);
  while (!Serial);

  for (int i = 0; i < N; ++i) x[i] = 0.1f + 0.1f * i;

  unsigned long t1 = micros();
  for (int i = 0; i < N; ++i) y_math[i] = log(x[i]) / log(2.0);
  unsigned long t2 = micros();
  for (int i = 0; i < N; ++i) y_bit[i] = fast_log2(x[i]);
  unsigned long t3 = micros();
  for (int i = 0; i < N; ++i) y_lin[i] = fast_log2_lin(x[i]);
  unsigned long t4 = micros();
  for (int i = 0; i < N; ++i) y_poly[i] = fast_log2_poly(x[i]);
  unsigned long t5 = micros();

  float err_bit = 0.0, err_lin = 0.0, err_poly = 0.0;
  float max_err_bit = 0.0, max_err_lin = 0.0, max_err_poly = 0.0;
  for (int i = 0; i < N; ++i) {
    float e_bit = fabs(y_math[i] - y_bit[i]);
    float e_lin = fabs(y_math[i] - y_lin[i]);
    float e_poly = fabs(y_math[i] - y_poly[i]);
    err_bit += e_bit;
    err_lin += e_lin;
    err_poly += e_poly;
    if (e_bit > max_err_bit) max_err_bit = e_bit;
    if (e_lin > max_err_lin) max_err_lin = e_lin;
    if (e_poly > max_err_poly) max_err_poly = e_poly;
  }
  err_bit /= N; err_lin /= N; err_poly /= N;

  Serial.println("---- Резултати ----");
  Serial.print("math.h log2: ");    Serial.print(t2 - t1); Serial.println(" us");
  Serial.print("bit trick (exp): ");Serial.print(t3 - t2); Serial.println(" us");
  Serial.print("bit trick + mantissa: ");Serial.print(t4 - t3); Serial.println(" us");
  Serial.print("bit trick + poly: ");Serial.print(t5 - t4); Serial.println(" us");

  Serial.print("Средна грешка (exp): "); Serial.println(err_bit, 6);
  Serial.print("Средна грешка (+man): "); Serial.println(err_lin, 6);
  Serial.print("Средна грешка (+poly): "); Serial.println(err_poly, 6);
  Serial.print("Максимална грешка (exp): "); Serial.println(max_err_bit, 6);
  Serial.print("Максимална грешка (+man): "); Serial.println(max_err_lin, 6);
  Serial.print("Максимална грешка (+poly): "); Serial.println(max_err_poly, 6);

  Serial.println("x\tmath.h\tbit\tlin\tpoly");
  for (int i = 0; i < 8; ++i) {
    Serial.print(x[i], 3); Serial.print('\t');
    Serial.print(y_math[i], 4); Serial.print('\t');
    Serial.print(y_bit[i], 4); Serial.print('\t');
    Serial.print(y_lin[i], 4); Serial.print('\t');
    Serial.println(y_poly[i], 4);
  }
}

void loop() {}
