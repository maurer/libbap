open Core_kernel.Std
open Bap.Std

let bitvector_of_int64 width v = Bitvector.of_int64 ~width v
let bigstring_of_string pos len s = Bigstring.of_string ~pos ~len s
let bigstring_to_string s = Bigstring.to_string s
let mem_create pos len endian addr bs =
  let mem_err = Memory.create ~pos ~len endian addr bs in
  match mem_err with
      Ok(v) -> v
    | _     -> failwith "mem_create failed"

let _ = Callback.register "bigstring_of_string" bigstring_of_string
let _ = Callback.register "bigstring_to_string" bigstring_to_string
let _ = Callback.register "mem_create" mem_create
let _ = Callback.register "disassemble_mem" disassemble
let _ = Callback.register "bitvector_of_int64" bitvector_of_int64
let _ = Callback.register "bitvector_to_string" Bitvector.to_string
let _ = Callback.register "mem_to_string" Memory.to_string
