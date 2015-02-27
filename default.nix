{ pkgs ? (import <nixpkgs> {}) }:
let
  inherit (pkgs) stdenv readline;
  wxGTK = pkgs.wxGTK30;
  libX11 = pkgs.xlibs.libX11;
  bin2c = stdenv.mkDerivation {
    name = "bossa-bin2c";
    src = ./bin2c.c;
    unpackPhase = "true";
    buildPhase = ''cc $src -o bin2c'';
    installPhase = ''mkdir -p $out/bin; cp bin2c $out/bin/'';
  };
in
stdenv.mkDerivation rec {
  name = "bossa-dev";
  src = ./.;
  patches = [ ./bossa-no-applet-build.patch ];
  nativeBuildInputs = [ bin2c ];
  buildInputs = [ wxGTK libX11 readline ];

  makeFlags = [ "bin/bossac" "bin/bossash" "bin/bossa" ];

  installPhase = ''
    mkdir -p $out/bin
    cp bin/bossa{c,sh,} $out/bin/
  '';
}
