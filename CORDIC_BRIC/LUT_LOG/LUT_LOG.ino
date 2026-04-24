#include <Arduino.h>
#include <math.h>
#include "log2_lut_linear.h"
#include "hist_expo.h" //"hist_gaus.h"

#define LUT_SIZE 1024      // Размер на LUT (както е в .h файла)
#define LUT_SCALE 256.0f   // Мащабиране (ако си експортирал *256)
#define LUT_MIN 0.0001f    // Минимално p за LUT
#define LUT_MAX 1.0f       // Максимално p за LUT

#define HIST_BINS 256
float probabilities[HIST_BINS];

float lut_log2_select_linear1(float p) {
  if (p < LUT_MIN) p = LUT_MIN;
  if (p > LUT_MAX) p = LUT_MAX;
  float pos = (p - LUT_MIN) * (LUT_SIZE - 1) / (LUT_MAX - LUT_MIN);
  int idx = (int)pos;
  float frac = pos - idx;
  float val0 = log2_lut1024_lin[idx] / LUT_SCALE;
  if (idx < LUT_SIZE - 1) {
    float val1 = log2_lut1024_lin[idx + 1] / LUT_SCALE;
    return val0 * (1.0f - frac) + val1 * frac;
  }
  return val0;
}

float lut_log2_select_linear2(float p) {
  // Ограничаваме p в интервала на LUT
  if (p < LUT_MIN) p = LUT_MIN;
  if (p > LUT_MAX) p = LUT_MAX;
  
  // Нормализираме p към позиция в LUT
  float pos = (p - LUT_MIN) * (LUT_SIZE - 1) / (LUT_MAX - LUT_MIN);
  int idx = (int)pos;
  float frac = pos - idx;

  // Взимаме две съседни стойности и интерполираме между тях
  float val0 = log2_lut1024_lin[idx] / LUT_SCALE;
  if (idx < LUT_SIZE - 1) {
    float val1 = log2_lut1024_lin[idx + 1] / LUT_SCALE;
    return val0 * (1.0f - frac) + val1 * frac;
  }
  return val0;
}


// Коректна функция за log(p)-разпределена LUT с линейна интерполация
float lut_log2_select_linear(float p) {
  if (p < LUT_MIN) p = LUT_MIN;
  if (p > LUT_MAX) p = LUT_MAX;
  float pos = (log(p) - log(LUT_MIN)) / (log(LUT_MAX) - log(LUT_MIN)) * (LUT_SIZE - 1);
  int idx = (int)pos;
  float frac = pos - idx;
  if (idx < LUT_SIZE - 1) {
    float val0 = log2_lut1024_lin[idx] / LUT_SCALE;
    float val1 = log2_lut1024_lin[idx + 1] / LUT_SCALE;
    return val0 * (1.0f - frac) + val1 * frac;
  } else {
    return log2_lut1024_lin[idx] / LUT_SCALE;
  }
}

float lut_log2_select(float p) {
  if (p < LUT_MIN) p = LUT_MIN;
  if (p > LUT_MAX) p = LUT_MAX;
  float pos = (log(p) - log(LUT_MIN)) / (log(LUT_MAX) - log(LUT_MIN)) * (LUT_SIZE - 1);
  int idx = (int)(pos + 0.5f); // закръгляне до най-близкия индекс
  if (idx < 0) idx = 0;
  if (idx >= LUT_SIZE) idx = LUT_SIZE - 1;
  return log2_lut1024_lin[idx] / LUT_SCALE;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Пресмятане на вероятности
  for (int i = 0; i < HIST_BINS; i++) {
    probabilities[i] = (float)hist[i] / total_samples;
  }

  // --- 1. Класическа ентропия ---
  unsigned long t1 = micros(); // старт време
  float H_classic = 0;
  for (int i = 0; i < HIST_BINS; i++) {
    float p = probabilities[i];
    if (p > 0) {
      H_classic -= p * (log(p) / log(2.0)); // log2(p)
    }
  }
  unsigned long t2 = micros(); // край време
  unsigned long classic_time = t2 - t1;

  // --- 2. Ентропия с LUT ---
  unsigned long average_time_e = 0;
  unsigned long average_time_l = 0;
  
  float H_lut = 0;
  Serial.println("Idx\tp\t\tlog2(p)_classic\tlog2(p)_LUT\tDiff\t\t-p*log2(p)_classic\t-p*log2(p)_LUT");
  for (int i = 0; i < HIST_BINS; i++) {
    float p = probabilities[i];
    if (p > 0) {
      t1 = micros(); // старт време за LUT
      float log2p_classic = log(p) / log(2.0);
      t2 = micros(); // край време за LUT
      average_time_e = average_time_e + t2 - t1;
      
      // float log2p_lut = lut_log2_select_linear(p);//lut_log2_select(p);
      t1 = micros(); // старт време за LUT
      float log2p_lut = lut_log2_select_linear2(p);//lut_log2_select(p);
      t2 = micros(); // край време за LUT
      average_time_l = average_time_l + t2 - t1;
      
      float diff = log2p_classic - log2p_lut;
      float term_classic = -p * log2p_classic;
      float term_lut     = -p * log2p_lut;
      H_lut += term_lut;

      Serial.print(i); Serial.print("\t");
      Serial.print(p, 6); Serial.print("\t");
      Serial.print(log2p_classic, 6); Serial.print("\t");
      Serial.print(log2p_lut, 6); Serial.print("\t");
      Serial.print(diff, 6); Serial.print("\t");
      Serial.print(term_classic, 6); Serial.print("\t");
      Serial.println(term_lut, 6);
    }
  }
  average_time_e = average_time_e/256;
  average_time_l = average_time_l/256;

  Serial.print("\nShannon entropy (classic): ");
  Serial.println(H_classic, 6);
  Serial.print("Shannon entropy (LUT, "); Serial.print(LUT_SIZE);
  Serial.print(" points): ");
  Serial.println(H_lut, 6);
  Serial.print("Abs difference:            ");
  Serial.println(fabs(H_classic - H_lut), 6);

  // --- Време за изчисление ---
  Serial.print("Classic entropy time (us): "); Serial.println(average_time_e);
  Serial.print("LUT entropy time (us):     "); Serial.println(average_time_l);
}

void loop() {
  // нищо не правим тук
}
