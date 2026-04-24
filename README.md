## Scientific Background
This project implements the algorithms described in the paper: 
[Download PDF Theory](./CORDIC_BRIC/Paper_BG_AI-4-2025.pdf)

Ето обновеното и допълнено резюме (Abstract) на английски език. В него е добавена информацията, че в текущата версия на разработката синусът се изчислява чрез CORDIC, а логаритъмът се базира на Look-Up Tables (LUT).
Abstract

Title: Bit Manipulations and Hardware Optimizations for Fast Transcendental Functions in Edge and IoT Devices

Author: Georgi Petrov, PhD

Affiliation: Department of Telecommunications, New Bulgarian University

Abstract:
Fast and energy-efficient evaluation of transcendental functions—such as logarithms, exponentials, square roots, and trigonometric operations—is critical for real-time signal processing and on-device AI inference in Edge and IoT systems. This paper surveys and experimentally validates a set of lightweight techniques, primarily IEEE-754 based bit-level manipulations (bit tricks), alongside code- and hardware-level optimizations that significantly accelerate these functions on resource-constrained microcontrollers.

In the current implementation, a hybrid approach is employed: sine functions are evaluated using the CORDIC algorithm for balanced precision, while logarithmic functions are optimized via Look-Up Tables (LUT) to ensure minimal latency. The study also describes effective bit hacks for fast approximations of 2x, x​, and 1/x​ (including the "Quake" inverse square root), explaining their theoretical basis in floating-point representation. Furthermore, it demonstrates how modest corrections to the mantissa (linear and low-order polynomial) can substantially improve accuracy while retaining performance gains.

Benchmarking across representative platforms—including AVR, ARM Cortex-M variants, ESP32, and RISC-V—demonstrates up to a 10× speedup relative to standard library implementations. The research discusses trade-offs between latency, memory footprint, and numerical fidelity, providing guidelines for safe deployment in production Edge/IoT projects such as embedded machine learning, entropy computation (Shannon-Fisher), and DSP normalization.

Keywords: Bit manipulations, hardware optimizations, CORDIC, Look-Up Tables (LUT), Edge devices, IoT, fast inverse square root, embedded machine learning, IEEE-754.
