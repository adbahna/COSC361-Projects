//printz - COSC361 given header file
//Spring 2017
//(c) Stephen Marz 2017
#pragma once

/**
 Variable arguments
 va_list       - The data type for variable argument list
 va_start(v,l) - Start a variable argument list
 va_end(v)     - End a variable argument list
 va_arg(v,l)   - Retrieve a variable argument from the list
*/
#define va_start(v,l)	__builtin_va_start(v,l)
#define va_end(v)	__builtin_va_end(v)
#define va_arg(v,l)	__builtin_va_arg(v,l)
#define va_copy(d,s)	__builtin_va_copy(d,s)
using va_list = __builtin_va_list;

/**
 Data types and sizes
*/
using pointer = unsigned long;
using uint64_t = unsigned long;
using int64_t = long;
using uint32_t = unsigned int;
using int32_t = int;
using uint16_t = unsigned short;
using int16_t = short;
using uint8_t = unsigned char;
using int8_t = char;
using size_t = uint64_t;

/**
  Printf equivalent. Prints format and associated
  variables to stdout using write from <unistd.h>.
  @fmt - The string/format to print to the screen
  @returns - The number of bytes printed to the screen.

  Supports %d, %f, and %x (decimal, float, and hex).
  All variables for %d, %f, and %x must be 64-bits!
*/
int printf(const char *fmt, ...);
/**
 Same as printf, except prints to the string dest.
 @dest - The destination string to print to, buffer must be big enough
         to store "size" or size of the "fmt".
 @fmt - The string/format to store into "dest"
 @returns - The number of bytes stored in "dest"

 Supports %d, %f, and %x (decimal, float, and hex).
 All variables for %d, %f, and %x must be 64-bits!
*/
int snprintf(char *dest, size_t size, const char *fmt, ...);


/**
 You do not need to use the heap to solve this problem. However,
 if you want to, you must not use any glibc function, such as malloc/free or new/delete.
 Instead, you must implement it using mmap, which is a kernel system call.

 You may NOT use malloc/free or new/delete.
 You must use raw system calls to call mmap and munmap
 which are call #s 9 and 11 (respectively)
*/
#define MAP_FAILED ((void*)-1)
#define MAP_PROTECTION 3
#define MAP_TYPE 34

#define SYSCALL_MMAP 9
#define SYSCALL_MUNMAP 11
#define SYSCALL_WRITE 1

/**
 System calls on Hydra:
 rax - System call number (SYSCALL_MMAP or SYSCALL_MUNMAP or SYSCALL_WRITE)

 write:
        rdi = file descriptor (1 = stdout, 2 = stderr)
	rsi = pointer to string
	rdx = size of string (# of bytes to write)
	Returns: rax = # of bytes written

 mmap:
 	rdi = offset address (should be 0)
	rsi = size of memory requested
	rdx = protection bits (should use MAP_PROTECTION)
	r10 = flags (should use MAP_TYPE)
	r8  = file descriptor (should be -1)
	r9  = file descriptor offset (should be 0)

 munmap:
	rdi = mapped address
	rsi = size of mapped memory

 Inline ASM:
 asm volatile("syscall"
              : "=a" (result) //puts rax into the variable result (outputs)
              : "a" (value_rax), "D" (value_rdi), "S" (value_rsi), "d" (value_rdx) // inputs
              : "memory", "rcx", "r11"  //tell ASM what will be clobbered
             );
*/

static inline int write(int fd, const char *output, int length) {
	int result;

	asm volatile("syscall"
              : "=a" (result)
              : "a" (SYSCALL_WRITE), "D" (fd), "S" (output), "d" (length)
              : "memory", "rcx", "r11"
             );

	return result;
}

