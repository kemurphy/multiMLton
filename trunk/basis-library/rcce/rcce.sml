structure MLtonRcce : MLTON_RCCE =
struct
  fun send (v, coreId) =
    Primitive.dontInline (fn () => Primitive.MLton.Rcce.send (v, coreId))

  fun recv (coreId) =
    Primitive.dontInline (fn () => Primitive.MLton.Rcce.recv (coreId))

  val wtime = _import "RCCE_wtime": unit -> real;
end