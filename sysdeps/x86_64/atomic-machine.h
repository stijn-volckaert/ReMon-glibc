/* Copyright (C) 2002-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2002.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _X86_64_ATOMIC_MACHINE_H
#define _X86_64_ATOMIC_MACHINE_H 1

#include <stdint.h>
#include <tls.h>                   /* For tcbhead_t.  */
#include <libc-pointer-arith.h>    /* For cast_to_integer.  */

typedef int8_t atomic8_t;
typedef uint8_t uatomic8_t;
typedef int_fast8_t atomic_fast8_t;
typedef uint_fast8_t uatomic_fast8_t;

typedef int16_t atomic16_t;
typedef uint16_t uatomic16_t;
typedef int_fast16_t atomic_fast16_t;
typedef uint_fast16_t uatomic_fast16_t;

typedef int32_t atomic32_t;
typedef uint32_t uatomic32_t;
typedef int_fast32_t atomic_fast32_t;
typedef uint_fast32_t uatomic_fast32_t;

typedef int64_t atomic64_t;
typedef uint64_t uatomic64_t;
typedef int_fast64_t atomic_fast64_t;
typedef uint_fast64_t uatomic_fast64_t;

typedef intptr_t atomicptr_t;
typedef uintptr_t uatomicptr_t;
typedef intmax_t atomic_max_t;
typedef uintmax_t uatomic_max_t;


#ifndef LOCK_PREFIX
# ifdef UP
#  define LOCK_PREFIX	/* nothing */
# else
#  define LOCK_PREFIX "lock;"
# endif
#endif

#define __HAVE_64B_ATOMICS 1
#define USE_ATOMIC_COMPILER_BUILTINS 1
#define ATOMIC_EXCHANGE_USES_CAS 0

#define orig_atomic_compare_and_exchange_val_acq(mem, newval, oldval) \
  __sync_val_compare_and_swap (mem, oldval, newval)
#define orig_atomic_compare_and_exchange_bool_acq(mem, newval, oldval) \
  (! __sync_bool_compare_and_swap (mem, oldval, newval))


#define orig___arch_c_compare_and_exchange_val_8_acq(mem, newval, oldval) \
  ({ __typeof (*mem) ret;						      \
    __asm __volatile ("cmpl $0, %%fs:%P5\n\t"				      \
		      "je 0f\n\t"					      \
		      "lock\n"						      \
		       "0:\tcmpxchgb %b2, %1"				      \
		       : "=a" (ret), "=m" (*mem)			      \
		       : "q" (newval), "m" (*mem), "0" (oldval),	      \
			 "i" (offsetof (tcbhead_t, multiple_threads)));	      \
     ret; })

#define orig___arch_c_compare_and_exchange_val_16_acq(mem, newval, oldval) \
  ({ __typeof (*mem) ret;						      \
    __asm __volatile ("cmpl $0, %%fs:%P5\n\t"				      \
		      "je 0f\n\t"					      \
		      "lock\n"						      \
		       "0:\tcmpxchgw %w2, %1"				      \
		       : "=a" (ret), "=m" (*mem)			      \
		       : "q" (newval), "m" (*mem), "0" (oldval),	      \
			 "i" (offsetof (tcbhead_t, multiple_threads)));	      \
     ret; })

#define orig___arch_c_compare_and_exchange_val_32_acq(mem, newval, oldval) \
  ({ __typeof (*mem) ret;						      \
    __asm __volatile ("cmpl $0, %%fs:%P5\n\t"				      \
		      "je 0f\n\t"					      \
		      "lock\n"						      \
		       "0:\tcmpxchgl %2, %1"				      \
		       : "=a" (ret), "=m" (*mem)			      \
		       : "q" (newval), "m" (*mem), "0" (oldval),	      \
			 "i" (offsetof (tcbhead_t, multiple_threads)));	      \
     ret; })

#define orig___arch_c_compare_and_exchange_val_64_acq(mem, newval, oldval) \
  ({ __typeof (*mem) ret;						      \
     __asm __volatile ("cmpl $0, %%fs:%P5\n\t"				      \
		       "je 0f\n\t"					      \
		       "lock\n"						      \
		       "0:\tcmpxchgq %q2, %1"				      \
		       : "=a" (ret), "=m" (*mem)			      \
		       : "q" ((atomic64_t) cast_to_integer (newval)),	      \
			 "m" (*mem),					      \
			 "0" ((atomic64_t) cast_to_integer (oldval)),	      \
			 "i" (offsetof (tcbhead_t, multiple_threads)));	      \
     ret; })


/* Note that we need no lock prefix.  */
#define orig_atomic_exchange_acq(mem, newvalue) \
  ({ __typeof (*mem) result;						      \
     if (sizeof (*mem) == 1)						      \
       __asm __volatile ("xchgb %b0, %1"				      \
			 : "=q" (result), "=m" (*mem)			      \
			 : "0" (newvalue), "m" (*mem));			      \
     else if (sizeof (*mem) == 2)					      \
       __asm __volatile ("xchgw %w0, %1"				      \
			 : "=r" (result), "=m" (*mem)			      \
			 : "0" (newvalue), "m" (*mem));			      \
     else if (sizeof (*mem) == 4)					      \
       __asm __volatile ("xchgl %0, %1"					      \
			 : "=r" (result), "=m" (*mem)			      \
			 : "0" (newvalue), "m" (*mem));			      \
     else								      \
       __asm __volatile ("xchgq %q0, %1"				      \
			 : "=r" (result), "=m" (*mem)			      \
			 : "0" ((atomic64_t) cast_to_integer (newvalue)),     \
			   "m" (*mem));					      \
     result; })


#define orig___arch_exchange_and_add_body(lock, mem, value)			      \
  ({ __typeof (*mem) result;						      \
     if (sizeof (*mem) == 1)						      \
       __asm __volatile (lock "xaddb %b0, %1"				      \
			 : "=q" (result), "=m" (*mem)			      \
			 : "0" (value), "m" (*mem),			      \
			   "i" (offsetof (tcbhead_t, multiple_threads)));     \
     else if (sizeof (*mem) == 2)					      \
       __asm __volatile (lock "xaddw %w0, %1"				      \
			 : "=r" (result), "=m" (*mem)			      \
			 : "0" (value), "m" (*mem),			      \
			   "i" (offsetof (tcbhead_t, multiple_threads)));     \
     else if (sizeof (*mem) == 4)					      \
       __asm __volatile (lock "xaddl %0, %1"				      \
			 : "=r" (result), "=m" (*mem)			      \
			 : "0" (value), "m" (*mem),			      \
			   "i" (offsetof (tcbhead_t, multiple_threads)));     \
     else								      \
       __asm __volatile (lock "xaddq %q0, %1"				      \
			 : "=r" (result), "=m" (*mem)			      \
			 : "0" ((atomic64_t) cast_to_integer (value)),	      \
			   "m" (*mem),					      \
			   "i" (offsetof (tcbhead_t, multiple_threads)));     \
     result; })

#define orig_atomic_exchange_and_add(mem, value) \
  __sync_fetch_and_add (mem, value)

#define __arch_exchange_and_add_cprefix \
  "cmpl $0, %%fs:%P4\n\tje 0f\n\tlock\n0:\t"

#define orig_catomic_exchange_and_add(mem, value) \
  orig___arch_exchange_and_add_body (__arch_exchange_and_add_cprefix, mem, value)


#define orig___arch_add_body(lock, pfx, mem, value)				      \
  do {									      \
    if (__builtin_constant_p (value) && (value) == 1)			      \
      pfx##_increment (mem);						      \
    else if (__builtin_constant_p (value) && (value) == -1)		      \
      pfx##_decrement (mem);						      \
    else if (sizeof (*mem) == 1)					      \
      __asm __volatile (lock "addb %b1, %0"				      \
			: "=m" (*mem)					      \
			: "iq" (value), "m" (*mem),			      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else if (sizeof (*mem) == 2)					      \
      __asm __volatile (lock "addw %w1, %0"				      \
			: "=m" (*mem)					      \
			: "ir" (value), "m" (*mem),			      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else if (sizeof (*mem) == 4)					      \
      __asm __volatile (lock "addl %1, %0"				      \
			: "=m" (*mem)					      \
			: "ir" (value), "m" (*mem),			      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else								      \
      __asm __volatile (lock "addq %q1, %0"				      \
			: "=m" (*mem)					      \
			: "ir" ((atomic64_t) cast_to_integer (value)),	      \
			  "m" (*mem),					      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
  } while (0)

#define orig_atomic_add(mem, value) \
  orig___arch_add_body (LOCK_PREFIX, orig_atomic, mem, value)

#define __arch_add_cprefix \
  "cmpl $0, %%fs:%P3\n\tje 0f\n\tlock\n0:\t"

#define orig_catomic_add(mem, value) \
  orig___arch_add_body (__arch_add_cprefix, orig_catomic, mem, value)


#define orig_atomic_add_negative(mem, value) \
  ({ unsigned char __result;						      \
     if (sizeof (*mem) == 1)						      \
       __asm __volatile (LOCK_PREFIX "addb %b2, %0; sets %1"		      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "iq" (value), "m" (*mem));			      \
     else if (sizeof (*mem) == 2)					      \
       __asm __volatile (LOCK_PREFIX "addw %w2, %0; sets %1"		      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "ir" (value), "m" (*mem));			      \
     else if (sizeof (*mem) == 4)					      \
       __asm __volatile (LOCK_PREFIX "addl %2, %0; sets %1"		      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "ir" (value), "m" (*mem));			      \
     else								      \
       __asm __volatile (LOCK_PREFIX "addq %q2, %0; sets %1"		      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "ir" ((atomic64_t) cast_to_integer (value)),	      \
			   "m" (*mem));					      \
     __result; })


#define orig_atomic_add_zero(mem, value) \
  ({ unsigned char __result;						      \
     if (sizeof (*mem) == 1)						      \
       __asm __volatile (LOCK_PREFIX "addb %b2, %0; setz %1"		      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "iq" (value), "m" (*mem));			      \
     else if (sizeof (*mem) == 2)					      \
       __asm __volatile (LOCK_PREFIX "addw %w2, %0; setz %1"		      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "ir" (value), "m" (*mem));			      \
     else if (sizeof (*mem) == 4)					      \
       __asm __volatile (LOCK_PREFIX "addl %2, %0; setz %1"		      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "ir" (value), "m" (*mem));			      \
     else								      \
       __asm __volatile (LOCK_PREFIX "addq %q2, %0; setz %1"		      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "ir" ((atomic64_t) cast_to_integer (value)),	      \
			   "m" (*mem));					      \
     __result; })


#define orig___arch_increment_body(lock, mem) \
  do {									      \
    if (sizeof (*mem) == 1)						      \
      __asm __volatile (lock "incb %b0"					      \
			: "=m" (*mem)					      \
			: "m" (*mem),					      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else if (sizeof (*mem) == 2)					      \
      __asm __volatile (lock "incw %w0"					      \
			: "=m" (*mem)					      \
			: "m" (*mem),					      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else if (sizeof (*mem) == 4)					      \
      __asm __volatile (lock "incl %0"					      \
			: "=m" (*mem)					      \
			: "m" (*mem),					      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else								      \
      __asm __volatile (lock "incq %q0"					      \
			: "=m" (*mem)					      \
			: "m" (*mem),					      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
  } while (0)

#define orig_atomic_increment(mem) orig___arch_increment_body (LOCK_PREFIX, mem)

#define __arch_increment_cprefix \
  "cmpl $0, %%fs:%P2\n\tje 0f\n\tlock\n0:\t"

#define orig_catomic_increment(mem) \
  orig___arch_increment_body (__arch_increment_cprefix, mem)


#define orig_atomic_increment_and_test(mem) \
  ({ unsigned char __result;						      \
     if (sizeof (*mem) == 1)						      \
       __asm __volatile (LOCK_PREFIX "incb %b0; sete %1"		      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "m" (*mem));					      \
     else if (sizeof (*mem) == 2)					      \
       __asm __volatile (LOCK_PREFIX "incw %w0; sete %1"		      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "m" (*mem));					      \
     else if (sizeof (*mem) == 4)					      \
       __asm __volatile (LOCK_PREFIX "incl %0; sete %1"			      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "m" (*mem));					      \
     else								      \
       __asm __volatile (LOCK_PREFIX "incq %q0; sete %1"		      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "m" (*mem));					      \
     __result; })


#define orig___arch_decrement_body(lock, mem) \
  do {									      \
    if (sizeof (*mem) == 1)						      \
      __asm __volatile (lock "decb %b0"					      \
			: "=m" (*mem)					      \
			: "m" (*mem),					      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else if (sizeof (*mem) == 2)					      \
      __asm __volatile (lock "decw %w0"					      \
			: "=m" (*mem)					      \
			: "m" (*mem),					      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else if (sizeof (*mem) == 4)					      \
      __asm __volatile (lock "decl %0"					      \
			: "=m" (*mem)					      \
			: "m" (*mem),					      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else								      \
      __asm __volatile (lock "decq %q0"					      \
			: "=m" (*mem)					      \
			: "m" (*mem),					      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
  } while (0)

#define orig_atomic_decrement(mem) orig___arch_decrement_body (LOCK_PREFIX, mem)

#define __arch_decrement_cprefix \
  "cmpl $0, %%fs:%P2\n\tje 0f\n\tlock\n0:\t"

#define orig_catomic_decrement(mem) \
  orig___arch_decrement_body (__arch_decrement_cprefix, mem)


#define orig_atomic_decrement_and_test(mem) \
  ({ unsigned char __result;						      \
     if (sizeof (*mem) == 1)						      \
       __asm __volatile (LOCK_PREFIX "decb %b0; sete %1"		      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "m" (*mem));					      \
     else if (sizeof (*mem) == 2)					      \
       __asm __volatile (LOCK_PREFIX "decw %w0; sete %1"		      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "m" (*mem));					      \
     else if (sizeof (*mem) == 4)					      \
       __asm __volatile (LOCK_PREFIX "decl %0; sete %1"			      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "m" (*mem));					      \
     else								      \
       __asm __volatile (LOCK_PREFIX "decq %q0; sete %1"		      \
			 : "=m" (*mem), "=qm" (__result)		      \
			 : "m" (*mem));					      \
     __result; })


#define orig_atomic_bit_set(mem, bit) \
  do {									      \
    if (sizeof (*mem) == 1)						      \
      __asm __volatile (LOCK_PREFIX "orb %b2, %0"			      \
			: "=m" (*mem)					      \
			: "m" (*mem), "iq" (1L << (bit)));		      \
    else if (sizeof (*mem) == 2)					      \
      __asm __volatile (LOCK_PREFIX "orw %w2, %0"			      \
			: "=m" (*mem)					      \
			: "m" (*mem), "ir" (1L << (bit)));		      \
    else if (sizeof (*mem) == 4)					      \
      __asm __volatile (LOCK_PREFIX "orl %2, %0"			      \
			: "=m" (*mem)					      \
			: "m" (*mem), "ir" (1L << (bit)));		      \
    else if (__builtin_constant_p (bit) && (bit) < 32)			      \
      __asm __volatile (LOCK_PREFIX "orq %2, %0"			      \
			: "=m" (*mem)					      \
			: "m" (*mem), "i" (1L << (bit)));		      \
    else								      \
      __asm __volatile (LOCK_PREFIX "orq %q2, %0"			      \
			: "=m" (*mem)					      \
			: "m" (*mem), "r" (1UL << (bit)));		      \
  } while (0)


#define orig_atomic_bit_test_set(mem, bit) \
  ({ unsigned char __result;						      \
     if (sizeof (*mem) == 1)						      \
       __asm __volatile (LOCK_PREFIX "btsb %3, %1; setc %0"		      \
			 : "=q" (__result), "=m" (*mem)			      \
			 : "m" (*mem), "iq" (bit));			      \
     else if (sizeof (*mem) == 2)					      \
       __asm __volatile (LOCK_PREFIX "btsw %3, %1; setc %0"		      \
			 : "=q" (__result), "=m" (*mem)			      \
			 : "m" (*mem), "ir" (bit));			      \
     else if (sizeof (*mem) == 4)					      \
       __asm __volatile (LOCK_PREFIX "btsl %3, %1; setc %0"		      \
			 : "=q" (__result), "=m" (*mem)			      \
			 : "m" (*mem), "ir" (bit));			      \
     else							      	      \
       __asm __volatile (LOCK_PREFIX "btsq %3, %1; setc %0"		      \
			 : "=q" (__result), "=m" (*mem)			      \
			 : "m" (*mem), "ir" (bit));			      \
     __result; })


#define atomic_spin_nop() asm ("rep; nop")


#define orig___arch_and_body(lock, mem, mask) \
  do {									      \
    if (sizeof (*mem) == 1)						      \
      __asm __volatile (lock "andb %b1, %0"				      \
			: "=m" (*mem)					      \
			: "iq" (mask), "m" (*mem),			      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else if (sizeof (*mem) == 2)					      \
      __asm __volatile (lock "andw %w1, %0"				      \
			: "=m" (*mem)					      \
			: "ir" (mask), "m" (*mem),			      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else if (sizeof (*mem) == 4)					      \
      __asm __volatile (lock "andl %1, %0"				      \
			: "=m" (*mem)					      \
			: "ir" (mask), "m" (*mem),			      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else								      \
      __asm __volatile (lock "andq %q1, %0"				      \
			: "=m" (*mem)					      \
			: "ir" (mask), "m" (*mem),			      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
  } while (0)

#define __arch_cprefix \
  "cmpl $0, %%fs:%P3\n\tje 0f\n\tlock\n0:\t"

#define orig_atomic_and(mem, mask) orig___arch_and_body (LOCK_PREFIX, mem, mask)

#define orig_catomic_and(mem, mask) orig___arch_and_body (__arch_cprefix, mem, mask)


#define orig___arch_or_body(lock, mem, mask)					      \
  do {									      \
    if (sizeof (*mem) == 1)						      \
      __asm __volatile (lock "orb %b1, %0"				      \
			: "=m" (*mem)					      \
			: "iq" (mask), "m" (*mem),			      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else if (sizeof (*mem) == 2)					      \
      __asm __volatile (lock "orw %w1, %0"				      \
			: "=m" (*mem)					      \
			: "ir" (mask), "m" (*mem),			      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else if (sizeof (*mem) == 4)					      \
      __asm __volatile (lock "orl %1, %0"				      \
			: "=m" (*mem)					      \
			: "ir" (mask), "m" (*mem),			      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
    else								      \
      __asm __volatile (lock "orq %q1, %0"				      \
			: "=m" (*mem)					      \
			: "ir" (mask), "m" (*mem),			      \
			  "i" (offsetof (tcbhead_t, multiple_threads)));      \
  } while (0)

#define orig_atomic_or(mem, mask) orig___arch_or_body (LOCK_PREFIX, mem, mask)

#define orig_catomic_or(mem, mask) orig___arch_or_body (__arch_cprefix, mem, mask)

/* We don't use mfence because it is supposedly slower due to having to
   provide stronger guarantees (e.g., regarding self-modifying code).  */
#define atomic_full_barrier() \
    __asm __volatile (LOCK_PREFIX "orl $0, (%%rsp)" ::: "memory")
#define atomic_read_barrier() __asm ("" ::: "memory")
#define atomic_write_barrier() __asm ("" ::: "memory")


/*--------------------------------------------------------------------------------
                                  MVEE PATCHES
--------------------------------------------------------------------------------*/
#define USE_MVEE_LIBC

#ifdef USE_MVEE_LIBC
#define MVEE_FAKE_SYSCALL_BASE          0x6FFFFFFF
#define MVEE_GET_MASTERTHREAD_ID        MVEE_FAKE_SYSCALL_BASE + 3
#define MVEE_GET_SHARED_BUFFER          MVEE_FAKE_SYSCALL_BASE + 4
#define MVEE_FLUSH_SHARED_BUFFER        MVEE_FAKE_SYSCALL_BASE + 5
#define MVEE_SET_INFINITE_LOOP_PTR      MVEE_FAKE_SYSCALL_BASE + 6
#define MVEE_TOGGLESYNC                 MVEE_FAKE_SYSCALL_BASE + 7
#define MVEE_SET_SHARED_BUFFER_POS_PTR  MVEE_FAKE_SYSCALL_BASE + 8
#define MVEE_RUNS_UNDER_MVEE_CONTROL    MVEE_FAKE_SYSCALL_BASE + 9
#define MVEE_GET_THREAD_NUM             MVEE_FAKE_SYSCALL_BASE + 10
#define MVEE_SET_SYNC_PRIMITIVES_PTR    MVEE_FAKE_SYSCALL_BASE + 12
#define MVEE_ALL_HEAPS_ALIGNED          MVEE_FAKE_SYSCALL_BASE + 13
#define MVEE_GET_VIRTUALIZED_ARGV0      MVEE_FAKE_SYSCALL_BASE + 17
#define MVEE_LIBC_LOCK_BUFFER           3
#define MVEE_LIBC_MALLOC_DEBUG_BUFFER   11
#define MVEE_LIBC_ATOMIC_BUFFER         13
#define MVEE_LIBC_LOCK_BUFFER_PARTIAL   16
#define MVEE_FUTEX_WAIT_TID             30

enum mvee_alloc_types
  {
  LIBC_MALLOC,
  LIBC_FREE,
  LIBC_REALLOC,
  LIBC_MEMALIGN,
  LIBC_CALLOC,
  MALLOC_TRIM,
  HEAP_TRIM,
  MALLOC_CONSOLIDATE,
  ARENA_GET2,
  _INT_MALLOC,
  _INT_FREE,
  _INT_REALLOC
  };

//
// Atomic operations list. Keep this in sync with MVEE/Inc/MVEE_shm.h
//
enum mvee_base_atomics
{
    // LOAD OPERATIONS FIRST!!! DO NOT CHANGE THIS CONVENTION
    ATOMIC_FORCED_READ,
    ATOMIC_LOAD,
    // THE FOLLOWING IS NOT AN ACTUAL ATOMIC OPERATION, IT JUST DENOTES THE END OF THE LOAD-ONLY ATOMICS!!!
    ATOMIC_LOAD_MAX,
    // STORES AFTER LOADS
    CATOMIC_AND,
    CATOMIC_OR,
    CATOMIC_EXCHANGE_AND_ADD,
    CATOMIC_ADD,
    CATOMIC_INCREMENT,
    CATOMIC_DECREMENT,
    CATOMIC_MAX,
    ATOMIC_COMPARE_AND_EXCHANGE_VAL,
    ATOMIC_COMPARE_AND_EXCHANGE_BOOL,
    ATOMIC_EXCHANGE,
    ATOMIC_EXCHANGE_AND_ADD,
    ATOMIC_INCREMENT_AND_TEST,
    ATOMIC_DECREMENT_AND_TEST,
	ATOMIC_ADD_NEGATIVE,
    ATOMIC_ADD_ZERO,
    ATOMIC_ADD,
	ATOMIC_OR,
	ATOMIC_OR_VAL,
    ATOMIC_INCREMENT,
    ATOMIC_DECREMENT,
    ATOMIC_BIT_TEST_SET,
    ATOMIC_BIT_SET,
    ATOMIC_AND,
	ATOMIC_AND_VAL,
    ATOMIC_STORE,
	ATOMIC_MIN,
    ATOMIC_MAX,
    ATOMIC_DECREMENT_IF_POSITIVE,
	ATOMIC_FETCH_ADD,
	ATOMIC_FETCH_AND,
	ATOMIC_FETCH_OR,
	ATOMIC_FETCH_XOR,
    __THREAD_ATOMIC_CMPXCHG_VAL,
    __THREAD_ATOMIC_AND,
    __THREAD_ATOMIC_BIT_SET,
    ___UNKNOWN_LOCK_TYPE___,
    __MVEE_BASE_ATOMICS_MAX__
};

enum mvee_extended_atomics {
  mvee_atomic_load_n,
  mvee_atomic_load,
  mvee_atomic_store_n,
  mvee_atomic_store,
  mvee_atomic_exchange_n,
  mvee_atomic_exchange,
  mvee_atomic_compare_exchange_n,
  mvee_atomic_compare_exchange,
  mvee_atomic_add_fetch,
  mvee_atomic_sub_fetch,
  mvee_atomic_and_fetch,
  mvee_atomic_xor_fetch,
  mvee_atomic_or_fetch,
  mvee_atomic_nand_fetch,
  mvee_atomic_fetch_add,
  mvee_atomic_fetch_sub,
  mvee_atomic_fetch_and,
  mvee_atomic_fetch_xor,
  mvee_atomic_fetch_or,
  mvee_atomic_fetch_nand,
  mvee_atomic_test_and_set,
  mvee_atomic_clear,
  mvee_atomic_always_lock_free,
  mvee_atomic_is_lock_free,
  mvee_sync_fetch_and_add,
  mvee_sync_fetch_and_sub,
  mvee_sync_fetch_and_or,
  mvee_sync_fetch_and_and,
  mvee_sync_fetch_and_xor,
  mvee_sync_fetch_and_nand,
  mvee_sync_add_and_fetch,
  mvee_sync_sub_and_fetch,
  mvee_sync_or_and_fetch,
  mvee_sync_and_and_fetch,
  mvee_sync_xor_and_fetch,
  mvee_sync_nand_and_fetch,
  mvee_sync_bool_compare_and_swap,
  mvee_sync_val_compare_and_swap,
  mvee_sync_lock_test_and_set,
  mvee_sync_lock_release,
  mvee_atomic_ops_max
};

#define MVEE_ROUND_UP(x, multiple)		\
  ((x + (multiple - 1)) & ~(multiple -1))

#define MVEE_MIN(a, b) ((a > b) ? (b) : (a))

# ifdef MVEE_USE_TOTALPARTIAL_AGENT
#  include "mvee-totalpartial-agent.h"
# else
#  include "mvee-woc-agent.h"
# endif

#endif

// We don't use our sync agent in the dynamic loader so just use the original atomics everywhere
#if IS_IN (rtld) || !defined USE_MVEE_LIBC

//
// generic atomics (include/atomic.h and sysdeps/arch/atomic-machine.h)
//
#define __arch_c_compare_and_exchange_val_8_acq(mem, newval, oldval) orig___arch_c_compare_and_exchange_val_8_acq(mem, newval, oldval)
#define __arch_c_compare_and_exchange_val_16_acq(mem, newval, oldval) orig___arch_c_compare_and_exchange_val_16_acq(mem, newval, oldval)
#define __arch_c_compare_and_exchange_val_32_acq(mem, newval, oldval) orig___arch_c_compare_and_exchange_val_32_acq(mem, newval, oldval)
#define __arch_c_compare_and_exchange_val_64_acq(mem, newval, oldval) orig___arch_c_compare_and_exchange_val_64_acq(mem, newval, oldval)
#define atomic_compare_and_exchange_val_acq(mem, newval, oldval) orig_atomic_compare_and_exchange_val_acq(mem, newval, oldval)
#define atomic_compare_and_exchange_val_rel(mem, newval, oldval) orig_atomic_compare_and_exchange_val_rel(mem, newval, oldval)
#define atomic_compare_and_exchange_bool_acq(mem, newval, oldval) orig_atomic_compare_and_exchange_bool_acq(mem, newval, oldval)
#define atomic_compare_and_exchange_bool_rel(mem, newval, oldval) orig_atomic_compare_and_exchange_bool_rel(mem, newval, oldval)
#define atomic_exchange_acq(mem, newvalue) orig_atomic_exchange_acq(mem, newvalue)
#define atomic_exchange_rel(mem, newvalue) orig_atomic_exchange_rel(mem, newvalue)
#define atomic_exchange_and_add(mem, value) orig_atomic_exchange_and_add(mem, value)
#define atomic_exchange_and_add_acq(mem, value) orig_atomic_exchange_and_add_acq(mem, value)
#define atomic_exchange_and_add_rel(mem, value) orig_atomic_exchange_and_add_rel(mem, value)
#define atomic_add(mem, value) orig_atomic_add(mem, value)
#define atomic_increment(mem) orig_atomic_increment(mem)
#define atomic_increment_and_test(mem) orig_atomic_increment_and_test(mem)
#define atomic_increment_val(mem) orig_atomic_increment_val(mem)
#define atomic_decrement(mem) orig_atomic_decrement(mem)
#define atomic_decrement_and_test(mem) orig_atomic_decrement_and_test(mem)
#define atomic_decrement_val(mem) orig_atomic_decrement_val(mem)
#define atomic_add_negative(mem, value) orig_atomic_add_negative(mem, value)
#define atomic_add_zero(mem, value) orig_atomic_add_zero(mem, value)
#define atomic_bit_set(mem, bit) orig_atomic_bit_set(mem, bit)
#define atomic_bit_test_set(mem, bit) orig_atomic_bit_test_set(mem, bit)
#define atomic_and(mem, mask) orig_atomic_and(mem, mask)
#define atomic_or(mem, mask) orig_atomic_or(mem, mask)
#define atomic_max(mem, value) orig_atomic_max(mem, value)
#define atomic_min(mem, value) orig_atomic_min(mem, value)
#define atomic_decrement_if_positive(mem) orig_atomic_decrement_if_positive(mem)
#define atomic_and_val(mem, mask) orig_atomic_and_val(mem, mask)
#define atomic_or_val(mem, mask) orig_atomic_or_val(mem, mask)
#define atomic_forced_read(x) orig_atomic_forced_read(x)
#define catomic_compare_and_exchange_val_acq(mem, newval, oldval) orig_catomic_compare_and_exchange_val_acq(mem, newval, oldval)
#define catomic_compare_and_exchange_val_rel(mem, newval, oldval) orig_catomic_compare_and_exchange_val_rel(mem, newval, oldval)
#define catomic_compare_and_exchange_bool_acq(mem, newval, oldval) orig_catomic_compare_and_exchange_bool_acq(mem, newval, oldval)
#define catomic_compare_and_exchange_bool_rel(mem, newval, oldval) orig_catomic_compare_and_exchange_bool_rel(mem, newval, oldval)
#define catomic_exchange_and_add(mem, value) orig_catomic_exchange_and_add(mem, value)
#define catomic_add(mem, value) orig_catomic_add(mem, value)
#define catomic_increment(mem) orig_catomic_increment(mem)
#define catomic_increment_val(mem) orig_catomic_increment_val(mem)
#define catomic_decrement(mem) orig_catomic_decrement(mem)
#define catomic_decrement_val(mem) orig_catomic_decrement_val(mem)
#define catomic_and(mem, mask) orig_catomic_and(mem, mask)
#define catomic_or(mem, mask) orig_catomic_or(mem, mask)
#define catomic_max(mem, value) orig_catomic_max(mem, value)

//
// C11-style atomics (include/atomic.h)
//
#define atomic_load_relaxed(mem) orig_atomic_load_relaxed(mem)
#define atomic_load_acquire(mem) orig_atomic_load_acquire(mem)
#define atomic_store_relaxed(mem, val) orig_atomic_store_relaxed(mem, val)
#define atomic_store_release(mem, val) orig_atomic_store_release(mem, val)
#define atomic_compare_exchange_weak_relaxed(mem, expected, desired) orig_atomic_compare_exchange_weak_relaxed(mem, expected, desired)
#define atomic_compare_exchange_weak_acquire(mem, expected, desired) orig_atomic_compare_exchange_weak_acquire(mem, expected, desired) 
#define atomic_compare_exchange_weak_release(mem, expected, desired) orig_atomic_compare_exchange_weak_release(mem, expected, desired)
#define atomic_exchange_relaxed(mem, desired) orig_atomic_exchange_relaxed(mem, desired)
#define atomic_exchange_acquire(mem, desired) orig_atomic_exchange_acquire(mem, desired)
#define atomic_exchange_release(mem, desired) orig_atomic_exchange_release(mem, desired)
#define atomic_fetch_add_relaxed(mem, operand) orig_atomic_fetch_add_relaxed(mem, operand)
#define atomic_fetch_add_acquire(mem, operand) orig_atomic_fetch_add_acquire(mem, operand)
#define atomic_fetch_add_release(mem, operand) orig_atomic_fetch_add_release(mem, operand)
#define atomic_fetch_add_acq_rel(mem, operand) orig_atomic_fetch_add_acq_rel(mem, operand)
#define atomic_fetch_and_relaxed(mem, operand) orig_atomic_fetch_and_relaxed(mem, operand)
#define atomic_fetch_and_acquire(mem, operand) orig_atomic_fetch_and_acquire(mem, operand)
#define atomic_fetch_and_release(mem, operand) orig_atomic_fetch_and_release(mem, operand)
#define atomic_fetch_or_relaxed(mem, operand) orig_atomic_fetch_or_relaxed(mem, operand) 
#define atomic_fetch_or_acquire(mem, operand) orig_atomic_fetch_or_acquire(mem, operand) 
#define atomic_fetch_or_release(mem, operand) orig_atomic_fetch_or_release(mem, operand) 
#define atomic_fetch_xor_release(mem, operand) orig_atomic_fetch_xor_release(mem, operand) 

//
// TLS atomics (tls.h)
//
#define THREAD_ATOMIC_CMPXCHG_VAL(descr, member, newval, oldval) orig_THREAD_ATOMIC_CMPXCHG_VAL(descr, member, newval, oldval)
#define THREAD_ATOMIC_AND(descr, member, val) orig_THREAD_ATOMIC_AND(descr, member, val)
#define THREAD_ATOMIC_BIT_SET(descr, member, bit) orig_THREAD_ATOMIC_BIT_SET(descr, member, bit)

//
// MVEE additions
//
#define THREAD_ATOMIC_GETMEM(descr, member) THREAD_GETMEM(descr, member)
#define THREAD_ATOMIC_SETMEM(descr, member, val) THREAD_SETMEM(descr, member, val)


#else // !IS_IN_rtld

//
// architecture-specific atomics (atomic-machine.h)
//
#define __arch_c_compare_and_exchange_val_8_acq(mem, newval, oldval)	\
	({																	\
		typeof(*mem) ____result;										\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_VAL, mem, 1);			\
		____result = orig___arch_c_compare_and_exchange_val_8_acq(mem, newval, oldval); \
		MVEE_POSTOP();													\
		____result;														\
	})

#define __arch_c_compare_and_exchange_val_16_acq(mem, newval, oldval)	\
	({																	\
		typeof(*mem) ____result;										\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_VAL, mem, 1);			\
		____result = orig___arch_c_compare_and_exchange_val_16_acq(mem, newval, oldval); \
		MVEE_POSTOP();													\
		____result;														\
	})

#define __arch_c_compare_and_exchange_val_32_acq(mem, newval, oldval)	\
	({																	\
		typeof(*mem) ____result;										\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_VAL, mem, 1);			\
		____result = orig___arch_c_compare_and_exchange_val_32_acq(mem, newval, oldval); \
		MVEE_POSTOP();													\
		____result;														\
	})

#define __arch_c_compare_and_exchange_val_64_acq(mem, newval, oldval)	\
	({																	\
		typeof(*mem) ____result;										\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_VAL, mem, 1);			\
		____result = orig___arch_c_compare_and_exchange_val_64_acq(mem, newval, oldval); \
		MVEE_POSTOP();													\
		____result;														\
	})

#define atomic_compare_and_exchange_val_acq(mem, newval, oldval)		\
	({																	\
		typeof(*mem) ____result;										\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_VAL, mem, 1);			\
		____result = orig_atomic_compare_and_exchange_val_acq(mem, newval, oldval); \
		MVEE_POSTOP();													\
		____result;														\
	})

#define atomic_compare_and_exchange_val_rel(mem, newval, oldval)		\
	({																	\
		typeof(*mem) ____result;										\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_VAL, mem, 1);			\
		____result = orig_atomic_compare_and_exchange_val_rel(mem, newval, oldval); \
		MVEE_POSTOP();													\
		____result;														\
	})

#define atomic_compare_and_exchange_bool_acq(mem, newval, oldval)		\
	({																	\
		bool ____result;												\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_BOOL, mem, 1);			\
		____result = orig_atomic_compare_and_exchange_bool_acq(mem, newval, oldval); \
		MVEE_POSTOP();													\
		____result;														\
	})

#define atomic_compare_and_exchange_bool_rel(mem, newval, oldval)		\
	({																	\
		bool ____result;												\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_BOOL, mem, 1);			\
		____result = orig_atomic_compare_and_exchange_bool_rel(mem, newval, oldval); \
		MVEE_POSTOP();													\
		____result;														\
	})

#define atomic_exchange_acq(mem, newvalue)						\
	({															\
		typeof(*mem) ____result;								\
		MVEE_PREOP(ATOMIC_EXCHANGE, mem, 1);					\
		____result = orig_atomic_exchange_acq(mem, newvalue);	\
		MVEE_POSTOP();											\
		____result;												\
	})

#define atomic_exchange_rel(mem, newvalue)						\
	({															\
		typeof(*mem) ____result;								\
		MVEE_PREOP(ATOMIC_EXCHANGE, mem, 1);					\
		____result = orig_atomic_exchange_rel(mem, newvalue);	\
		MVEE_POSTOP();											\
		____result;												\
	})

#define atomic_exchange_and_add(mem, value)						\
	({															\
		typeof(*mem) ____result;								\
		MVEE_PREOP(ATOMIC_EXCHANGE_AND_ADD, mem, 1);			\
		____result = orig_atomic_exchange_and_add(mem, value);	\
		MVEE_POSTOP();											\
		____result;												\
	})

#define atomic_exchange_and_add_acq(mem, value)					\
	({															\
		typeof(*mem) ____result;								\
		MVEE_PREOP(ATOMIC_EXCHANGE_AND_ADD, mem, 1);			\
		____result = orig_atomic_exchange_and_add_acq(mem, value);	\
		MVEE_POSTOP();											\
		____result;												\
	})

#define atomic_exchange_and_add_rel(mem, value)					\
	({															\
		typeof(*mem) ____result;								\
		MVEE_PREOP(ATOMIC_EXCHANGE_AND_ADD, mem, 1);			\
		____result = orig_atomic_exchange_and_add_rel(mem, value);	\
		MVEE_POSTOP();											\
		____result;												\
	})

#define atomic_add(mem, value)					\
	({											\
		MVEE_PREOP(ATOMIC_ADD, mem, 1);			\
		orig_atomic_add(mem, value);			\
		MVEE_POSTOP();							\
	})

#define atomic_increment(mem)					\
	({											\
		MVEE_PREOP(ATOMIC_INCREMENT, mem, 1);	\
		orig_atomic_increment(mem);				\
		MVEE_POSTOP();							\
	})

#define atomic_increment_and_test(mem)						\
	({														\
		unsigned char ____result;							\
		MVEE_PREOP(ATOMIC_INCREMENT_AND_TEST, mem, 1);		\
		____result = orig_atomic_increment_and_test(mem);	\
		MVEE_POSTOP();										\
		____result;											\
	})

#define atomic_increment_val(mem)				\
	({											\
		typeof(*mem) ____result;				\
		MVEE_PREOP(ATOMIC_INCREMENT, mem, 1);	\
		____result = orig_atomic_increment_val(mem);	\
		MVEE_POSTOP();							\
		____result;								\
	})

#define atomic_decrement(mem)					\
	({											\
		MVEE_PREOP(ATOMIC_DECREMENT, mem, 1);	\
		orig_atomic_decrement(mem);				\
		MVEE_POSTOP();							\
	})

#define atomic_decrement_and_test(mem)						\
	({														\
		unsigned char ____result;							\
		MVEE_PREOP(ATOMIC_DECREMENT_AND_TEST, mem, 1);		\
		____result = orig_atomic_decrement_and_test(mem);	\
		MVEE_POSTOP();										\
		____result;											\
	})

#define atomic_decrement_val(mem)				\
	({											\
		typeof(*mem) ____result;				\
		MVEE_PREOP(ATOMIC_DECREMENT, mem, 1);	\
		____result = orig_atomic_decrement_val(mem);	\
		MVEE_POSTOP();							\
		____result;								\
	})

#define atomic_add_negative(mem, value)						\
	({														\
		unsigned char ____result;							\
		MVEE_PREOP(ATOMIC_ADD, mem, 1);						\
		____result = orig_atomic_add_negative(mem, value);	\
		MVEE_POSTOP();										\
		____result;											\
	})

#define atomic_add_zero(mem, value)						\
	({													\
		unsigned char ____result;						\
		MVEE_PREOP(ATOMIC_ADD_ZERO, mem, 1);			\
		____result = orig_atomic_add_zero(mem, value);	\
		MVEE_POSTOP();									\
		____result;										\
	})

#define atomic_bit_set(mem, bit)				\
	({											\
		MVEE_PREOP(ATOMIC_BIT_SET, mem, 1);		\
		orig_atomic_bit_set(mem, bit);			\
		MVEE_POSTOP();							\
	})

#define atomic_bit_test_set(mem, bit)						\
	({														\
		unsigned char ____result;							\
		MVEE_PREOP(ATOMIC_BIT_TEST_SET, mem, 1);			\
		____result = orig_atomic_bit_test_set(mem, bit);	\
		MVEE_POSTOP();										\
		____result;											\
	})

#define atomic_and(mem, mask)					\
	({											\
		MVEE_PREOP(ATOMIC_AND, mem, 1);			\
		orig_atomic_and(mem, mask);				\
		MVEE_POSTOP();							\
	})

#define atomic_or(mem, mask)					\
	({											\
		MVEE_PREOP(ATOMIC_OR, mem, 1);			\
		orig_atomic_or(mem, mask);				\
		MVEE_POSTOP();							\
	})

#define atomic_max(mem, value)					\
	({											\
		MVEE_PREOP(ATOMIC_MAX, mem, 1);			\
		orig_atomic_max(mem, value);			\
		MVEE_POSTOP();							\
	})

#define atomic_min(mem, value)					\
	({											\
		MVEE_PREOP(ATOMIC_MIN, mem, 1);			\
		orig_atomic_max(mem, value);			\
		MVEE_POSTOP();							\
	})

#define atomic_decrement_if_positive(mem)					\
	({														\
		__typeof(*mem) __result;							\
		MVEE_PREOP(ATOMIC_DECREMENT_IF_POSITIVE, mem, 1);	\
		__result = orig_atomic_decrement_if_positive(mem);	\
		MVEE_POSTOP();										\
		__result;											\
	})

#define atomic_and_val(mem, mask)							\
	({														\
		__typeof(*mem) __result;							\
		MVEE_PREOP(ATOMIC_AND_VAL, mem, 1);					\
		__result = orig_atomic_and_val(mem);				\
		MVEE_POSTOP();										\
		__result;											\
	})

#define atomic_or_val(mem, mask)							\
	({														\
		__typeof(*mem) __result;							\
		MVEE_PREOP(ATOMIC_OR_VAL, mem, 1);					\
		__result = orig_atomic_or_val(mem);					\
		MVEE_POSTOP();										\
		__result;											\
	})

#define atomic_forced_read(x)						\
	({												\
		typeof(x) ____result;						\
		MVEE_PREOP(ATOMIC_FORCED_READ, &x, 0);		\
		____result = orig_atomic_forced_read(x);	\
		MVEE_POSTOP();								\
		____result;									\
	})

#define catomic_compare_and_exchange_val_acq(mem, newval, oldval)		\
	({																	\
		typeof(*mem) ____result;										\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_VAL, mem, 1);			\
		____result = orig_catomic_compare_and_exchange_val_acq(mem, newval, oldval); \
		MVEE_POSTOP();													\
		____result;														\
	})

#define catomic_compare_and_exchange_val_rel(mem, newval, oldval)		\
	({																	\
		typeof(*mem) ____result;										\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_VAL, mem, 1);			\
		____result = orig_catomic_compare_and_exchange_val_rel(mem, newval, oldval); \
		MVEE_POSTOP();													\
		____result;														\
	})

#define catomic_compare_and_exchange_bool_acq(mem, newval, oldval)		\
	({																	\
		bool ____result;												\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_BOOL, mem, 1);			\
		____result = orig_catomic_compare_and_exchange_bool_acq(mem, newval, oldval); \
		MVEE_POSTOP();													\
		____result;														\
	})

#define catomic_compare_and_exchange_bool_rel(mem, newval, oldval)		\
	({																	\
		bool ____result;												\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_BOOL, mem, 1);			\
		____result = orig_catomic_compare_and_exchange_bool_rel(mem, newval, oldval); \
		MVEE_POSTOP();													\
		____result;														\
	})

#define catomic_exchange_and_add(mem, value)					\
	({															\
		typeof(*mem) ____result;								\
		MVEE_PREOP(CATOMIC_EXCHANGE_AND_ADD, mem, 1);			\
		____result = orig_catomic_exchange_and_add(mem, value);	\
		MVEE_POSTOP();											\
		____result;												\
	})

#define catomic_add(mem, value)					\
	({											\
		MVEE_PREOP(CATOMIC_ADD, mem, 1);		\
		orig_catomic_add(mem, value);			\
		MVEE_POSTOP();							\
	})

#define catomic_increment(mem)					\
	({											\
		MVEE_PREOP(CATOMIC_INCREMENT, mem, 1);	\
		orig_catomic_increment(mem);			\
		MVEE_POSTOP();							\
	})

#define catomic_increment_val(mem)						\
	({													\
		typeof(*mem) ____result;						\
		MVEE_PREOP(CATOMIC_INCREMENT, mem, 1);			\
		____result = orig_catomic_increment_val(mem);	\
		MVEE_POSTOP();									\
		____result;										\
	})

#define catomic_decrement(mem)					\
	({											\
		MVEE_PREOP(CATOMIC_DECREMENT, mem, 1);	\
		orig_catomic_decrement(mem);			\
		MVEE_POSTOP();							\
	})

#define catomic_decrement_val(mem)						\
	({													\
		typeof(*mem) ____result;						\
		MVEE_PREOP(CATOMIC_DECREMENT, mem, 1);			\
		____result = orig_catomic_decrement_val(mem);	\
		MVEE_POSTOP();									\
		____result;										\
	})


#define catomic_and(mem, mask)					\
	({											\
		MVEE_PREOP(CATOMIC_AND, mem, 1);		\
		orig_catomic_and(mem, mask);			\
		MVEE_POSTOP();							\
	})

#define catomic_or(mem, mask)					\
	({											\
		MVEE_PREOP(CATOMIC_OR, mem, 1);			\
		orig_catomic_or(mem, mask);				\
		MVEE_POSTOP();							\
	})

#define catomic_max(mem, value)					\
	({											\
		MVEE_PREOP(CATOMIC_MAX, mem, 1);		\
		orig_catomic_max(mem, value);			\
		MVEE_POSTOP();							\
	})


//
// generic C11-style atomics (include/atomic.h)
//
#define atomic_load_relaxed(mem)					\
	({												\
		__typeof(*mem) ____result;					\
		MVEE_PREOP(ATOMIC_LOAD, mem, 0);			\
		____result = orig_atomic_load_relaxed(mem);	\
		MVEE_POSTOP();								\
		____result;									\
	})

#define atomic_load_acquire(mem)					\
	({												\
		__typeof(*mem) ____result;					\
		MVEE_PREOP(ATOMIC_LOAD, mem, 0);			\
		____result = orig_atomic_load_acquire(mem);	\
		MVEE_POSTOP();								\
		____result;									\
	})

#define atomic_store_relaxed(mem, val)			\
	(void)({									\
		MVEE_PREOP(ATOMIC_STORE, mem, 1);		\
		orig_atomic_store_relaxed(mem, val);	\
		MVEE_POSTOP();							\
	})

#define atomic_store_release(mem, val)			\
	(void)({									\
		MVEE_PREOP(ATOMIC_STORE, mem, 1);		\
		orig_atomic_store_release(mem, val);	\
		MVEE_POSTOP();							\
	})

#define atomic_compare_exchange_weak_relaxed(mem, expected, desired)	\
	({																	\
		bool __result;													\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_BOOL, mem, 1);			\
		__result = orig_atomic_compare_exchange_weak_relaxed(mem, expected, desired); \
		MVEE_POSTOP();													\
		__result;														\
	})

#define atomic_compare_exchange_weak_acquire(mem, expected, desired)	\
	({																	\
		bool __result;													\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_BOOL, mem, 1);			\
		__result = orig_atomic_compare_exchange_weak_acquire(mem, expected, desired); \
		MVEE_POSTOP();													\
		__result;														\
	})

#define atomic_compare_exchange_weak_release(mem, expected, desired)	\
	({																	\
		bool __result;													\
		MVEE_PREOP(ATOMIC_COMPARE_AND_EXCHANGE_BOOL, mem, 1);			\
		__result = orig_atomic_compare_exchange_weak_release(mem, expected, desired); \
		MVEE_POSTOP();													\
		__result;														\
	})

#define atomic_exchange_relaxed(mem, desired)						\
	({																\
		typeof(*mem) ____result;									\
		MVEE_PREOP(ATOMIC_EXCHANGE, mem, 1);						\
		____result = orig_atomic_exchange_relaxed(mem, desired);	\
		MVEE_POSTOP();												\
		____result;													\
	})

#define atomic_exchange_acquire(mem, desired)						\
	({																\
		typeof(*mem) ____result;									\
		MVEE_PREOP(ATOMIC_EXCHANGE, mem, 1);						\
		____result = orig_atomic_exchange_acquire(mem, desired);	\
		MVEE_POSTOP();												\
		____result;													\
	})

#define atomic_exchange_release(mem, desired)						\
	({																\
		typeof(*mem) ____result;									\
		MVEE_PREOP(ATOMIC_EXCHANGE, mem, 1);						\
		____result = orig_atomic_exchange_release(mem, desired);	\
		MVEE_POSTOP();												\
		____result;													\
	})

#define atomic_fetch_add_relaxed(mem, operand)						\
	({																\
		typeof(*mem) ____result;									\
		MVEE_PREOP(ATOMIC_FETCH_ADD, mem, 1);						\
		____result = orig_atomic_fetch_add_relaxed(mem, operand);	\
		MVEE_POSTOP();												\
		____result;													\
	})

#define atomic_fetch_add_acquire(mem, operand)						\
	({																\
		typeof(*mem) ____result;									\
		MVEE_PREOP(ATOMIC_FETCH_ADD, mem, 1);						\
		____result = orig_atomic_fetch_add_acquire(mem, operand);	\
		MVEE_POSTOP();												\
		____result;													\
	})

#define atomic_fetch_add_release(mem, operand)						\
	({																\
		typeof(*mem) ____result;									\
		MVEE_PREOP(ATOMIC_FETCH_ADD, mem, 1);						\
		____result = orig_atomic_fetch_add_release(mem, operand);	\
		MVEE_POSTOP();												\
		____result;													\
	})

#define atomic_fetch_add_acq_rel(mem, operand)						\
	({																\
		typeof(*mem) ____result;									\
		MVEE_PREOP(ATOMIC_FETCH_ADD, mem, 1);						\
		____result = orig_atomic_fetch_add_acq_rel(mem, operand);	\
		MVEE_POSTOP();												\
		____result;													\
	})

#define atomic_fetch_and_relaxed(mem, operand)						\
	({																\
		typeof(*mem) ____result;									\
		MVEE_PREOP(ATOMIC_FETCH_AND, mem, 1);						\
		____result = orig_atomic_fetch_and_relaxed(mem, operand);	\
		MVEE_POSTOP();												\
		____result;													\
	})

#define atomic_fetch_and_acquire(mem, operand)						\
	({																\
		typeof(*mem) ____result;									\
		MVEE_PREOP(ATOMIC_FETCH_AND, mem, 1);						\
		____result = orig_atomic_fetch_and_acquire(mem, operand);	\
		MVEE_POSTOP();												\
		____result;													\
	})

#define atomic_fetch_and_release(mem, operand)						\
	({																\
		typeof(*mem) ____result;									\
		MVEE_PREOP(ATOMIC_FETCH_AND, mem, 1);						\
		____result = orig_atomic_fetch_and_release(mem, operand);	\
		MVEE_POSTOP();												\
		____result;													\
	})


#define atomic_fetch_or_relaxed(mem, operand)						\
	({																\
		typeof(*mem) ____result;									\
		MVEE_PREOP(ATOMIC_FETCH_OR, mem, 1);						\
		____result = orig_atomic_fetch_or_relaxed(mem, operand);	\
		MVEE_POSTOP();												\
		____result;													\
	})

#define atomic_fetch_or_acquire(mem, operand)						\
	({																\
		typeof(*mem) ____result;									\
		MVEE_PREOP(ATOMIC_FETCH_OR, mem, 1);						\
		____result = orig_atomic_fetch_or_acquire(mem, operand);	\
		MVEE_POSTOP();												\
		____result;													\
	})

#define atomic_fetch_or_release(mem, operand)						\
	({																\
		typeof(*mem) ____result;									\
		MVEE_PREOP(ATOMIC_FETCH_OR, mem, 1);						\
		____result = orig_atomic_fetch_or_release(mem, operand);	\
		MVEE_POSTOP();												\
		____result;													\
	})

#define atomic_fetch_xor_release(mem, operand)						\
	({																\
		typeof(*mem) ____result;									\
		MVEE_PREOP(ATOMIC_FETCH_XOR, mem, 1);						\
		____result = orig_atomic_fetch_xor_release(mem, operand);	\
		MVEE_POSTOP();												\
		____result;													\
	})

//
// TLS atomics (tls.h)
//
#define THREAD_ATOMIC_CMPXCHG_VAL(descr, member, newval, oldval)		\
	({																	\
		__typeof(descr->member) ____result;								\
		MVEE_PREOP(__THREAD_ATOMIC_CMPXCHG_VAL, &descr->member, 1);		\
		____result = orig_THREAD_ATOMIC_CMPXCHG_VAL(descr, member, newval, oldval); \
		MVEE_POSTOP();													\
		____result;														\
	})


#define THREAD_ATOMIC_AND(descr, member, val)					\
	(void)({													\
			MVEE_PREOP(__THREAD_ATOMIC_AND, &descr->member, 1);	\
			orig_THREAD_ATOMIC_AND(descr, member, val);			\
			MVEE_POSTOP();										\
		})


#define THREAD_ATOMIC_BIT_SET(descr, member, bit)					\
	(void)({														\
			MVEE_PREOP(__THREAD_ATOMIC_BIT_SET, &descr->member, 1);	\
			orig_THREAD_ATOMIC_BIT_SET(descr, member, bit);			\
			MVEE_POSTOP();											\
		})

//
// MVEE additions
//
#define THREAD_ATOMIC_GETMEM(descr, member)			\
	({												\
		__typeof(descr->member) ____result;			\
		MVEE_PREOP(ATOMIC_LOAD, &descr->member, 1);	\
		____result = THREAD_GETMEM(descr, member);	\
		MVEE_POSTOP();								\
		____result;									\
	})

#define THREAD_ATOMIC_SETMEM(descr, member, val)			\
	(void)({												\
			MVEE_PREOP(ATOMIC_STORE, &descr->member, 1);	\
			THREAD_SETMEM(descr, member, val);				\
			MVEE_POSTOP();									\
		})

//
// sys_futex with FUTEX_WAKE_OP usually overwrites the value of the futex.
// We have to make sure that we include the futex write in our sync buf ordering
//
#define lll_futex_wake_unlock(futexp, nr_wake, nr_wake2, futexp2, private) \
	({																	\
		INTERNAL_SYSCALL_DECL (__err);									\
		long int __ret;													\
		MVEE_PREOP(___UNKNOWN_LOCK_TYPE___, futexp2, 1);				\
		__ret = INTERNAL_SYSCALL (futex, __err, 6, (futexp),			\
								  __lll_private_flag (FUTEX_WAKE_OP, private), \
								  (nr_wake), (nr_wake2), (futexp2),		\
								  FUTEX_OP_CLEAR_WAKE_IF_GT_ONE);		\
		if (mvee_should_futex_unlock())									\
		{																\
			*futexp2 = 0;												\
		}																\
		MVEE_POSTOP();													\
		INTERNAL_SYSCALL_ERROR_P (__ret, __err);						\
	})

#endif // !IS_IN (rtld)

#endif /* atomic-machine.h */
