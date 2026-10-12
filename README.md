# Freeuni Operating System Engineering

This repository contains my implementations, system call extensions and kernel modifications for the **xv6-riscv** operating system as part of the **Operating Systems Engineering** course at Free University of Tbilisi.

## 🛠️ Key Topics & Lab Implementations
- **System Calls & User Utilities:** Extending the kernel interface (`sleep`, `find`, `trace`, process introspection)
- **Page Tables & Virtual Memory:** Managing page directory walks, kernel/user page mapping, and virtual memory isolation
- **Traps & Interrupts:** User/kernel space transitions, hardware interrupts, and PLIC handling
- **Multiprocessing & Concurrency:** Spinlocks, sleep locks, and thread synchronization
- **Scheduling & Coordination:** Process states, context switching (`swtch.S`), and round-robin scheduling
- **File System & Crash Recovery:** Inode handling, logging mechanisms, and crash-resilient block buffering

## 🚀 Environment Setup
To build and run xv6 using the QEMU emulator:

```bash
# Build kernel and launch in QEMU
make qemu

# Exit QEMU
Ctrl+A then X
