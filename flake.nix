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

  outputs = { self, expidus-sdk, nixpkgs, expat, libffi, wayland, }@inputs:
    with expidus-sdk.lib;
    flake-utils.eachSystem flake-utils.allSystems (system:
      let
        pkgs = expidus-sdk.legacyPackages.${system};

        expat = pkgs.stdenvNoCC.mkDerivation {
          name = "expat-2.5.0";

          src = inputs.expat;

          dontConfigure = true;
          dontBuild = true;

          installPhase = ''
            cp -r --no-preserve=ownership,mode ${inputs.expat.outPath} $out
            cp ${cleanSource self}/packages/gokai_sdk/third_party/expat/BUILD $out/BUILD
            cp ${cleanSource self}/packages/gokai_sdk/third_party/expat/WORKSPACE $out/WORKSPACE
          '';
        };

        libffi = pkgs.stdenvNoCC.mkDerivation {
          name = "libffi-3.4.4";

          src = inputs.libffi;

          dontConfigure = true;
          dontBuild = true;

          installPhase = ''
            cp -r --no-preserve=ownership,mode ${inputs.libffi.outPath} $out
            cp ${cleanSource self}/packages/gokai_sdk/third_party/libffi/BUILD $out/BUILD
            cp ${cleanSource self}/packages/gokai_sdk/third_party/libffi/WORKSPACE $out/WORKSPACE
          '';
        };

        wayland = pkgs.stdenvNoCC.mkDerivation {
          name = "wayland-1.22.0";

          src = inputs.wayland;

          dontConfigure = true;
          dontBuild = true;

          installPhase = ''
            cp -r --no-preserve=ownership,mode ${inputs.wayland.outPath} $out
            cp ${cleanSource self}/packages/gokai_sdk/third_party/wayland/BUILD $out/BUILD
            cp ${cleanSource self}/packages/gokai_sdk/third_party/wayland/WORKSPACE $out/WORKSPACE
            cd $out
            patch -p0 -i ${cleanSource self}/packages/gokai_sdk/third_party/wayland/remove-config-os.patch
            patch -p0 -i ${cleanSource self}/packages/gokai_sdk/third_party/wayland/remove-config-shm.patch
          '';
        };
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

            bazelFlags = [
              "--override_repository=expat=${expat}"
              "--override_repository=libffi=${libffi}"
              "--override_repository=wayland=${wayland}"
            ];

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
