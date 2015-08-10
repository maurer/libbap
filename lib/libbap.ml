open Core_kernel.Std
open Bap.Std

let bitvector_of_int64 width v = Bitvector.of_int64 ~width v

let _ = Callback.register "bigstring_of_string" Bigstring.of_string
let _ = Callback.register "mem_create" Memory.create
let _ = Callback.register "disassemble_mem" disassemble
let _ = Callback.register "bitvector_of_int64" bitvector_of_int64
let _ = Callback.register "bitvector_to_string" Bitvector.to_string
