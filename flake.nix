{
  description = "why";
  inputs = {
    flake-compat = {
      url = "github:edolstra/flake-compat";
      flake = false;
    };
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    imanimate = {
      url = "github:RaidcoreGG/ImAnimate";
      flake = false;
    };
    mumble-api = {
      url = "github:RaidcoreGG/RCGG-lib-mumble-api";
      flake = false;
    };
    /*nexus-translations = {
      url = "github:RaidcoreGG/Nexus-Translations";
      flake = false;
    };*/
    flake-utils = {
      url = "github:numtide/flake-utils";
    };
  };

  outputs = inputs @ { self, rust, flake-utils, nixpkgs, ... }: let
    nixlib = nixpkgs.lib;
    shell = {
      mkShell
    , cmake
    , pkg-config
    , nlohmann_json
    , openssl
    , httplib
    , stb
    , mcfgthreads ? windows.mcfgthreads
    , pthreads ? windows.pthreads
    , windows
    }: mkShell {
      nativeBuildInputs = [ cmake pkg-config ];
      buildInputs = [
        nlohmann_json openssl httplib stb
        #pthreads
        mcfgthreads
      ];
    };
    ImAnimate = {
      stdenv
    , source ? inputs.imanimate
    }: stdenv.mkDerivation {
      pname = "ImAnimate";
    };
  in flake-utils.lib.eachDefaultSystem (system: let
    legacyPackages = self.legacyPackages.${system};
    packages = self.packages.${system};
    pkgs = nixpkgs.legacyPackages.${system};
    pkgs-w64 =
      if pkgs.hostPlatform.isWindows then pkgs
      else pkgs.pkgsCross.mingwW64;
  in {
    devShells = {
      default = pkgs-w64.callPackage shell {
        cmake = pkgs-w64.buildPackages.cmake;
      };
    };

    legacyPackages = {
      inherit pkgs pkgs-w64;
    };

    packages = {
      default = packages.nexus;
      /*ImAnimate = pkgs-w64.callPackage ImAnimate {
        source = inputs.imanimate;
      };*/
      nexus = pkgs-w64.callPackage ./derivation.nix {
        source = self.outPath;
        ImAnimateSource = inputs.imanimate;
        MumbleApiSource = inputs.mumble-api;
        #NexusTranslations = inputs.nexus-translations;
      };
    };
  }) // {
    lib = {
      version = "0.0.0";
    };
  };
}

