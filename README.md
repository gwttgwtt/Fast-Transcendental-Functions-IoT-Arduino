## Scientific Background
This project implements the algorithms described in the paper: 
[Download PDF Theory](./CORDIC_BRIC/Paper_BG_AI-4-2025.pdf)

Title: Bit Manipulations and Hardware Optimizations for Fast Transcendental Functions in Edge and IoT Devices

Author: Georgi Petrov, PhD

Affiliation: Department of Telecommunications, New Bulgarian University

Abstract:
Fast and energy-efficient evaluation of transcendental functions—such as logarithms, exponentials, square roots, and trigonometric operations—is critical for real-time signal processing and on-device AI inference in Edge and IoT systems. This paper surveys and experimentally validates a set of lightweight techniques, primarily IEEE-754 based bit-level manipulations (bit tricks), alongside code- and hardware-level optimizations that significantly accelerate these functions on resource-constrained microcontrollers.

In the current implementation, a hybrid approach is employed: sine functions are evaluated using the CORDIC algorithm for balanced precision, while logarithmic functions are optimized via Look-Up Tables (LUT) to ensure minimal latency. The study also describes effective bit hacks for fast approximations of 2x, x​, and 1/x​ (including the "Quake" inverse square root), explaining their theoretical basis in floating-point representation. Furthermore, it demonstrates how modest corrections to the mantissa (linear and low-order polynomial) can substantially improve accuracy while retaining performance gains.

Benchmarking across representative platforms—including AVR, ARM Cortex-M variants, ESP32, and RISC-V—demonstrates up to a 10× speedup relative to standard library implementations. The research discusses trade-offs between latency, memory footprint, and numerical fidelity, providing guidelines for safe deployment in production Edge/IoT projects such as embedded machine learning, entropy computation (Shannon-Fisher), and DSP normalization.

Keywords: Bit manipulations, hardware optimizations, CORDIC, Look-Up Tables (LUT), Edge devices, IoT, fast inverse square root, embedded machine learning, IEEE-754.


Fast Shannon Entropy Calculation with LUT & Linear Interpolation 
[Download PDF Theory](./CORDIC_BRIC/LUT_LOG)

This project provides a high-performance implementation of Shannon Entropy calculation optimized for microcontrollers (Arduino, ESP32, STM32). It compares the standard floating-point approach (math.h) against a Look-Up Table (LUT) method with linear interpolation.
🚀 Performance Summary

Based on benchmark tests using an exponential distribution:
    Classic Method: 204 μs
    LUT Optimized Method: 133 μs
    Speed Improvement: ~35% faster with negligible precision loss.
🛠 Features
    Memory Efficient: Uses a 1024-point LUT stored in int16_t (Q8.8 fixed-point format) to save flash memory.
    Interpolation: Implements linear interpolation between LUT points to maintain high accuracy across the probability range.
    Comprehensive Datasets: Includes multiple header files with pre-calculated histograms to test different statistical distributions:
        hist_uniform.h (High entropy/randomness)
        hist_gaus.h (Normal distribution)
        hist_expo.h (Exponential decay - common in compressed data)
        hist_bimodal.h (Two-peak distribution)

📂 Project Structure
    LUT_LOG.ino: Main Arduino sketch containing the benchmark logic.
    log2_lut_linear.h: The 1024-entry lookup table for log2​(p).
    hist_*.h: Modular datasets for easy testing.

📖 Usage
To test different data distributions, simply change the include directive in the main file:
C++
#include "hist_expo.h"  // Change to "hist_gaus.h" or "hist_uniform.h"

Why use this?
In real-time embedded systems (like signal processing or cryptography), calculating logarithms is computationally expensive. This library allows you to estimate entropy in real-time with significantly lower CPU overhead while keeping the error margin (Abs Diff) as low as ~0.03.
