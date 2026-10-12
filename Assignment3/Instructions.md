Lab: page tables
In this lab you will explore page tables and modify them to implement common OS features.

Before you start coding, read Chapter 3 of the xv6 book, and related files:

kernel/memlayout.h, which captures the layout of memory.
kernel/vm.c, which contains most virtual memory (VM) code.
kernel/kalloc.c, which contains code for allocating and freeing physical memory.
Consult RISC-V privileged instructions for how RISC-V defines page tables.
To start the lab, switch to the pgtbl branch:

  $ git fetch
  $ git checkout pgtbl
  $ make clean
  
Inspect a user-process page table (easy)
To help you understand RISC-V page tables, your first task is to explain the page table for a user process.

Run make qemu and run the user program pgtbltest. The print_pgtbl function prints out the page-table entries for the first 10 and last 10 pages of the pgtbltest process using the pgpte system call that we added to xv6 for this lab. The output looks as follows:

va 0 pte 0x21FCF45B pa 0x87F3D000 perm 0x5B
va 1000 pte 0x21FCE85B pa 0x87F3A000 perm 0x5B
...
va 0xFFFFD000 pte 0x0 pa 0x0 perm 0x0
va 0xFFFFE000 pte 0x21FD80C7 pa 0x87F60000 perm 0xC7
va 0xFFFFF000 pte 0x20001C4B pa 0x80007000 perm 0x4B
 
For every page table entry in the print_pgtbl output, explain what it logically contains and what its permission bits are. Figures 3.2 and 3.4 in the xv6 book might be helpful, although Figure 3.4 might have a slightly different set of pages than the process that's being inspected here. Note that xv6 doesn't place the virtual pages consecutively in physical memory.
Walk and print a two-level page table (easy)
To help you visualize RISC-V page tables, and perhaps to aid future debugging, your next task is to write a function that walks the two-level page table and prints it.

We added a system call vmprint(), which calls vmprint() in vm.c. It takes a pagetable_t argument (the page table of the process that made the system call), and your job is to print that pagetable in the format described below.
When you run pgtbltest, vmprint_test() calls your vmprint(), which might print something along the following lines:

page table 0x0000000087f22000
 ..0x0000000000000000: pte 0x0000000021fc7801 pa 0x0000000087f1e000 
 .. ..0x0000000000000000: pte 0x0000000021fc7401 pa 0x0000000087f1d000 
 .. .. ..0x0000000000000000: pte 0x0000000021fc7c5b pa 0x0000000087f1f000 RXU
 .. .. ..0x0000000000001000: pte 0x0000000021fc705b pa 0x0000000087f1c000 RXU
 .. .. ..0x0000000000002000: pte 0x0000000021fc6cd7 pa 0x0000000087f1b000 RWU
 .. .. ..0x0000000000003000: pte 0x0000000021fc6807 pa 0x0000000087f1a000 RW
 .. .. ..0x0000000000004000: pte 0x0000000021fc64d7 pa 0x0000000087f19000 RWU
 ..0x0000003fc0000000: pte 0x0000000021fc8401 pa 0x0000000087f21000 
 .. ..0x0000003fffe00000: pte 0x0000000021fc8001 pa 0x0000000087f20000 
 .. .. ..0x0000003fffffe000: pte 0x0000000021fd00c7 pa 0x0000000087f40000 RW
 .. .. ..0x0000003ffffff000: pte 0x0000000020001c4b pa 0x0000000080007000 RX
  
The output might be slightly different for you (e.g., one text page with RXU permissions instead of two pages).

The first line displays the argument to vmprint. After that there is a line for each PTE, including PTEs that refer to page-table pages deeper in the tree. Each PTE line is indented by a number of " .." that indicates its depth in the tree. Each PTE line shows its virtual address, the pte bits, the physical address extracted from the PTE, and the permission bits in the PTE: R(ead), W(rite), eXecute, and U(ser). Don't print PTEs that are not valid. In the above example, the top-level page-table page has mappings for entries 0 and 255. The next level down for entry 0 has only index 0 mapped, and the bottom-level for that index 0 has a few entries mapped.

Your code might emit different physical addresses than those shown above. The number of entries and the virtual addresses should be the same.

Some hints:

Use the macros at the end of the file kernel/riscv.h.
The function freewalk may be inspirational.
Use %p in your printk calls to print out full 64-bit hex PTEs and addresses as shown in the example.
For every leaf page in the vmprint output, explain what it logically contains and what the permission bits allow and disallow, and how it relates to the output of the earlier print_pgtbl() exercise above. Again, figure 3.4 in the xv6 book might be helpful.
Detect which pages have been accessed (easy)
Some applications (e.g., garbage collectors) can benefit from information about which pages have been accessed (read or write). In this part of the lab, you will add a new feature to xv6 that detects and reports this information to userspace by inspecting the access bits in the RISC-V page table. The RISC-V hardware page walker marks these bits in the PTE whenever it resolves a TLB miss.

Your job is to implement pgaccess(), a system call that reports which pages have been accessed. The system call takes three arguments. First, it takes the starting virtual address of the first user page to check. Second, it takes the number of pages to check. Finally, it takes a user address to a buffer to store the results into a bitmask (a data structure that uses one bit per page and where the first page corresponds to the least significant bit). Your implementation should handle invalid arguments to pgaccess(); for example, pgaccess should return an -1 error for a check on a page that isn't mapped or for an invalid buffer address. You will receive full credit for this part of the lab if the pgaccess test case passes when running pgtbltest.
Some hints:

Read pgaccess_test() in user/pgtbltest.c to see how pgaccess is used.
Start by implementing sys_pgaccess() in kernel/sysproc.c.
You'll need to parse arguments using argaddr() and argint().
For the output bitmask, it's easier to store a temporary buffer in the kernel and copy it to the user (via copyout()) after filling it with the right bits.
It's okay to set an upper limit on the number of pages that can be scanned.
walk() in kernel/vm.c is very useful for finding the right PTEs.
You'll need to define PTE_A, the access bit, in kernel/riscv.h. Consult RISC-V privileged instructions to determine its value
Be sure to clear PTE_A after checking if it is set. Otherwise, it won't be possible to determine if the page was accessed since the last time pgaccess() was called (i.e., the bit will be set forever).
vmprint() may come in handy to debug page tables.
Speed up the getpid system call (easy)
Some operating systems (e.g., Linux) speed up certain system calls by sharing data in a read-only region between userspace and the kernel. This eliminates the need for kernel crossings when performing these system calls. To help you learn how to insert mappings into a page table, your task is to implement this optimization for the getpid() system call in xv6.

When each process is created, map one read-only page at USYSCALL (a virtual address defined in memlayout.h). At the start of this page, store a struct usyscall (also defined in memlayout.h), and initialize it to store the PID of the current process. For this lab, ugetpid() has been provided on the userspace side and will automatically use the USYSCALL mapping. You will receive full credit for this part of the lab if the ugetpid test case passes when running pgtbltest.
Some hints:

Choose permission bits that allow userspace to only read the page.
There are a few things that need to be done over the lifecycle of a new page. For inspiration, understand the trapframe handling in kernel/proc.c.
Which other xv6 system call(s) could be made faster using this shared page? Explain how.
Use superpages (easy)
The RISC-V paging hardware supports two-megabyte pages as well as ordinary 4096-byte pages. The general idea of larger pages is called superpages, and (since RISC-V supports more than one size) 2M pages are called megapages. The operating system creates a superpage by setting the PTE_V and PTE_R bits in the level-1 PTE, and setting the physical page number to point to the start of a two-megabyte region of physical memory. This physical address must be two-megabyte-aligned (i.e., a multiple of two megabytes). You can read about this in the RISC-V privileged architecture manual by searching for megapage and superpage. Use of superpages decreases the amount of physical memory used by the page table, and can decrease misses in the TLB cache. For some programs this leads to large increases in performance.
Your job is to modify the xv6 kernel to use superpages for the kernel page table. The kernel uses kvmmake() to build a page table for itself by inserting many mappings using kvmmap. If a mapping includes one or more areas that are two-megabyte-aligned and at least two megabytes in size you should use a single superpage (instead of many bottom-level pages). You will receive full credit for this part of the lab if ksuper_test passes when running pgtbltest.
Some hints:

Make a copy of mappages that only kvmmap uses so that your copy of mappages can be specialized for the kernel by using superpages for kernel mappings (but not for user programs).
Change your vmprint() to indicate which PTEs are a superpage
How many superpage PTEs does your solution use? How many bottom-level pages do these superpage PTEs save?
Real operating systems dynamically promote a collection of pages to a superpage for the kernel and user processes. The following reference explains why that is a good idea and what is hard in a more serious design: Juan Navarro, Sitaram Iyer, Peter Druschel, and Alan Cox. Practical, transparent operating system support for superpages. SIGOPS Oper. Syst. Rev., 36(SI):89-104, December 2002. This reference summarizes superpage-implementations for different OSes: A comprehensive analysis of superpage management mechanism and policies.


Submit the lab
Time spent
Create a new file, time.txt, and put in a single integer, the number of hours you spent on the lab. git add and git commit the file.

Answers
If this lab had questions, write up your answers in answers-*.txt. git add and git commit these files.

Submit
You will turn in your assignments using Google Classroom.
When you're ready to submit, run make zipball, which will generate lab.zip. Upload this zip file to the corresponding Google Classroom assignment.

If you run make zipball and you have either uncomitted changes or untracked files, you will see output similar to the following:

 M hello.c
?? bar.c
?? foo.pyc
Untracked files will not be handed in.  Continue? [y/N]
Inspect the above lines and make sure all files that your lab solution needs are tracked, i.e., not listed in a line that begins with ??. You can cause git to track a new file that you create using git add {filename}.

Please run make grade to ensure that your code passes all of the tests.
Commit any modified source code before running make zipball.
Optional challenge exercises
Implement some ideas from the paper referenced above to make your super-page design more real.
Unmap the first page of a user process so that dereferencing a null pointer will result in a fault. You will have to change user.ld to start the user text segment at, for example, 4096, instead of 0.
Add a system call that reports dirty pages (modified pages) using PTE_D.
