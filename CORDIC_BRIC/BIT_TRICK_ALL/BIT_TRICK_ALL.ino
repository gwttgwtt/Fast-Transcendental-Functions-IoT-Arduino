#include <math.h>
#include <stdint.h>

// Fast sqrt(x) using Quake trick
float fast_inv_sqrt(float x) {
    float xhalf = 0.5f * x;
    uint32_t i;
    memcpy(&i, &x, sizeof(float));
    i = 0x5f3759df - (i >> 1);
    memcpy(&x, &i, sizeof(float));
    x = x * (1.5f - xhalf * x * x);
    return x;
}
float fast_sqrt(float x) {
    return x * fast_inv_sqrt(x);
}

// Fast pow2(x)
float fast_pow2(float x) {
    uint32_t i = (uint32_t)((x + 127.0f) * (1 << 23));
    float f;
    memcpy(&f, &i, sizeof(float));
    return f;
}


// Fast exp(x) ≈ 2^(x/ln2)
float fast_exp(float x) {
    return fast_pow2(x * 1.44269504f);
}

// Fast fabs
float fast_abs(float x) {
    uint32_t u;
    memcpy(&u, &x, sizeof(float));
    u &= 0x7FFFFFFF;
    memcpy(&x, &u, sizeof(float));
    return x;
}


// Fast sign
float fast_sign(float x) {
    return (x > 0) - (x < 0);
}

// Fast sigmoid (approximation)
float fast_sigmoid(float x) {
    return 0.5f * (x / (1.0f + fabsf(x))) + 0.5f; // Ако искаш стойност в [0,1]
}

const int N = 50; // <= 100 за AVR
float x[N], y_std[N], y_fast[N];

void print_results(const char* label, float* x, float* y1, float* y2, int N) {
    Serial.print(label); Serial.println(": x\tmath.h\tbit/fast");
    for (int i = 0; i < 8 && i < N; ++i) {
        Serial.print(x[i], 4); Serial.print('\t');
        Serial.print(y1[i], 4); Serial.print('\t');
        Serial.println(y2[i], 4);
    }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // SQRT(x) TEST
  for (int i = 0; i < N; ++i) x[i] = 0.1f + 0.2f * i; // x = 0.1..10.0

  unsigned long t1 = micros();
  for (int i = 0; i < N; ++i) y_std[i] = sqrtf(x[i]);
  unsigned long t2 = micros();
  for (int i = 0; i < N; ++i) y_fast[i] = fast_sqrt(x[i]);
  unsigned long t3 = micros();

  float err_sum = 0, max_err = 0;
  for (int i = 0; i < N; ++i) {
    float e = fabs(y_std[i] - y_fast[i]);
    err_sum += e;
    if (e > max_err) max_err = e;
  }
  Serial.println("==== SQRT(x) ====");
  Serial.print("math.h: "); Serial.print(t2-t1); Serial.print(" us,  ");
  Serial.print("fast: "); Serial.print(t3-t2); Serial.println(" us");
  Serial.print("mean err: "); Serial.print(err_sum/N, 6);
  Serial.print("   max err: "); Serial.println(max_err, 6);
  print_results("SQRT", x, y_std, y_fast, N);

  // POW2(x) TEST (x от -2 до +6)
  for (int i = 0; i < N; ++i) x[i] = -2.0f + 0.16f * i;
  t1 = micros();
  for (int i = 0; i < N; ++i) y_std[i] = powf(2.0f, x[i]);
  t2 = micros();
  for (int i = 0; i < N; ++i) y_fast[i] = fast_pow2(x[i]);
  t3 = micros();
  err_sum = 0; max_err = 0;
  for (int i = 0; i < N; ++i) {
    float e = fabs(y_std[i] - y_fast[i]);
    err_sum += e;
    if (e > max_err) max_err = e;
  }
  Serial.println("==== POW2(x) ====");
  Serial.print("math.h: "); Serial.print(t2-t1); Serial.print(" us,  ");
  Serial.print("fast: "); Serial.print(t3-t2); Serial.println(" us");
  Serial.print("mean err: "); Serial.print(err_sum/N, 6);
  Serial.print("   max err: "); Serial.println(max_err, 6);
  print_results("POW2", x, y_std, y_fast, N);

  // EXP(x) TEST (x от -2 до +4)
  for (int i = 0; i < N; ++i) x[i] = -2.0f + 0.12f * i;
  t1 = micros();
  for (int i = 0; i < N; ++i) y_std[i] = expf(x[i]);
  t2 = micros();
  for (int i = 0; i < N; ++i) y_fast[i] = fast_exp(x[i]);
  t3 = micros();
  err_sum = 0; max_err = 0;
  for (int i = 0; i < N; ++i) {
    float e = fabs(y_std[i] - y_fast[i]);
    err_sum += e;
    if (e > max_err) max_err = e;
  }
  Serial.println("==== EXP(x) ====");
  Serial.print("math.h: "); Serial.print(t2-t1); Serial.print(" us,  ");
  Serial.print("fast: "); Serial.print(t3-t2); Serial.println(" us");
  Serial.print("mean err: "); Serial.print(err_sum/N, 6);
  Serial.print("   max err: "); Serial.println(max_err, 6);
  print_results("EXP", x, y_std, y_fast, N);

  // ABS(x) TEST
  for (int i = 0; i < N; ++i) x[i] = -5.0f + 0.2f * i;
  t1 = micros();
  for (int i = 0; i < N; ++i) y_std[i] = fabsf(x[i]);
  t2 = micros();
  for (int i = 0; i < N; i++) y_fast[i] = fast_abs(x[i]);
  t3 = micros();
  err_sum = 0; max_err = 0;
  for (int i = 0; i < N; ++i) {
    float e = fabs(y_std[i] - y_fast[i]);
    err_sum += e;
    if (e > max_err) max_err = e;
  }
  Serial.println("==== ABS(x) ====");
  Serial.print("math.h: "); Serial.print(t2-t1); Serial.print(" us,  ");
  Serial.print("fast: "); Serial.print(t3-t2); Serial.println(" us");
  Serial.print("mean err: "); Serial.print(err_sum/N, 6);
  Serial.print("   max err: "); Serial.println(max_err, 6);
  print_results("ABS", x, y_std, y_fast, N);

  // SIGN(x) TEST
  for (int i = 0; i < N; ++i) x[i] = -5.0f + 0.2f * i;
  t1 = micros();
  for (int i = 0; i < N; ++i) y_std[i] = (x[i] > 0) - (x[i] < 0); // math.h signbit не винаги дава ±1
  t2 = micros();
  for (int i = 0; i < N; ++i) y_fast[i] = fast_sign(x[i]);
  t3 = micros();
  err_sum = 0; max_err = 0;
  for (int i = 0; i < N; ++i) {
    float e = fabs(y_std[i] - y_fast[i]);
    err_sum += e;
    if (e > max_err) max_err = e;
  }
  Serial.println("==== SIGN(x) ====");
  Serial.print("math.h: "); Serial.print(t2-t1); Serial.print(" us,  ");
  Serial.print("fast: "); Serial.print(t3-t2); Serial.println(" us");
  Serial.print("mean err: "); Serial.print(err_sum/N, 6);
  Serial.print("   max err: "); Serial.println(max_err, 6);
  print_results("SIGN", x, y_std, y_fast, N);

  // SIGMOID(x) TEST
  for (int i = 0; i < N; ++i) x[i] = -4.0f + 0.16f * i;
  t1 = micros();
  for (int i = 0; i < N; ++i) y_std[i] = 1.0f / (1.0f + expf(-x[i]));
  t2 = micros();
  for (int i = 0; i < N; ++i) y_fast[i] = fast_sigmoid(x[i]);
  t3 = micros();
  err_sum = 0; max_err = 0;
  for (int i = 0; i < N; ++i) {
    float e = fabs(y_std[i] - y_fast[i]);
    err_sum += e;
    if (e > max_err) max_err = e;
  }
  Serial.println("==== SIGMOID(x) ====");
  Serial.print("math.h: "); Serial.print(t2-t1); Serial.print(" us,  ");
  Serial.print("fast: "); Serial.print(t3-t2); Serial.println(" us");
  Serial.print("mean err: "); Serial.print(err_sum/N, 6);
  Serial.print("   max err: "); Serial.println(max_err, 6);
  print_results("SIGMOID", x, y_std, y_fast, N);

  Serial.println("\n--- КРАЙ НА ТЕСТА ---");
}

void loop() {}
