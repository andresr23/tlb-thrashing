#include "shm.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

int shm_populate(const char *path, size_t size)
{
  void *page;

  int fd = shm_open(path, O_RDWR | O_CREAT | O_EXCL,
    S_IRWXU | S_IRWXG | S_IRWXO);

  if (fd < 0) {
    PRINT_LOG("Failed to create the new shared-memory object %s.", path);
    return -1;
  }

  if (ftruncate(fd, size) < 0)
    goto err_unlink;

  page = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (page == MAP_FAILED)
    goto err_unlink;

  memset(page, 0x23, size);
  close(fd);

  return 0;

err_unlink:
  close(fd);
  shm_unlink(path);

  return -1;
}

/*
 * Attempt to open the shared-memory object pointed by 'path'. If the object
 * doesn't exist, then create it and populate it.
 *
 * @return: File descriptor (fd) for the shared-memory object pointed by 'path'.
 */
int shm_open_or_create(const char *path, size_t size)
{
  int fd = shm_open(path, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);

  if (fd >= 0)
    return fd;

  if (shm_populate(path, size) < 0)
    return -1;

  return shm_open(path, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
}

int shm_open_or_create_default(void)
{
  return shm_open_or_create(SHM_DEFAULT_PATH, SHM_DEFAULT_SIZE);
}

void *shm_remap(void *addr, size_t size, int fd)
{
  return mmap(addr, size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, fd, 0);
}
