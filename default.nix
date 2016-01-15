{ pkgs ? (import <local> {}).pkgs }:

with pkgs;
lib.camlCall ./package.nix { }
