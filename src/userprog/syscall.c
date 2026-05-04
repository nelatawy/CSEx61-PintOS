#include "userprog/syscall.h"
#include <stdio.h>
#include <stdbool.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"

typedef int pid_t;

static void syscall_handler (struct intr_frame *);

static void halt(void);
static void exit(int status);
static pid_t exec(const char *cmd_line);
static int wait(pid_t pid);
static bool create(const char *file, unsigned initial_size);
static bool remove(const char *file);
static int open(const char *file);
static int filesize(int fd);
static int read(int fd, void *buffer, unsigned size);
static int write(int fd, const void *buffer, unsigned size);
static void seek(int fd, unsigned position);
static unsigned tell(int fd);
static void close(int fd);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/* Get the next argument of the given type and 
	 store it in a newly defined variable of the 
	 given name */
#define GET_ARGUMENT(type, name)\
	f->esp += sizeof(type);\
	type name = *(type *)f->esp;

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	/* Get the system call number */
	int systcall_number = *(int *)f->esp;

	switch (systcall_number) {
		case SYS_HALT: {
			halt();
		} break;
		case SYS_EXIT: {
			/* Get the arguments from the stack */
			GET_ARGUMENT(int, status);

			/* Execute the system call */
			exit(status);
		} break;
		case SYS_EXEC: {
			/* Get the arguments from the stack */
			GET_ARGUMENT(const char *, cmd_line);

			/* Execute the system call */
			pid_t pid = exec(cmd_line);
		} break;
		case SYS_WAIT: {
			/* Get the arguments from the stack */
			GET_ARGUMENT(pid_t, pid);

			/* Execute the system call */
			int status = wait(pid);

			/* Set the return value */
			f->eax = status;
		} break;
		case SYS_CREATE: {
			/* Get the arguments from the stack */
			GET_ARGUMENT(const char *, file);
			GET_ARGUMENT(unsigned, initial_size);

			/* Execute the system call */
			bool success = create(file, initial_size);

			/* Set the return value */
			f->eax = success;
		} break;
		case SYS_REMOVE: {
			/* Get the arguments from the stack */
			GET_ARGUMENT(const char *, file);

			/* Execute the system call */
			bool success = remove(file);

			/* Set the return value */
			f->eax = success;
		} break;
		case SYS_OPEN: {
			/* Get the arguments from the stack */
			GET_ARGUMENT(const char *, file);

			/* Execute the system call */
			int fd = open(file);

			/* Set the return value */
			f->eax = fd;
		} break;
		case SYS_FILESIZE: {
			/* Get the arguments from the stack */
			GET_ARGUMENT(int, fd);

			/* Execute the system call */
			int size = filesize(fd);

			/* Set the return value */
			f->eax = size;
		} break;
		case SYS_READ: {
			/* Get the arguments from the stack */
			GET_ARGUMENT(int, fd);
			GET_ARGUMENT(void *, buffer);
			GET_ARGUMENT(unsigned, size);

			/* Execute the system call */
			int bytes_read = read(fd, buffer, size);

			/* Set the return value */
			f->eax = bytes_read;
		} break;
		case SYS_WRITE: {
			/* Get the arguments from the stack */
			GET_ARGUMENT(int, fd);
			GET_ARGUMENT(const void *, buffer);
			GET_ARGUMENT(unsigned, size);

			f->esp += sizeof(unsigned);
			unsigned size = *(unsigned *)f->esp;

			/* Execute the system call */
			int bytes_written = write(fd, buffer, size);

			/* Set the return value */
			f->eax = bytes_written;
		} break;
		case SYS_SEEK: {
			/* Get the arguments from the stack */
			GET_ARGUMENT(int, fd);
			GET_ARGUMENT(unsigned, position);

			/* Execute the system call */
			seek(fd, position);
		} break;
		case SYS_TELL: {
			/* Get the arguments from the stack */
			GET_ARGUMENT(int, fd);

			/* Execute the system call */
			unsigned position = tell(fd);

			/* Set the return value */
			f->eax = position;
		} break;
		case SYS_CLOSE: {
			/* Get the arguments from the stack */
			GET_ARGUMENT(int, fd);

			int fd = *(int *)f->esp;

			/* Execute the system call */
			close(fd);
		} break;
		default: {
			printf ("Unknown system call: %d\n", systcall_number);
			thread_exit ();
		}
	}
}

static void 
halt(void)
{
	shutdown_power_off();
}

static void 
exit(int status)
{
	struct thread* current_thread = thread_current();
	printf("%s: exit(%d)\n", current_thread->name, status);

    struct list_elem *e;
    while (!list_empty (&current_thread->fd_table))
    {
        e = list_pop_front (&current_thread->fd_table);
        struct fd_entry *entry = list_entry (e, struct fd_entry, elem);
        file_close (entry->file);
        free (entry);
    }

	if (current_thread->executable != NULL)
	{
		file_allow_write (current_thread->executable);
		file_close (current_thread->executable);
	}

	thread_exit();
}

static pid_t
exec(const char *cmd_line)
{
	return -1;
}

static int 
wait(pid_t pid)
{
	return -1;
}

static bool 
create(const char *file, unsigned initial_size)
{
	return false;
}

static bool 
remove(const char *file)
{
	return false;
}

static int 
open(const char *file)
{
	//// TODO: add the file to the process FD table
	//// NOTE: FD table is in struct thread in "threads.h"
	return -1;
}

static int 
filesize(int fd)
{
	return 0;
}

static int 
read(int fd, void *buffer, unsigned size)
{
	return 0;
}

static int 
write(int fd, const void *buffer, unsigned size)
{
	return 0;
}

static void 
seek(int fd, unsigned position)
{

}

static unsigned 
tell(int fd)
{
	return 0;
}

static void 
close(int fd)
{
	//// TODO: remove the file from the process FD table
	//// NOTE: FD table is in struct thread in "threads.h"
}
