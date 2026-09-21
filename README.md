# Freeuni-Operating-System-Engineering
This repository contains my solutions, kernel extensions, and system call implementations for the **xv6-riscv** operating system as part of the **Operating Systems Engineering** course at the Free University of Tbilisi.

---

## 📌 Course Overview
- **Institution:** Free University of Tbilisi (MACI / Computer Science)
- **Course:** Operating Systems Engineering (V Semester)
- **Primary System:** xv6 (MIT 6.S081 / 6.1810 curriculum base)
- **Language:** C / RISC-V Assembly

---

## 🛠️ Key Topics & Implementations

### 1. System Calls & User Utilities
- Implemented core user-level utilities in C (`sleep`, `find`, `trace`, `memdump`).
- Extended the kernel system call table, argument passing, and user/kernel memory pointers.

### 2. Page Tables & Virtual Memory
- Handled virtual memory mappings, page table walks, and page table allocation/deallocation.
- Implemented user virtual memory isolation, kernel page maps, and lazy allocation mechanisms.

### 3. Traps, Interrupts & Drivers
- Managed user/kernel mode transitions, trap frames, and system call entry/exit vectors.
- Configured device interrupts, PLIC handling, UART console I/O, and kernel drivers.

### 4. Multiprocessing & Concurrency
- Mastered synchronization primitives: spinlocks, sleep locks, and atomic operations.
- Avoided concurrency pitfalls such as deadlocks, race conditions, and priority inversion.

### 5. Process Scheduling & Coordination
- Handled process state transitions, context switching (`swtch.S`), and round-robin scheduling.
- Implemented process synchronization primitives (`sleep`, `wakeup`, condition variables).

### 6. File Systems & Crash Recovery
- Configured layer abstraction for block caching (`bio.c`), inodes, and system file handling.
- Implemented logging mechanisms for atomic transaction execution and crash resiliency.
