
#ifndef _ASM_SYSCALL_H
#define _ASM_SYSCALL_H

#define NRSYS_syscall	0	/* reserved */
#define NRSYS_fork	1	/* fork() */
#define NRSYS_execve	2	/* execve(filename, argv, envp) */
#define NRSYS_waitpid	3	/* waitpid(pid, status, options) */
#define NRSYS_kill	4	/* kill(pid, sig) */
#define NRSYS_read	5	/* read(fd, buf, len) */
#define NRSYS_write	6	/* write(fd, buf, len) */
#define NRSYS_yield	7	/* yield() */
#define NRSYS_exit	8	/* exit(code) */

#define NR_SYSCALLS	256	/* Number of system calls */


#endif
