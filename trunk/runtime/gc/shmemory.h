/* Copyright (C) 1999-2005 Henry Cejtin, Matthew Fluet, Suresh
 *    Jagannathan, and Stephen Weeks.
 * Copyright (C) 1997-2000 NEC Research Institute.
 *
 * MLton is released under a BSD-style license.
 * See the file MLton-LICENSE for details.
 */

#if (defined (MLTON_GC_INTERNAL_FUNCS))

PRIVATE void* GC_shmalloc (size_t size);
PRIVATE void* GC_mpbmalloc (size_t size);

#endif /* (defined (MLTON_GC_INTERNAL_BASIS)) */
