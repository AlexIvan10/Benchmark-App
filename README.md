# Programming Languages Performance Benchmark

This project aims to measure and compare the execution time of various operations across three programming languages: C, C++, and Java. By evaluating these processes, the project provides insights into the performance characteristics of each language, helping developers make informed decisions when selecting the best language for specific scenarios.

## Table of Contents
- **Objectives**
- **Technologies Used**
- **Measured Processes**
- **System Specifications**
- **Interpreting Results**

---

## Objectives

- Measure the execution time of key operations in C, C++, and Java.
- Provide a user-friendly interface to select operations and view results.
- Display execution times in nanoseconds and generate comparative performance charts.
- Save detailed results in a text file for further analysis.

## Technologies Used

- **C** (time.h, pthreads for threading)
- **C++** (chrono, std::thread for threading)
- **Java** (System.nanoTime(), Thread API)
- **Python** (Tkinter for GUI)

## Measured Processes

- **Memory Allocation**: Measures the time to allocate memory either statically or dynamically.
- **Memory Access**: Evaluates the time taken to access elements in allocated memory.
- **Thread Creation**: Measures the time to create new threads.
- **Thread Context Switch**: Evaluates the time required to switch control between threads.
- **Thread Migration**: Measures the time for a thread to migrate between CPU cores (C and C++).

---

## System Specifications

- OS: Windows

---

## Interpreting Results

- Graphs illustrate the performance of each operation across the three languages.
- **X-axis**: Programming Languages (C, C++, Java)
- **Y-axis**: Execution Time (Nanoseconds)
- Results are saved automatically in benchmark_results.txt

---
