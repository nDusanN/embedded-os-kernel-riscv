# RISC-V Multithreaded Operating System Kernel

A lightweight operating system kernel implemented for the RISC-V architecture, designed to support multithreading, time sharing, and preemptive context switching in an embedded-like environment.

The kernel is implemented as a library-style operating system, where both the application and the kernel share the same address space and are statically linked into a single executable. This design is typical for embedded systems, where the OS and application are tightly coupled and run as one unified program.

---

## Overview

This project implements a minimal but functional operating system kernel with support for:

* Multithreading (user-level threads)
* Time-sharing scheduler
* Preemptive multitasking
* Context switching via timer interrupts
* Custom memory allocator
* Semaphore-based synchronization

The system is executed in a RISC-V emulator environment (QEMU).

---

## Key Features

### Threading System

* Thread creation and management
* Thread lifecycle control (create, exit, dispatch)
* Cooperative and preemptive scheduling
* Context switching on timer interrupts

### Scheduler

* Time-sharing based scheduling
* Preemptive thread switching using hardware timer interrupts
* Fair CPU time distribution across threads

### Memory Management

* Custom heap allocator
* Memory allocation and deallocation primitives
* No dependency on host OS memory APIs

### Synchronization

* Semaphore implementation
* Blocking and waking of threads
* Safe coordination between concurrent threads

---

## System Architecture

The kernel and user application are compiled into a single binary and share a common address space.

### Execution Model

* Kernel runs in privileged mode
* User code runs in user mode
* Timer interrupts trigger context switching
* Scheduler selects next runnable thread

---

## API Layers

The system is structured in three abstraction layers:

### 1. ABI Layer (Low-Level Interface)

Direct system call interface using software interrupts and CPU registers.

### 2. C API Layer

Procedural wrapper over the ABI layer.

### 3. C++ API Layer

Object-oriented abstraction built on top of the C API.

---

## Implemented System Calls

* `mem_alloc` / `mem_free` — dynamic memory management
* `thread_create` — create new thread
* `thread_exit` — terminate current thread
* `thread_dispatch` — voluntary context switch
* `sem_open` / `sem_close` — semaphore lifecycle
* `sem_wait` / `sem_signal` — synchronization primitives
* `time_sleep` — thread sleep mechanism

---

## Implementation Details

The kernel is implemented using a combination of:

* **C/C++** for high-level kernel logic (scheduler, memory management, synchronization)
* **Assembly language (RISC-V and x86 segments)** for low-level context switching, interrupt handling, and critical CPU state manipulation

This hybrid approach was used to handle architecture-specific operations that cannot be expressed efficiently in high-level languages.

---

## Concurrency Model

* Preemptive multitasking based on timer interrupts
* Cooperative yielding via explicit dispatch calls
* Thread scheduling based on time-sharing policy

---

## Memory Model

* Single shared address space
* Kernel and application coexist in the same memory space
* Custom heap allocator handles dynamic memory requests

---

## Interrupt Handling

* Timer interrupts are used for preemption and scheduling
* Keyboard interrupt handling is present as part of system infrastructure (not exposed via user-level I/O APIs in this implementation)

---

## Platform

* RISC-V architecture
* Executed in QEMU emulator
* No dependency on host operating system system calls
* Fully self-contained kernel + application binary

---

## Design Goals

This project focuses on demonstrating:

* Core operating system concepts
* Thread scheduling and concurrency
* Low-level system programming
* Interrupt-driven execution model
* Embedded-style OS design

---

## What This Project Demonstrates

This implementation shows understanding of:

* Operating system internals
* Preemptive scheduling
* Context switching mechanisms
* Memory allocation design
* Interrupt handling
* Hybrid C/C++ and assembly system design
* Cross-layer system architecture (ABI → C API → C++ API)

---

## Notes

This is a minimal research/educational kernel implementation, not a general-purpose operating system.
