#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <list.h>
#include "filesys/file.h"
#include "threads/synch.h"

struct fd_entry {
    int fd;
    struct file *file;
    struct list_elem elem;
};

struct lock_entry {
    struct lock *lock;
    struct list_elem elem;
};

void exit_failure(void);

void syscall_init (void);

#endif /* userprog/syscall.h */
