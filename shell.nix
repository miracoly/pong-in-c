{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
    buildInputs = with pkgs.buildPackages; [
        gnumake
        gcc
        SDL_ttf
        SDL2.dev
        SDL2_ttf
    ];

    CPATH = pkgs.lib.makeSearchPath "include/SDL2" [ pkgs.SDL2.dev ];
}
