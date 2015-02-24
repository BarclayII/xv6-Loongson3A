# xv6-Loongson3A
Well, I intended to port xv6 onto Loongson 3A (or MIPS64r2) first, but I soon found out that xv6 is too... primitive, yeah.  It simply does not satisfy current
operating system research standard.

So, basically, I'm trying my best to write a robust operating system for educational use, introducing some more operating system concepts by the way.

Writing an OS is hard, but not *that* hard.

Hardware: [Loongson Multi Techboard](http://www.loongson.cn/multi_techboard.php).  It supports CC-NUMA, but currently I'm only going to implement a uniprocessor
OS first, introducing SMP after that, and I'm not considering having CC-NUMA implemented.  Maybe I'll add CC-NUMA some day...  Besides, I fail to start up
the board in CC-NUMA mode, and I have no idea why it fails.

CPU: Loongson 3A, MIPS64r2 instruction set, cache coherency maintained by hardware, no hazards, no cache aliasing, really great for a MIPS.

BIOS: PMON2000, with caches and TLB initialization already done.

GCC: 4.4.0 from [Loongnix website](http://www.loongnix.com:8000/dev/ftp/toolchain/gcc/release/CROSS_COMPILE/loongson3-gcc4.4.tar.gz)
