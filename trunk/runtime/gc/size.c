/* Copyright (C) 1999-2008 Henry Cejtin, Matthew Fluet, Suresh
 *    Jagannathan, and Stephen Weeks.
 * Copyright (C) 1997-2000 NEC Research Institute.
 *
 * MLton is released under a BSD-style license.
 * See the file MLton-LICENSE for details.
 */

size_t GC_sizeOfObject (GC_state s, pointer root) {
  size_t res;

  //XXX KC : Is this correct?
  //enter (s); /* update stack in heap, in case it is reached */
  s->syncReason = SYNC_FORCE;
  ENTER0 (s); /* update stack in heap, in case it is reached */

  if (DEBUG_SIZE)
    fprintf (stderr, "GC_sizeOfObject marking\n");
  res = dfsMarkByMode (s, root, MARK_MODE, FALSE, FALSE);
  if (DEBUG_SIZE)
    fprintf (stderr, "GC_sizeOfObject unmarking\n");
  dfsMarkByMode (s, root, UNMARK_MODE, FALSE, FALSE);
  LEAVE0 (s);

  return res;
}
