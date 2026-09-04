{
  description = "C++ development flake for the netlib project";

  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    inputs@{ flake-parts, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "aarch64-darwin"
        "x86_64-darwin"
      ];
      perSystem =
        {
          config,
          self',
          inputs',
          pkgs,
          system,
          ...
        }:
        {
          devShells.default = pkgs.mkShell.override { stdenv = pkgs.gcc16Stdenv; } {
            # this ensures that we have gcc16, which we need for C++26. We do not need to do any additional boostrapping

            nativeBuildInputs = with pkgs; [
              cmake
              ninja
              gdb # debugger that i use with emacs
              clang-tools # clang-format, tidy
            ];

            buildInputs = with pkgs; [
              gtest # my unit testing framework of choice
              # i have used both Catch2 and doctest. I choose gtest this time for the
              # sake of getting some hands-on experience with a new framework
            ];
          };
        };
    };
}
