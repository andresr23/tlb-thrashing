/*
 * Functions to manipulate SHared-Memory (SHM) objects.
 *
 * -Original implementation by Stephan van Schaik for the paper:
 * Malicious Management Unit: Why Stopping Cache Attacks in Software is Harder
 * than You Think, by van Schaik et al.
 *
 * Credit to the authors...
 */
#pragma once

#include <stdlib.h>

#include "macros.h"

#define SHM_DEFAULT_PATH "/foo"
#define SHM_DEFAULT_SIZE (4 * KIB)

/* API -----------------------------------------------------------------------*/

int shm_populate(const char *path, size_t size);
int shm_open_or_create(const char *path, size_t size);
int shm_open_or_create_default(void);

void *shm_remap(void *addr, size_t size, int fd);
