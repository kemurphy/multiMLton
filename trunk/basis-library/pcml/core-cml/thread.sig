(* thread.sig
 * 2004 Matthew Fluet (mfluet@acm.org)
 *  Ported to MLton threads.
 *)

(* threads-sig.sml
 *
 * COPYRIGHT (c) 1995 AT&T Bell Laboratories.
 * COPYRIGHT (c) 1989-1991 John H. Reppy
 *)

signature THREAD =
  sig
     include THREAD_ID
     val getTid : unit -> thread_id

     (* XXXGC KC must change to (unit->unit) -> thread_id *)
     val spawn' : (unit -> unit) -> unit
     val spawn  : (unit -> unit) -> thread_id
     val spawnOnCurProc : (unit -> unit) -> thread_id
     val exit   : unit -> 'a
     val yield  : unit -> unit  (* mostly for benchmarking *)
     val enablePreemption : unit -> unit
     val disablePreemption : unit -> unit

     val joinEvt : thread_id -> unit EventType.event

     (* thread-local data *)
     val newThreadProp : (unit -> 'a) ->
        {
         clrFn : unit -> unit,          (* clear's current thread's property *)
         getFn : unit -> 'a,            (* get current thread's property; if *)
                                        (* the property is not defined, then *)
                                        (* it sets it using the initialization *)
                                        (* function. *)
         peekFn : unit -> 'a option,    (* return the property's value, if any *)
         setFn : 'a -> unit             (* set the property's value for the *)
                                        (* current thread. *)
         }
     val newThreadFlag : unit -> {getFn : unit -> bool, setFn : bool -> unit}
  end

