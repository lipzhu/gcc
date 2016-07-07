//===-- asan_malloc_linux.cc ----------------------------------------------===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of AddressSanitizer, an address sanity checker.
//
// Linux-specific malloc interception.
// We simply define functions like malloc, free, realloc, etc.
// They will replace the corresponding libc functions automagically.
//===----------------------------------------------------------------------===//

#include "sanitizer_common/sanitizer_platform.h"
#if SANITIZER_LINUX

#include "asan_allocator.h"
#include "asan_interceptors.h"
#include "asan_internal.h"
#include "asan_stack.h"

#if SANITIZER_ANDROID
DECLARE_REAL_AND_INTERCEPTOR(void*, malloc, uptr size)
DECLARE_REAL_AND_INTERCEPTOR(void, free, void *ptr)
DECLARE_REAL_AND_INTERCEPTOR(void*, calloc, uptr nmemb, uptr size)
DECLARE_REAL_AND_INTERCEPTOR(void*, realloc, void *ptr, uptr size)
DECLARE_REAL_AND_INTERCEPTOR(void*, memalign, uptr boundary, uptr size)

struct MallocDebug {
  void* (*malloc)(uptr bytes);
  void  (*free)(void* mem);
  void* (*calloc)(uptr n_elements, uptr elem_size);
  void* (*realloc)(void* oldMem, uptr bytes);
  void* (*memalign)(uptr alignment, uptr bytes);
};

const MallocDebug asan_malloc_dispatch ALIGNED(32) = {
  WRAP(malloc), WRAP(free), WRAP(calloc), WRAP(realloc), WRAP(memalign)
};

extern "C" const MallocDebug* __libc_malloc_dispatch;

namespace __asan {
void ReplaceSystemMalloc() {
  __libc_malloc_dispatch = &asan_malloc_dispatch;
}
}  // namespace __asan

#else  // ANDROID

namespace __asan {
void ReplaceSystemMalloc() {
}
}  // namespace __asan
#endif  // ANDROID

// ---------------------- Replacement functions ---------------- {{{1
using namespace __asan;  // NOLINT

static uptr allocated_for_dlsym;
static const uptr kDlsymAllocPoolSize = 1024;
static uptr alloc_memory_for_dlsym[kDlsymAllocPoolSize];

static bool IsInDlsymAllocPool(const void *ptr) {
  uptr off = (uptr)ptr - (uptr)alloc_memory_for_dlsym;
  return off < sizeof(alloc_memory_for_dlsym);
}

static void *AllocateFromLocalPool(uptr size_in_bytes) {
  uptr size_in_words = RoundUpTo(size_in_bytes, kWordSize) / kWordSize;
  void *mem = (void*)&alloc_memory_for_dlsym[allocated_for_dlsym];
  allocated_for_dlsym += size_in_words;
  CHECK_LT(allocated_for_dlsym, kDlsymAllocPoolSize);
  return mem;
}

INTERCEPTOR(void, free, void *ptr) {
  GET_STACK_TRACE_FREE;
  if (UNLIKELY(IsInDlsymAllocPool(ptr)))
    return;
  asan_free(ptr, &stack, FROM_MALLOC);
}

INTERCEPTOR(void, cfree, void *ptr) {
  GET_STACK_TRACE_FREE;
  if (UNLIKELY(IsInDlsymAllocPool(ptr)))
    return;
  asan_free(ptr, &stack, FROM_MALLOC);
}

INTERCEPTOR(void*, malloc, uptr size) {
  if (UNLIKELY(!asan_inited))
    // Hack: dlsym calls malloc before REAL(malloc) is retrieved from dlsym.
    return AllocateFromLocalPool(size);
  GET_STACK_TRACE_MALLOC;
  return asan_malloc(size, &stack);
}

INTERCEPTOR(void*, calloc, uptr nmemb, uptr size) {
  if (UNLIKELY(!asan_inited))
    // Hack: dlsym calls calloc before REAL(calloc) is retrieved from dlsym.
    return AllocateFromLocalPool(nmemb * size);
  GET_STACK_TRACE_MALLOC;
  return asan_calloc(nmemb, size, &stack);
}

INTERCEPTOR(void*, realloc, void *ptr, uptr size) {
  GET_STACK_TRACE_MALLOC;
  if (UNLIKELY(IsInDlsymAllocPool(ptr))) {
    uptr offset = (uptr)ptr - (uptr)alloc_memory_for_dlsym;
    uptr copy_size = Min(size, kDlsymAllocPoolSize - offset);
    void *new_ptr = asan_malloc(size, &stack);
    internal_memcpy(new_ptr, ptr, copy_size);
    return new_ptr;
  }
  return asan_realloc(ptr, size, &stack);
}

INTERCEPTOR(void*, memalign, uptr boundary, uptr size) {
  GET_STACK_TRACE_MALLOC;
  return asan_memalign(boundary, size, &stack, FROM_MALLOC);
}

INTERCEPTOR(void*, __libc_memalign, uptr align, uptr s)
  ALIAS("memalign");

INTERCEPTOR(uptr, malloc_usable_size, void *ptr) {
  GET_CURRENT_PC_BP_SP;
  (void)sp;
  return asan_malloc_usable_size(ptr, pc, bp);
}

// We avoid including malloc.h for portability reasons.
// man mallinfo says the fields are "long", but the implementation uses int.
// It doesn't matter much -- we just need to make sure that the libc's mallinfo
// is not called.
struct fake_mallinfo {
  int x[10];
};

INTERCEPTOR(struct fake_mallinfo, mallinfo, void) {
  struct fake_mallinfo res;
  REAL(memset)(&res, 0, sizeof(res));
  return res;
}

INTERCEPTOR(int, mallopt, int cmd, int value) {
  return -1;
}

INTERCEPTOR(int, posix_memalign, void **memptr, uptr alignment, uptr size) {
  GET_STACK_TRACE_MALLOC;
  // Printf("posix_memalign: %zx %zu\n", alignment, size);
  return asan_posix_memalign(memptr, alignment, size, &stack);
}

INTERCEPTOR(void*, valloc, uptr size) {
  GET_STACK_TRACE_MALLOC;
  return asan_valloc(size, &stack);
}

INTERCEPTOR(void*, pvalloc, uptr size) {
  GET_STACK_TRACE_MALLOC;
  return asan_pvalloc(size, &stack);
}

INTERCEPTOR(void, malloc_stats, void) {
  __asan_print_accumulated_stats();
}

#endif  // SANITIZER_LINUX
