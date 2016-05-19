/* API for the TODO system calls */
#ifndef _TODO_API_H
#define _TODO_API_H

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
int add_TODO (pid_t pid, const char *TODO_description, ssize_t description_size)
{
	int res;
	__asm__
	(
		"pushl %%eax;"
		"pushl %%ebx;"
		"pushl %%ecx;"
		"pushl %%edx;"
		"movl $243, %%eax;"
		"movl %1, %%ebx;"
		"movl %2, %%ecx;"
		"movl %3, %%edx;"
		"int $0x80;"
		"movl %%eax, %0;"
		"popl %%edx;"
		"popl %%ecx;"
		"popl %%ebx;"
		"popl %%eax;"
		:"=m" (res)
		:"m" (pid), "m" (TODO_description), "m" (description_size)
	);
	if (res) {
		errno = res;
		res = -1;
	}
	//printf("add wrapper debug return: res=%d, pid=%d, description = %s, size = %d\n",res,pid,TODO_description,description_size);
	return (int)res;
}

ssize_t read_TODO (pid_t pid, int TODO_index, char *TODO_description, ssize_t description_size, int* status)
{
	int res;
	__asm__
	(
		"pushl %%eax;"
		"pushl %%ebx;"
		"pushl %%ecx;"
		"pushl %%edx;"
		"pushl %%esi;"
		"pushl %%edi;"
		"movl $244, %%eax;"
		"movl %1, %%ebx;"
		"movl %2, %%ecx;"
		"movl %3, %%edx;"
		"movl %4, %%esi;"
		"movl %5, %%edi;"
		"int $0x80;"
		"movl %%eax, %0;"
		"popl %%edi;"
		"popl %%esi;"
		"popl %%edx;"
		"popl %%ecx;"
		"popl %%ebx;"
		"popl %%eax;"
		:"=m" (res)
		:"m" (pid), "m" (TODO_index), "m" (TODO_description), "m" (description_size), "m" (status)
	);
	if (res < 0) {
		errno = -res;
		res = -1;
	}
	//printf("read wrapper debug return: res=%d, pid=%d, index = %d, description = %s, size = %d, status = %d\n",res,pid,TODO_index, TODO_description,description_size,*status);
	return (ssize_t)res;
}

int mark_TODO (pid_t pid, int TODO_index, int status)
{
	int res;
	__asm__
	(
		"pushl %%eax;"
		"pushl %%ebx;"
		"pushl %%ecx;"
		"pushl %%edx;"
		"movl $245, %%eax;"
		"movl %1, %%ebx;"
		"movl %2, %%ecx;"
		"movl %3, %%edx;"
		"int $0x80;"
		"movl %%eax, %0;"
		"popl %%edx;"
		"popl %%ecx;"
		"popl %%ebx;"
		"popl %%eax;"
		:"=m" (res)
		:"m" (pid), "m" (TODO_index), "m" (status)
	);
	if (res) {
		errno = res;
		res = -1;
	}
	//printf("mark wrapper debug return: res=%d, pid=%d, TODO_index=%d, status=%d\n",res,pid,TODO_index,status);
	return (int)res;
}

int delete_TODO (pid_t pid, int TODO_index)
{
	int res;
	__asm__
	(
		"pushl %%eax;"
		"pushl %%ebx;"
		"pushl %%ecx;"
		"movl $246, %%eax;"
		"movl %1, %%ebx;"
		"movl %2, %%ecx;"
		"int $0x80;"
		"movl %%eax, %0;"
		"popl %%ecx;"
		"popl %%ebx;"
		"popl %%eax;"
		:"=m" (res)
		:"m" (pid), "m" (TODO_index)
	);
	if (res) {
		errno = res;
		res = -1;
	}
	//printf("delete wrapper debug return: res=%d, pid=%d, TODO_index=%d\n",res,pid,TODO_index);
	return (int)res;
}

#endif //_TODO_API_H
