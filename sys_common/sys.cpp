/// Copyright 2021 Piotr Grygorczuk <grygorek@gmail.com>
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.

#include <cstddef>
#ifdef __xil__
#include <fcntl.h>
#endif

extern "C"
{
  // FreeRTOS malloc/free declarations
  void *pvPortMalloc(size_t);
  void vPortFree(void *);

  // Redirect malloc to FreeRTOS malloc
  void *__wrap_malloc(unsigned long long size)
  {
    return pvPortMalloc(size);
  }

  // Redirect free to FreeRTOS free
  void __wrap_free(void *p)
  {
    vPortFree(p);
  }

  void *memcpy(void *dest, const void *src, size_t n);
  // Redirect realloc to FreeRTOS malloc
  void *__wrap_realloc(void *ptr, size_t nbytes)
  {
    if (nbytes == 0)
    {
      vPortFree(ptr);
      return NULL;
    }
    void *p = pvPortMalloc(nbytes);
    if (p) {
      if (ptr) {
        memcpy(p, ptr, nbytes);
        vPortFree(ptr);
      }
    }
    return p;
  }

  extern void *memset(void *s, int c, size_t n);
  // Redirect calloc to FreeRTOS malloc
  void *__wrap_calloc(unsigned long long nmemb, unsigned long long size)
  {
    void *p = pvPortMalloc(nmemb * size);
    if (p) {
        memset(p, 0, nmemb * size);
    }
    return p;
  }

  __attribute__( ( weak ) ) void _exit()
  {
    while (1)
      ;
  }

  struct stat;
  int _stat(const char *path, struct stat *buf)
  {
    (void)path;
    (void)buf;
    return -1;
  }

#ifndef __xil__
  int _fstat(int fd, struct stat *buf)
  {
    (void)fd;
    (void)buf;
    return -1;
  }

  int _lstat(const char *path, struct stat *buf)
  {
    (void)path;
    (void)buf;
    return -1;
  }

  struct _reent;
  int _open_r(struct _reent *, const char *, int, int)
  {
    return -1;
  }
  int _close(struct _reent *, int)
  {
    return -1;
  }
  long _write(struct _reent *, int, const char *, int)
  {
    return -1;
  }
  long _read(struct _reent *, int, char *, int)
  {
    return -1;
  }

  int _sbrk(int)
  {
    while (1)
      ;
  }
  void _kill(int, int) {}
  int _getpid() { return 1; }
  int _isatty(int) { return -1; }
  int _lseek(int, int, int) { return 0; }

#else
  int _open(const char *pathname, int flags, mode_t mode)
  {
    (void)pathname;
    (void)flags;
    (void)mode;
    return -1;
  }

  int mkdir(const char *pathname, mode_t mode)
  {
    (void)pathname;
    (void)mode;
    return -1;
  }

  int posix_memalign(void **memptr, size_t alignment, size_t size)
  {
    *memptr = pvPortMalloc(size);
    if (*memptr)
      return 0;
    else
      return -1;
  }
#endif
}
