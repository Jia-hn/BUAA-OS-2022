// User-level IPC library routines

#include "lib.h"
#include <mmu.h>
#include <env.h>

extern struct Env *env;
unsigned long my_exception_handlers[30][30];

void *my_set_except_vector(int n, void *addr)
{
    unsigned long handler = (unsigned long)addr;
    unsigned long old_handler = my_exception_handlers[n];
    my_exception_handlers[n] = handler;
    return (void *)old_handler;
}

void signal(int sig, void (*handler)(int)) {
	int env_id = sys_getenvid();
	my_set_except_vector(ENVX(env_id), (void *)handler);
}

// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void
ipc_send(u_int whom, u_int val, u_int srcva, u_int perm)
{
	int r;

	while ((r = syscall_ipc_can_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		syscall_yield();
		//writef("QQ");
	}

	if (r == 0) {
		return;
	}

	user_panic("error in ipc_send: %d", r);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int
ipc_recv(u_int *whom, u_int dstva, u_int *perm)
{
	//printf("ipc_recv:come 0\n");
	syscall_ipc_recv(dstva);

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}

