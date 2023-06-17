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

        deps = mapAttrs (name: src: pkgs.stdenvNoCC.mkDerivation {
            inherit name src;

            dontConfigure = true;
            dontBuild = true;

            installPhase = ''
              cp -r --no-preserve=ownership,mode ${src.outPath} $out
              cp ${cleanSource self}/packages/gokai_sdk/third_party/${name}/BUILD $out/BUILD
              cp ${cleanSource self}/packages/gokai_sdk/third_party/${name}/WORKSPACE $out/WORKSPACE

              cd $out
              for p in ${cleanSource self}/packages/gokai_sdk/third_party/${name}/*.patch; do
                patch -p0 -i $p
              done
            '';
          }) (removeAttrs inputs [ "nixpkgs" "expidus-sdk" "self" ]);
      in {
        packages = {
          sdk = pkgs.buildBazelPackage {
            pname = "gokai-sdk";
            version = "0.1.0-git+${self.shortRev or "dirty"}";

            src = "${cleanSource self}/packages/gokai_sdk";

            bazel = pkgs.bazel;

            bazelTargets = [
              "//core:gokai-core"
              "//framework:gokai-framework"
            ];

            bazelFlags = attrValues (mapAttrs (name: pkg: "--override_repository=${name}=${pkg}") deps);

            fetchAttrs = {
              sha256 = "sha256-c6vLNiK54I8b0/K9/ikxp8VQfeZdOjCjlM60lS71l30=";
            };

            buildAttrs = {
              installPhase = ''
                install -Dm755 bazel-bin/core/libgokai-core.a $out/lib/libgokai-core.a
                install -Dm755 bazel-bin/core/libgokai-core.so $out/lib/libgokai-core.so
                install -Dm755 bazel-bin/framework/linux/libgokai-framework.so $out/lib/libgokai-framework.so
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
