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

        depot_tools = pkgs.fetchgit {
          url = "https://chromium.googlesource.com/chromium/tools/depot_tools.git";
          rev = "25cf78395cd77e11b13c1bd26124e0a586c19166";
          sha256 = "sha256-Qn0rqX2+wYpbyfwYzeaFsbsLvuGV6+S9GWrH3EqaHmU=";
        };

        engineCommit = "45f6e009110df4f34ec2cf99f63cf73b71b7a420";
        cipdCommit = "89ada246fcbf10f330011e4991d017332af2365b";

        cipd-binary = pkgs.fetchurl {
          name = "cipd-${cipdCommit}";
          url = "https://chrome-infra-packages.appspot.com/client?platform=linux-amd64&version=git_revision:${cipdCommit}";
          sha256 = "2ada6b46ad1cd1350522c5c05899d273f5c894c7665e30104e7f57084a5aeeb9";
        };

        git-revision = pkgs.writeText "git-revision.py" ''
          #!/usr/bin/env python3

          import sys
          import subprocess
          import os
          import argparse

          def get_repository_version(repository):
            'Returns the Git HEAD for the supplied repository path as a string.'
            if not os.path.exists(repository):
              raise IOError('path does not exist')

            with open(os.path.join(repository, '.git', 'HEAD'), 'r') as head:
              return head.read().strip()

          def main():
            parser = argparse.ArgumentParser()

            parser.add_argument(
              '--repository',
              action='store',
              help='Path to the Git repository.',
              required=True
            )

            args = parser.parse_args()
            repository = os.path.abspath(args.repository)
            version = get_repository_version(repository)
            print(version.strip())

            return 0

          if __name__ == '__main__':
            sys.exit(main())
        '';

        cipd = pkgs.runCommandLocal "cipd-${cipdCommit}" {}
          "mkdir -p $out/bin && install -m755 ${cipd-binary} $out/bin/cipd";

        vpython = pkgs.runCommandLocal "vpython3" {}
          "mkdir -p $out/bin && install -m755 ${pkgs.python3}/bin/python $out/bin/vpython3";

        gclient = pkgs.writeText "flutter-engine.gclient" ''
          solutions = [{
            "managed": False,
            "name": "src/flutter",
            "url": "https://github.com/flutter/engine.git@${engineCommit}",
          }]
        '';

        flutter-engine-src = pkgs.stdenvNoCC.mkDerivation {
          pname = "flutter-engine-src";
          version = engineCommit;

          inherit gclient depot_tools;

          nativeBuildInputs = with pkgs; [
            curl
            pkg-config
            git
            cipd
            (python3.withPackages (ps: with ps; [
              httplib2
              six
            ]))
          ];

          unpackPhase = ''
            source ${nixpkgs}/pkgs/build-support/fetchgit/deterministic-git
            export -f clean_git
            export -f make_deterministic_repo

            mkdir -p $out
            cp $gclient $out/.gclient
            cd $out

            export PATH=$PATH:$depot_tools
            python3 $depot_tools/gclient.py sync --no-history --shallow --nohooks
            find $out -name '.git' -exec dirname {} \; | xargs bash -c 'make_deterministic_repo $@' _
            find $out -path '*/.git/*' ! -name 'HEAD' -prune -exec rm -rf {} \;

            echo "3.0.0" >$out/src/third_party/dart/sdk/version

            rm -rf $out/.cipd $out/.gclient $out/.gclient_entries $out/.gclient_previous_custom_vars $out/.gclient_previous_sync_commits
          '';

          NIX_SSL_CERT_FILE = "${pkgs.cacert}/etc/ssl/certs/ca-bundle.crt";
          GIT_SSL_CAINFO = "${pkgs.cacert}/etc/ssl/certs/ca-bundle.crt";
          SSL_CERT_FILE = "${pkgs.cacert}/etc/ssl/certs/ca-bundle.crt";
          DEPOT_TOOLS_UPDATE = "0";
          DEPOT_TOOLS_COLLECT_METRICS = "0";
          PYTHONDONTWRITEBYTECODE = "1";

          dontConfigure = true;
          dontBuild = true;
          dontInstall = true;
          dontFixup = true;
          dontPatchShebangs = true;
          dontStrip = true;
          dontMoveSbin = true;
          dontPatchELF = true;

          outputHashAlgo = "sha256";
          outputHashMode = "recursive";
          outputHash = "sha256-x57mRIOPdQ4tXaLcHj5JhGZy+tGH3UVOVUgPXI44QVA=";
        };

        flutter-engine-toolchain = pkgs.stdenvNoCC.mkDerivation {
          pname = "flutter-engine-toolchain";
          version = engineCommit;

          dontUnpack = true;
          dontConfigure = true;
          dontBuild = true;

          installPhase = ''
            mkdir -p $out/lib/pkgconfig

            find ${pkgs.libglvnd}/lib -name '*.so' -exec cp {} $out/lib \;
            find ${pkgs.libglvnd.dev}/lib/pkgconfig -name '*.pc' -exec cp {} $out/lib/pkgconfig \;
          '';
        };

        flutter-engine = pkgs.fetchzip {
          url = "https://storage.googleapis.com/flutter_infra_release/flutter/${engineCommit}/linux-x64/linux-x64-embedder.zip";
          stripRoot = false;
          sha256 = "sha256-WjCJzoTUF/Po2JMS4T2Z/SGQA/c/AZDik5kg4KHFGHM=";
        };
      in {
        packages = {
          flutter-engine = pkgs.stdenvNoCC.mkDerivation {
            pname = "flutter-engine";
            version = engineCommit;

            src = flutter-engine-src;

            inherit vpython;

            nativeBuildInputs = with pkgs; [
              python3
              dart
              patchelf
              vpython
              git
              pkg-config
            ];

            buildInputs = with pkgs; [
              freetype
              xorg.libX11
              gtk3
            ];

            PYTHONDONTWRITEBYTECODE = "1";

            patchtools = [
              "flutter/third_party/gn/gn"
              "third_party/dart/tools/sdks/dart-sdk/bin/dart"
            ];

            postUnpack = ''
              dart ${flutter-engine-src.name}/src/third_party/dart/tools/generate_package_config.dart
              cp ${git-revision} ${flutter-engine-src.name}/src/flutter/build/git_revision.py

              for patchtool in ''${patchtools[@]}; do
                patchelf ${flutter-engine-src.name}/src/$patchtool --set-interpreter ${pkgs.stdenv.cc.libc_lib}/lib/ld-linux-${builtins.replaceStrings ["_"] ["-"] pkgs.targetPlatform.linuxArch}.so.2
              done

              pushd ${flutter-engine-src.name}/src/third_party/dart
              rm -rf .git
              git init
              git add .
              popd
            '';

            configurePhase = ''
              runHook preConfigure
              python3 ./src/flutter/tools/gn --no-goma --embedder-for-target --no-prebuilt-dart-sdk --out-dir $out --target-sysroot ${flutter-engine-toolchain}
              runHook postConfigure
            '';
          };

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
              wayland-protocols
            ];

            buildInputs = with pkgs; [
              libxdg_basedir
              libuv
              glib
              packagekit
              wlroots_0_16
              wayland
              spdlog
              yaml-cpp
              udev
              glm
              cairo
              libglvnd
              vulkan-loader
              crossguid
              libuuid
              jsoncpp
              libxkbcommon
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

            LIBGL_DRIVERS_PATH = "${pkgs.mesa.drivers}/lib/dri";
            VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
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
