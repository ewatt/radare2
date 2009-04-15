/* radare - LGPL - Copyright 2009 pancake<nopcode.org> */

#include <r_debug.h>
#include <r_lib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

static int r_debug_ptrace_step(int pid)
{
	//u32 addr = 0; /* should be eip */
	//u32 data = 0;
	printf("NATIVE STEP over PID=%d\n", pid);
	ptrace(PTRACE_SINGLESTEP, pid, 0, 0); //addr, data);
	perror("ptrace-singlestep");
	return R_TRUE;
}

static int r_debug_ptrace_attach(int pid)
{
	u32 addr;
	u32 data;
	int ret = ptrace(PTRACE_ATTACH, pid, addr, data);
	return (ret != -1)?R_TRUE:R_FALSE;
}

static int r_debug_ptrace_detach(int pid)
{
	u32 addr;
	u32 data;
	return ptrace(PTRACE_DETACH, pid, addr, data);
}

static int r_debug_ptrace_continue(int pid)
{
	u32 addr;
	u32 data;
	return ptrace(PTRACE_CONT, pid, addr, data);
}

static int r_debug_ptrace_wait(int pid)
{
	int ret, status = -1;
	printf("prewait\n");
	ret = waitpid(pid, &status, 0);
	printf("status=%d (return=%d)\n", status, ret);
	return status;
}

struct r_debug_regset_t * r_debug_ptrace_reg_read(int pid)
{
	struct r_debug_regset *r = NULL;
#if __linux__
#include <sys/user.h>
#include <limits.h>
	struct user_regs_struct regs;
	memset(&regs,0, sizeof(regs));
	ptrace(PTRACE_GETREGS, pid, 0, &regs);
#if __WORDSIZE == 64
	r = r_debug_regset_new(17);
	r_debug_regset_set(r, 0, "rax", regs.rax);
	r_debug_regset_set(r, 1, "rbx", regs.rbx);
	r_debug_regset_set(r, 2, "rcx", regs.rcx);
	r_debug_regset_set(r, 3, "rdx", regs.rdx);
	r_debug_regset_set(r, 4, "rsi", regs.rsi);
	r_debug_regset_set(r, 5, "rdi", regs.rdi);
	r_debug_regset_set(r, 6, "rsp", regs.rsp);
	r_debug_regset_set(r, 7, "rbp", regs.rbp);
	r_debug_regset_set(r, 8, "rip", regs.rip);
	r_debug_regset_set(r, 9, "r8", regs.r8);
	r_debug_regset_set(r, 10, "r9", regs.r9);
	r_debug_regset_set(r, 11, "r10", regs.r10);
	r_debug_regset_set(r, 12, "r11", regs.r11);
	r_debug_regset_set(r, 13, "r12", regs.r12);
	r_debug_regset_set(r, 14, "r13", regs.r13);
	r_debug_regset_set(r, 15, "r14", regs.r14);
	r_debug_regset_set(r, 16, "r15", regs.r15);
#else
	/* TODO: use enum for 0, 1, 2... ? */
	/* TODO: missing eflags here */
	r = r_debug_regset_new(9);
	r_debug_regset_set(r, 0, "eax", regs.eax);
	r_debug_regset_set(r, 1, "ebx", regs.ebx);
	r_debug_regset_set(r, 2, "ecx", regs.ecx);
	r_debug_regset_set(r, 3, "edx", regs.edx);
	r_debug_regset_set(r, 4, "esi", regs.esi);
	r_debug_regset_set(r, 5, "edi", regs.edi);
	r_debug_regset_set(r, 6, "esp", regs.esp);
	r_debug_regset_set(r, 7, "ebp", regs.ebp);
	r_debug_regset_set(r, 8, "eip", regs.eip);
#endif
#endif /* linux */
	return r;
}

int r_debug_ptrace_reg_write(int pid, struct r_debug_regset_t *regs)
{
	/* TODO */
}

static int r_debug_ptrace_bp_write(int pid, u64 addr, int hw, int type)
{
	return R_TRUE;
}

static int r_debug_ptrace_bp_read(int pid, u64 addr, int hw, int type)
{
	return R_TRUE;
}

#if 0
static int r_debug_ptrace_import(struct r_debug_handle_t *from)
{
	//int pid = from->export(R_DEBUG_GET_PID);
	//int maps = from->export(R_DEBUG_GET_MAPS);
	return R_FALSE;
}
#endif

static struct r_debug_handle_t r_dbg_plugin_ptrace = {
	.name = "ptrace",
#if __WORDSIZE == 64
	.archs = { "x86-64", 0 },
#else
	.archs = { "x86", 0 },
#endif
	.step = &r_debug_ptrace_step,
	.cont = &r_debug_ptrace_continue,
	.attach = &r_debug_ptrace_attach,
	.detach = &r_debug_ptrace_detach,
	.wait = &r_debug_ptrace_wait,
	.bp_write = &r_debug_ptrace_bp_write,
	.reg_read = &r_debug_ptrace_reg_read,
	.reg_write = &r_debug_ptrace_reg_write,
	//.bp_read = &r_debug_ptrace_bp_read,
};

struct r_lib_struct_t radare_plugin = {
	.type = R_LIB_TYPE_DBG,
	.data = &r_dbg_plugin_ptrace
};
