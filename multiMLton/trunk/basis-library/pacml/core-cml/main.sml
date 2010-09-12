structure Main =
struct

  open Critical

  structure S = Scheduler
  structure SQ = SchedulerQueues
  structure SH = SchedulerHooks
  structure TO = Timeout
  structure TID = ThreadID
  structure MT = MLtonThread
  structure PT = ProtoThread

  structure Assert = LocalAssert(val assert = false)
  structure Debug = LocalDebug(val debug = false)

  fun debug msg = Debug.sayDebug ([atomicMsg, TID.tidMsg], msg)
  fun debug' msg = debug (fn () => msg^" : "^Int.toString(PacmlFFI.processorNumber()))



  (* Enabling preemption for processor 0
   * For other processors, preemprtion is enabled when they are scheduled
   *)
  val _ = PacmlFFI.enablePreemption ()

  (* Dummy signal handler *)
  (* Since signals are handled by a separate pthread, the working threads are
  * not interrupted and thus failing to interrupt the syscalls that could be
  * restarted. Hence, the signal handler thread sends SIGUSR2 to each worker
  * thread, which corresponds to this handler *)

  (* XXX DEBUG *)
  val h = MLtonSignal.Handler.handler (fn t => t)
  (* Install handler for processor 0*)
  val _ = MLtonSignal.setHandler (Posix.Signal.usr2, h)

  fun thread_main () =
  let
    val _ = MLtonProfile.init ()
    val _ = PacmlFFI.disablePreemption ()
    val _ = MLtonSignal.setHandler (Posix.Signal.usr2, h)
    fun loop () =
    let
      val _ = PacmlFFI.maybeWaitForGC ()
    in
      case SQ.deque (RepTypes.PRI) of
           NONE => (PacmlFFI.wait (); loop ())
         | SOME (t) =>
             let
               val _ = if !Config.isRunning then ()
                       else loop ()
               val _ = atomicBegin ()
               val _ = PacmlFFI.enablePreemption ()
             in
               S.atomicSwitch (fn _ => t)
             end
    end
  in
    loop ()
  end

  val () = (_export "Parallel_run": (unit -> unit) -> unit;) thread_main

  fun alrmHandler thrd =
    let
      val () = Assert.assertAtomic' ("RunCML.alrmHandler", NONE)
      val () = debug' "alrmHandler" (* Atomic 1 *)
      val () = Assert.assertAtomic' ("RunCML.alrmHandler", SOME 1)
      val () = S.preempt thrd
      val () = if (PacmlFFI.processorNumber () = 0) then ignore (TO.preempt ())
                else ()
      val nextThrd = S.next()
    in
      nextThrd
    end

  fun pauseHook (iter) =
    let
      (* If there are waiting time events, then make proc 0 spin *)
      val to = TO.preempt ()

      val iter = case to of
                      NONE => if (iter > Config.maxIter) then (PacmlFFI.wait (); iter-1) else iter
                    | _ => (iter - 1)
      val () = if not (!Config.isRunning) then (atomicEnd ();ignore (SchedulerHooks.deathTrap())) else ()
    in
      S.nextWithCounter (iter + 1)
    end

  fun reset running =
      (S.reset running
      ; SH.reset ()
      ; TID.reset ()
      ; TO.reset ())


  fun run (initialProc : unit -> unit) =
  let
    val installAlrmHandler = fn (h) => MLtonSignal.setHandler (Posix.Signal.alrm, h)
    val status =
        S.switchToNext
        (fn thrd =>
        let
          val () = reset true
          val () = SH.shutdownHook := PT.prepend (thrd, fn arg => (atomicBegin (); arg))
          val () = SH.pauseHook := pauseHook
          val () = ignore (Thread.spawn (fn ()=> (Config.isRunning := true;initialProc ())))
          val handler = MLtonSignal.Handler.handler (S.unwrap alrmHandler Thread.reifyHostFromParasite)
          val () = installAlrmHandler handler
          (* Spawn the Non-blocking worker threads *)
          (* val _ = List.tabulate (numIOThreads * 5, fn _ => NonBlocking.mkNBThread ()) *)
        in
            ()
        end)
      val () = reset false
      val () = Config.isRunning := false
      val () = atomicEnd ()
    in
      status
    end

end
