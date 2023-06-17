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

  inputs.expat = {
    url = "https://github.com/libexpat/libexpat/releases/download/R_2_5_0/expat-2.5.0.tar.gz";
    flake = false;
  };

  inputs.libffi = {
    url = "https://github.com/libffi/libffi/releases/download/v3.4.4/libffi-3.4.4.tar.gz";
    flake = false;
  };

  inputs.wayland = {
    url = "https://gitlab.freedesktop.org/wayland/wayland/-/archive/1.22.0/wayland-1.22.0.tar.gz";
    flake = false;
  };

  outputs = { self, expidus-sdk, nixpkgs, expat, libffi, wayland }@inputs:
    with expidus-sdk.lib;
    flake-utils.eachSystem flake-utils.allSystems (system:
      let
        pkgs = expidus-sdk.legacyPackages.${system};
      in {
        packages = {
          sdk = pkgs.stdenv.mkDerivation {
            pname = "gokai-sdk";
            version = "0.1.0-git+${self.shortRev or "dirty"}";

            src = "${cleanSource self}/packages/gokai_sdk";

            nativeBuildInputs = with pkgs; [
              wayland-scanner
              meson
              ninja
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
            ];
          };

          sdk = pkgs.mkShell {
            name = "gokai-sdk";

            packages = with pkgs; [
              meson
              pkg-config
              ninja
            ];
          };
        };
      });
}
