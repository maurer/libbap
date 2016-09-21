open Core_kernel.Std
open Bap.Std
open Bap_plugins.Std

let bitvector_of_int64 width v = Bitvector.of_int64 ~width v
let bigstring_of_string pos len s = Bigstring.of_string ~pos ~len s
let bigstring_to_string s = Bigstring.to_string s
let mem_create pos len endian addr bs =
  let mem_err = Memory.create ~pos ~len endian addr bs in
  match mem_err with
      Ok(v) -> v
    | _     -> failwith "mem_create failed"
let disassemble_mem root_arr arch mem =
  ok_exn (Disasm.of_mem ~rooter:(Rooter.create (Array.to_sequence root_arr)) arch mem)

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
    | `r128 -> 128
    | `r256 -> 256

let bv_to_bytes bv =
  Sequence.to_array (Bitvector.enum_chars bv LittleEndian)

let image_of_string buf =
  match Image.of_string buf with
    | Ok((v,_)) -> v
    | _ -> failwith "Image failed to lift"

let rec table_to_list ?(e=None) tbl =
  match e with
    | None -> (match Table.min tbl with
                 | Some (m, v) -> table_to_list ~e:(Some (m, v)) tbl
                 | None -> [])
    | Some (m, v) -> (match Table.next tbl m with
                     | Some (m', v') -> (m, v)::(table_to_list ~e:(Some (m', v')) tbl)
                     | None -> [(m, v)])

let segments_of_image img =
  Table.map (Image.segments img) ~f:(fun seg ->
    (Image.Segment.name seg,
     Image.Segment.is_readable seg,
     Image.Segment.is_writable seg,
     Image.Segment.is_executable seg)) |> table_to_list

let file_contents_to_raw_segments str =
  str |> image_of_string |> segments_of_image |> List.map ~f:(fun (mem, segd) ->
    (segd, mem)) |>
    Array.of_list

let byteweight (arch : arch) (mem : mem) : addr Array.t =
  let module BW = Bap_byteweight.Bytes in
  let osigs = Bap_byteweight_signatures.load ~mode:"bytes" arch in
  let sigs = Option.value_exn (Result.ok osigs) in
  let parsed = Binable.of_string (module BW) sigs in
  Array.of_list (BW.find parsed mem ~length:16 ~threshold:0.5)

let file_contents_to_symbols (str : string) : (string * bool * bool * addr * addr) Array.t =
  let module Symbol = Image.Symbol in
  str |> image_of_string |> Image.symbols |> table_to_list |> List.map ~f:(fun(mem, sym) ->
  (Symbol.name sym, Symbol.is_function sym, Symbol.is_debug sym, Memory.min_addr mem, Memory.max_addr mem)) |> Array.of_list

let mem_project (mem : mem) : (addr * addr * string) = (Memory.min_addr mem, Memory.max_addr mem, Bigsubstring.to_string (Memory.to_buffer mem))

let parse_arch (contents : string) : arch =
  Image.arch (image_of_string contents)

let arch_to_string (arch : arch) : string =
  Format.asprintf "%a" Arch.pp arch

let plugin_init () =
  Log.start ();
  Plugins.run ~don't_setup_handlers:true ()

let _ = Callback.register "mem_project" mem_project
let _ = Callback.register "get_symbols" file_contents_to_symbols
let _ = Callback.register "byteweight" byteweight
let _ = Callback.register "get_segments" file_contents_to_raw_segments
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
let _ = Callback.register "bv_size" Bitvector.bitwidth
let _ = Callback.register "bv_contents" bv_to_bytes
let _ = Callback.register "insn_is_call" (Insn.is Insn.call)
let _ = Callback.register "parse_arch" parse_arch
let _ = Callback.register "arch_to_string" arch_to_string
let _ = Callback.register "plugin_init" plugin_init
let _ = Thread.yield ()
