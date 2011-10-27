/* Copyright (C) 1999-2008 Henry Cejtin, Matthew Fluet, Suresh
 *    Jagannathan, and Stephen Weeks.
 * Copyright (C) 1997-2000 NEC Research Institute.
 *
 * MLton is released under a BSD-style license.
 * See the file MLton-LICENSE for details.
 */

#ifndef _COMMON_MAIN_H_
#define _COMMON_MAIN_H_

#include "mlton-main.h"

#define MLTON_GC_INTERNAL_TYPES
#define MLTON_GC_INTERNAL_BASIS
#include "platform.h"

/* The label must be declared as weak because gcc's optimizer may prove that
 * the code that declares the label is dead and hence eliminate the declaration.
 */
#define DeclareProfileLabel(l)                  \
        extern char l __attribute__ ((weak))

#define BeginIntInfInits static struct GC_intInfInit intInfInits[] = {
#define IntInfInitElem(g, n) { g, n },
#define EndIntInfInits };

#define BeginVectorInits static struct GC_vectorInit vectorInits[] = {
#define VectorInitElem(a, b, c, d) { (pointer)a, b, c, d },
#define EndVectorInits };

#define LoadArray(a, f) if (fread (a, sizeof(*a), cardof(a), f) != cardof(a)) return -1;
#define SaveArray(a, f) if (fwrite(a, sizeof(*a), cardof(a), f) != cardof(a)) return -1;

PRIVATE Pointer gcStateAddress = NULL;
#define Initialize(s, al, mg, mfs, mmc, pk, ps, gnr)              \
        gcStateAddress = (pointer)&s;                             \
        s.alignment = al;                                         \
        s.atMLtons = atMLtons;                                    \
        s.atMLtonsLength = cardof(atMLtons);                      \
        s.frameLayouts = frameLayouts;                            \
        s.frameLayoutsLength = cardof(frameLayouts);              \
        s.globals = (objptr*)globalObjptr;                        \
        s.globalsLength = globalsLength;                          \
        s.intInfInits = intInfInits;                              \
        s.intInfInitsLength = cardof(intInfInits);                \
        s.loadGlobals = loadGlobals;                              \
        s.magic = mg;                                             \
        s.maxFrameSize = mfs;                                     \
        s.mutatorMarksCards = mmc;                                \
        s.objectTypes = objectTypes;                              \
        s.objectTypesLength = cardof(objectTypes);                \
        s.returnAddressToFrameIndex = returnAddressToFrameIndex;  \
        s.saveGlobals = saveGlobals;                              \
        s.vectorInits = vectorInits;                              \
        s.vectorInitsLength = cardof(vectorInits);                \
        s.sourceMaps.frameSources = frameSources;                 \
        s.sourceMaps.frameSourcesLength = cardof(frameSources);   \
        s.sourceMaps.sourceLabels = sourceLabels;                 \
        s.sourceMaps.sourceLabelsLength = cardof(sourceLabels);   \
        s.sourceMaps.sourceNames = sourceNames;                   \
        s.sourceMaps.sourceNamesLength = cardof(sourceNames);     \
        s.sourceMaps.sourceSeqs = sourceSeqs;                     \
        s.sourceMaps.sourceSeqsLength = cardof(sourceSeqs);       \
        s.sourceMaps.sources = sources;                           \
        s.sourceMaps.sourcesLength = cardof(sources);             \
        s.profiling.kind = pk;                                    \
        s.profiling.stack = ps;                                   \
        s.profiling.isProfilingTimeOn = false;                    \
        s.globalObjptrNonRoot = (Pointer *) RCCE_shmalloc (gnr * sizeof (Pointer));  \
        MLton_init (argc, argv, &s);                              \

#define LIB_PASTE(x,y) x ## y
#define LIB_OPEN(x) LIB_PASTE(x, _open)
#define LIB_CLOSE(x) LIB_PASTE(x, _close)

static void MLton_callFromC ();

void Duplicate (GC_state d, GC_state s) {
  gcStateAddress = (pointer)d;
  d->alignment = s->alignment;
  d->atMLtons = s->atMLtons;
  d->atMLtonsLength = s->atMLtonsLength;
  d->frameLayouts = s->frameLayouts;
  d->frameLayoutsLength = s->frameLayoutsLength;
  d->globals = s->globals;
  d->globalsLength = s->globalsLength;
  d->intInfInits = s->intInfInits;
  d->intInfInitsLength = s->intInfInitsLength;
  d->loadGlobals = s->loadGlobals;
  d->magic = s->magic;
  d->maxFrameSize = s->maxFrameSize;
  d->mutatorMarksCards = s->mutatorMarksCards;
  d->objectTypes = s->objectTypes;
  d->objectTypesLength = s->objectTypesLength;
  d->returnAddressToFrameIndex = s->returnAddressToFrameIndex;
  d->saveGlobals = s->saveGlobals;
  d->vectorInits = s->vectorInits;
  d->vectorInitsLength = s->vectorInitsLength;
  d->sourceMaps.frameSources = s->sourceMaps.frameSources;
  d->sourceMaps.frameSourcesLength = s->sourceMaps.frameSourcesLength;
  d->sourceMaps.sourceLabels = s->sourceMaps.sourceLabels;
  d->sourceMaps.sourceLabelsLength = s->sourceMaps.sourceLabelsLength;
  d->sourceMaps.sourceNames = s->sourceMaps.sourceNames;
  d->sourceMaps.sourceNamesLength = s->sourceMaps.sourceNamesLength;
  d->sourceMaps.sourceSeqs = s->sourceMaps.sourceSeqs;
  d->sourceMaps.sourceSeqsLength = s->sourceMaps.sourceSeqsLength;
  d->sourceMaps.sources = s->sourceMaps.sources;
  d->sourceMaps.sourcesLength = s->sourceMaps.sourcesLength;
  d->profiling.kind = s->profiling.kind;
  d->profiling.stack = s->profiling.stack;
  d->profiling.isProfilingTimeOn = false;
  d->globalObjptrNonRoot = s->globalObjptrNonRoot;
  GC_duplicate (d, s);
}

#endif /* #ifndef _COMMON_MAIN_H_ */
