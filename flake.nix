{
  description = "Gokai - universal framework used by ExpidusOS";

  nixConfig = rec {
    trusted-public-keys = [ "cache.nixos.org-1:6NCHdD59X431o0gWypbMrAURkbJ16ZPMQFGspcDShjY=" "cache.garnix.io:CTFPyKSLcx5RMJKfLo5EEPUObbA78b0YQ2DTCJXqr9g=" ];
    substituters = [ "https://cache.nixos.org" "https://cache.garnix.io" ];
    trusted-substituters = substituters;
    fallback = true;
    http2 = false;
  };

  inputs.expidus-sdk = {
    url = github:ExpidusOS/sdk;
    inputs.nixpkgs.follows = "nixpkgs";
  };

  inputs.nixpkgs.url = github:ExpidusOS/nixpkgs;

  outputs = { self, expidus-sdk, nixpkgs }@inputs:
    with expidus-sdk.lib;
    flake-utils.eachSystem flake-utils.allSystems (system:
      let
        pkgs = expidus-sdk.legacyPackages.${system};

        flutter-engine = pkgs.fetchzip {
          url = "https://storage.googleapis.com/flutter_infra_release/flutter/45f6e009110df4f34ec2cf99f63cf73b71b7a420/linux-x64/linux-x64-embedder.zip";
          stripRoot = false;
          sha256 = "sha256-WjCJzoTUF/Po2JMS4T2Z/SGQA/c/AZDik5kg4KHFGHM=";
        };
      in {
        packages = {
          sdk = pkgs.stdenv.mkDerivation {
            pname = "gokai-sdk";
            version = "0.1.0-git+${self.shortRev or "dirty"}";

            src = "${cleanSource self}/packages/gokai_sdk";

            mesonFlags = [
              "-Dflutter-engine=${flutter-engine}"
            ];

            nativeBuildInputs = with pkgs; [
              wayland-scanner
              meson
              ninja
              pkg-config
            ];

            buildInputs = with pkgs; [
              libxdg_basedir
              libuv
              glib
              packagekit
              wlroots
              wayland
              spdlog
              yaml-cpp
              udev
              glm
            ];
          };
        };

        devShells = {
          default = pkgs.mkShell {
            name = "gokai";

            packages = with pkgs; [
              flutter
              pkgs.wayland
              pkg-config
              self.packages.${system}.sdk
              gdb
            ] ++ self.packages.${system}.sdk.buildInputs;
          };

          sdk = pkgs.mkShell {
            name = "gokai-sdk";

            inherit (self.packages.${system}.sdk) mesonFlags;

            packages = self.packages.${system}.sdk.nativeBuildInputs
              ++ self.packages.${system}.sdk.buildInputs;
          };
        };
      });
}
