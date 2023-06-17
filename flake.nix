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

            buildInputs = with pkgs; [
              libxdg_basedir
              libuv
              appstream
              glib
              packagekit
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
              libxdg_basedir
              libuv
              appstream
              glib
              packagekit
            ];
          };
        };
      });
}
