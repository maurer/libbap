open Core_kernel.Std
open Bap.Std
;;

Callback.register "disassemble" disassemble;;
Callback.register "bigstring_of_string" Bigstring.of_string;;
