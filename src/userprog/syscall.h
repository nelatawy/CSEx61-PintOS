#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

struct fd_entry {
    int fd;
    struct file *file;
    struct list_elem elem;
};

struct lock_entry {
    struct lock *lock;
    struct list_elem elem;
};

void syscall_init (void);

#endif /* userprog/syscall.h */
