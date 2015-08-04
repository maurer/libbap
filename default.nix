{ pkgs ? (import <local> {}).pkgs }:

with pkgs;
with ocamlPackages;
let callPackage = newScope ocamlPackages;
in callPackage (import ./package.nix) { }
