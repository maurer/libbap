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
let disassemble_mem root_arr arch mem =
  disassemble ~roots:(Array.to_list root_arr) arch mem

let disasm_to_string d =
  Seq.fold (Disasm.insns d) ~init:"" ~f:(fun s (_, insn) ->
    s ^ (Insn.to_string insn) ^ "\n")
let disasm_to_insns d =
  Array.of_list (List.rev (Seq.fold (Disasm.insns d) ~init:[]
    ~f:(fun s (mem, insn) ->
      (Memory.min_addr mem, Memory.max_addr mem, insn)::s)))

let size_to_bits (sz : Size.t) : int =
  match sz with
    | `r8 -> 8
    | `r16 -> 16
    | `r32 -> 32
    | `r64 -> 64

let _ = Callback.register "bigstring_of_string" bigstring_of_string
let _ = Callback.register "bigstring_to_string" bigstring_to_string
let _ = Callback.register "mem_create" mem_create
let _ = Callback.register "disassemble_mem" disassemble_mem
let _ = Callback.register "bitvector_of_int64" bitvector_of_int64
let _ = Callback.register "bitvector_to_string" Bitvector.to_string
let _ = Callback.register "mem_to_string" Memory.to_string
let _ = Callback.register "disasm_to_string" disasm_to_string
let _ = Callback.register "disasm_to_insns" disasm_to_insns
let _ = Callback.register "insn_asm" Insn.asm
let _ = Callback.register "insn_to_bils" Insn.bil
let _ = Callback.register "array_of_list" Array.of_list
let _ = Callback.register "size_to_bits" size_to_bits
let _ = Thread.yield ()
