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

  outputs = { self, expidus-sdk, nixpkgs }:
    with expidus-sdk.lib;
    flake-utils.eachSystem flake-utils.allSystems (system:
      let
        pkgs = expidus-sdk.legacyPackages.${system};
      in {
        packages = {
          sdk = pkgs.buildBazelPackage {
            pname = "gokai-sdk";
            version = "0.1.0-git+${self.shortRev or "dirty"}";

            src = "${cleanSource self}/packages/gokai_sdk";

            bazel = pkgs.bazel;

            bazelTargets = [
              "//core:gokai-core"
            ];

            fetchAttrs = {
              sha256 = "sha256-tMiZsX5OyVw6VVImn4nWwEFdBw+TKKbE7oACibQC0oA=";
            };

            buildAttrs = {
              installPhase = ''
                install -Dm755 bazel-bin/core/libgokai-core.a $out/lib/libgokai-core.a
                install -Dm755 bazel-bin/core/libgokai-core.so $out/lib/libgokai-core.so
              '';
            };
          };
        };

        devShells = {
          default = pkgs.mkShell {
            name = "gokai";

            packages = with pkgs; [
              flutter
              wayland
              pkg-config
            ];
          };

          sdk = pkgs.mkShell {
            name = "gokai-sdk";

            packages = with pkgs; [
              bazel
              pkg-config
            ];
          };
        };
      });
}
