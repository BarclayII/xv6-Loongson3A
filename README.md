# xv6-Loongson3A

**This repository is superseded by [AIMv6](https://github.com/BarclayII/AIMv6).**

Well, I intended to port xv6 onto Loongson 3A (or MIPS64r2) first, but I soon found out that xv6 is too... primitive, yeah.  It simply does not satisfy current
operating system research standard.

So, basically, I'm trying my best to write a robust operating system for educational use, introducing some more operating system concepts by the way.

Writing an OS is hard, but not *that* hard.

Hardware: [Loongson Multi Techboard](http://www.loongson.cn/multi_techboard.php).  It supports CC-NUMA, but currently I'm only going to implement a uniprocessor
OS first, introducing SMP after that, and I'm not considering having CC-NUMA implemented.  Maybe I'll add CC-NUMA some day...  Besides, I fail to start up
the board in CC-NUMA mode, and I have no idea why it fails.

CPU: Loongson 3A, MIPS64r2 instruction set, cache coherency maintained by hardware, no hazards, no cache aliasing for (slightly) larger pages, really great for a MIPS.

BIOS: PMON2000, with caches and TLB initialization already done.

GCC: 4.4.0 from [Loongnix website](http://www.loongnix.com:8000/dev/ftp/toolchain/gcc/release/CROSS_COMPILE/loongson3-gcc4.4.tar.gz)

### Roadmap
1. Trap handling (completed)
  - Nested trap handling
2. Memory management (completed)
  - Physical page allocation
  - Virtual memory management
  - SLAB allocation
3. Process & thread management
  - Context switches (mostly completed)
  - `fork(2)`, `exit(2)`, `waitpid(2)`, `kill(2)`, `yield()`
  - O(1) scheduler (mostly completed)
  - `clone()` (not planned)
4. Concurrency maintenance
  - Spinlocks
  - (POSIX) Semaphores
5. File system
  - RAM disk
  - `tmpfs`
  - *Berkeley FFS* (not planned)
6. Multicore support
  - Inter-process interrupts
7. *Signal handling*
8. *Dynamic loading* (not planned)

