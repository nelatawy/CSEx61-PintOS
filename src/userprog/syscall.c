#include "userprog/syscall.h"
#include <stdio.h>
#include <stdbool.h>
#include "threads/malloc.h"
#include <syscall-nr.h>
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "devices/shutdown.h"
#include "devices/input.h"

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

struct lock file_lock;


void
syscall_init (void) 
{
  lock_init(&file_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void exit_failure(void) {
	exit(-1);
}

/* Checks if the given pointer is a valid user pointer, and exits if not */
static void check_pointer(const void *pointer) {
	/* For a pointer to be valid, it must not be null, it must be in user space, and it must be mapped to a page */
	if (pointer == NULL || !is_user_vaddr(pointer) || pagedir_get_page(thread_current()->pagedir, pointer) == NULL) {
		exit_failure();
	}
}

static void check_buffer(const void *buffer, unsigned size) {
	for (unsigned i = 0; i < size; i++) {
		check_pointer(buffer + i);
	}
}

static void check_string(const char *string) {
	do {
		check_pointer(string);
	} while (*string++ != '\0');
}

/* Get the next argument of the given type and 
	 store it in a newly defined variable of the 
	 given name */
#define GET_ARGUMENT(type, name)\
	check_buffer(esp, sizeof(type));\
	type name = *(type *)esp;\
	esp += sizeof(type);

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	void *esp = f->esp;

	/* Get the system call number */
	GET_ARGUMENT(int, systcall_number);

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

			/* Check arguments */
			check_string(cmd_line);

			/* Execute the system call */
			f->eax = exec(cmd_line);
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

			/* Check arguments */
			check_string(file);

			/* Execute the system call */
			bool success = create(file, initial_size);

			/* Set the return value */
			f->eax = success;
		} break;
		case SYS_REMOVE: {
			/* Get the arguments from the stack */
			GET_ARGUMENT(const char *, file);

			/* Check arguments */
			check_string(file);

			/* Execute the system call */
			bool success = remove(file);

			/* Set the return value */
			f->eax = success;
		} break;
		case SYS_OPEN: {
			/* Get the arguments from the stack */
			GET_ARGUMENT(const char *, file);

			/* Check arguments */
			check_string(file);

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

			/* Check arguments */
			check_buffer(buffer, size);

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

			/* Check arguments */
			check_buffer(buffer, size);

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

			/* Execute the system call */
			close(fd);
		} break;
		default: {
			printf ("Unknown system call: %d\n", systcall_number);
			exit(-1);
		}
	}
}

static struct file*
get_file(int fd){
	struct list_elem* itr = list_begin(&thread_current()->fd_table);
	while (itr != list_end(&thread_current()->fd_table))
	{
		struct fd_entry* entry = list_entry(itr, struct fd_entry, elem);
		if (entry->fd == fd)
		{
			return entry->file;
		}
		itr = list_next(itr);
	}
	return NULL;
}

static void 
halt(void)
{
	shutdown_power_off();
}

static void 
exit(int status)
{
	struct thread* cur = thread_current();
	printf("%s: exit(%d)\n", cur->name, status);

	if (cur->self_ct != NULL)
	{
		cur->self_ct->exit_status = status;
		cur->self_ct->has_exited = true;
		sema_up(&cur->self_ct->exit_sema);
		if (cur->self_ct->orphaned)
			free(cur->self_ct);
	}

	thread_exit();
}

static pid_t
exec(const char *cmd_line)
{
tid_t tid = process_execute(cmd_line);

    return tid;

}

static int 
wait(pid_t pid)
{
	return process_wait(pid);
}

static bool 
create(const char *file, unsigned initial_size)
{	
	lock_acquire(&file_lock);
	bool success = filesys_create(file, initial_size);
	lock_release(&file_lock);

	return success;
}

static bool 
remove(const char *file)
{
	lock_acquire(&file_lock);
	bool success = filesys_remove(file);
	lock_release(&file_lock);
	return success;
}

/*This method should only be called from a USERPROG because otherwise it will return -1
  So for the kernel, it should directly call the filesys_open*/
static int 
open(const char *file)
{
	struct thread* curr_thread = thread_current();

	lock_acquire(&file_lock);
	// so that if something wrong happens we can release the lock even if the thread was killed

	struct file* opened_f = filesys_open(file);
	if (opened_f == NULL)
	{
		lock_release(&file_lock);
		return -1;
	}

	struct fd_entry* f_entry = calloc(1, sizeof (struct fd_entry));

	f_entry->file = opened_f;
	f_entry->fd = curr_thread->next_fd++;
	list_push_front(&curr_thread->fd_table, &f_entry->elem);

	int fd = f_entry->fd;
	lock_release(&file_lock);// releases the lock + removes it from the acquired_locks listZZZ
	// it wasnt killed and the file open was completed, we can now release the file lock
		
	return fd;
}

static int 
filesize(int fd)
{	
	if (fd == 0 || fd == 1)
		return -1;
	
	struct file* f = get_file(fd);
	// no need to acquire 'file_lock' for this since we add push at the front of the list
	// so no next pointer ever gets modified and is in an unstable state
	lock_acquire(&file_lock);
	if(f == NULL)
	{
		lock_release(&file_lock);
		return 0; // this fd doesn't exist
	}

	int len = file_length(f);
	lock_release(&file_lock);

	return len;
}

static int //note that size is limited to int max value
read(int fd, void *buffer, unsigned size)
{
	if (fd == 0)
	{
		// read from keyboard one byte at a time using input_getc()
		uint8_t *buf = (uint8_t *) buffer;
		for (unsigned i = 0; i < size; i++)
			buf[i] = input_getc();
		return (int) size;
	}
	
	if (fd == 1)
		return -1; // STDOUT is write-only

	struct file* f = get_file(fd);
	if (f == NULL)
		return -1; // invalid fd, no lock to release

	lock_acquire(&file_lock);
	int bytes_read = (int)file_read(f, buffer, size);
	lock_release(&file_lock);

	return bytes_read;
}

static int 
write(int fd, const void *buffer, unsigned size)
{	
	if (fd == 1)
	{
		// write to console — break into chunks to prevent interleaving
		const size_t CHUNK = 512;
		size_t remaining = size;
		const char *buf = (const char *) buffer;
		while (remaining > CHUNK)
		{
			putbuf(buf, CHUNK);
			buf += CHUNK;
			remaining -= CHUNK;
		}
		putbuf(buf, remaining);
		return (int) size;
	}

	if (fd == 0)
		return -1; // STDIN is read-only

	struct file* f = get_file(fd);
	if (f == NULL)
		return -1; // invalid fd, no lock to release

	lock_acquire(&file_lock);
	int bytes_written = (int)file_write(f, buffer, size);
	lock_release(&file_lock);

	return bytes_written;
}

static void 
seek(int fd, unsigned position)
{	
	if (fd == 0 || fd == 1)
		return; // STDIN,STDOUT are not random access

	struct file* f = get_file(fd);
	lock_acquire(&file_lock);
	if (f == NULL)
	{
		lock_release(&file_lock);
		return;
	}

	file_seek(f, position);
	lock_release(&file_lock);
}

static unsigned 
tell(int fd)
{	
	if (fd == 0 || fd == 1)
		return 0; // STDIN,STDOUT are not random access

	struct file* f = get_file(fd);
	lock_acquire(&file_lock);
	if (f == NULL)
	{
		lock_release(&file_lock);
		return 0;
	}
	unsigned pos = file_tell(f);
	lock_release(&file_lock);

	return pos;
}

static void 
close(int fd)
{	
	struct thread* curr = thread_current();
	struct list_elem* itr = list_begin(&curr->fd_table);
	while (itr != list_end(&curr->fd_table))
	{
		struct fd_entry* entry = list_entry(itr, struct fd_entry, elem);
		if (entry->fd == fd)
		{	
			lock_acquire(&file_lock);
			
			file_close(entry->file);
			list_remove(itr); // removes the fd_entry from the list
			free(entry); //deallocate the entry
			
			lock_release(&file_lock);
			return;
		}
		itr = list_next(itr);
	}
}
