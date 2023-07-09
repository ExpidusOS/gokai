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
            find $out -name '.git' -exec mkdir {}/logs \;
            find $out -name '.git' -exec cp {}/HEAD {}/logs/HEAD \;

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
          outputHash = "sha256-CDabtah9CZla8CrA6L/L04UXrFGd0Uq12a3QEPPX5Ww=";
        };

        flutter-engine-toolchain = pkgs.stdenvNoCC.mkDerivation {
          pname = "flutter-engine-toolchain";
          version = engineCommit;

          dontUnpack = true;
          dontConfigure = true;
          dontBuild = true;

          includeInputs = with pkgs; [
            libglvnd.dev
            libepoxy.dev
            xorg.libxcb.dev
            xorg.libX11.dev
            xorg.libXcursor.dev
            xorg.libXrandr.dev
            xorg.libXrender.dev
            xorg.libXinerama.dev
            xorg.libXi.dev
            xorg.libXext.dev
            xorg.libXfixes.dev
            xorg.libXxf86vm.dev
            gtk3.dev
            freetype.dev
            pango.dev
            glib.dev
            harfbuzz.dev
            cairo.dev
            gdk-pixbuf.dev
            at-spi2-atk.dev
            wayland.dev
            zlib.dev
            xorg.xorgproto
            stdenv.cc.libc_dev
          ];

          libInputs = with pkgs; [
            libglvnd
            libepoxy
            xorg.libxcb
            xorg.libX11
            xorg.libXcursor
            xorg.libXrandr
            xorg.libXrender
            xorg.libXinerama
            xorg.libXi
            xorg.libXext
            xorg.libXfixes
            xorg.libXxf86vm
            freetype
            gtk3
            pango.out
            glib.out
            harfbuzz
            cairo
            gdk-pixbuf
            at-spi2-atk
            wayland.out
            zlib
            stdenv.cc.libc_lib
          ];

          pkgconfigInputs = with pkgs; [
            libglvnd.dev
            libepoxy.dev
            xorg.libX11.dev
            xorg.xorgproto
            freetype.dev
            gtk3.dev
            pango.dev
            glib.dev
            harfbuzz.dev
            cairo.dev
            gdk-pixbuf.dev
            at-spi2-atk.dev
            wayland.dev
          ];

          installPhase = ''
            mkdir -p $out/usr/lib/pkgconfig $out/usr/include $out/usr/share/pkgconfig $out/nix/store

            for includeInput in ''${includeInputs[@]}; do
              cp --no-preserve=mode,ownership -r $includeInput/include/* $out/usr/include/
            done

            for libInput in ''${libInputs[@]}; do
              if [[ -d $libInput/lib ]]; then
                cp --no-preserve=mode,ownership -r $libInput/lib/* $out/usr/lib/
                cp -r $libInput $out/$libInput
              fi
            done

            for pkgconfigInput in ''${pkgconfigInputs[@]}; do
              if [[ -d $pkgconfigInput/lib/pkgconfig ]]; then
                for pkgconfig in $(find $pkgconfigInput/lib/pkgconfig -name '*.pc'); do
                  cp $pkgconfig $out/usr/lib/pkgconfig/$(basename $pkgconfig)
                  sed -i "s|prefix=/.*|prefix=$out/usr|g" $out/usr/lib/pkgconfig/$(basename $pkgconfig)
                  sed -i "s|includedir=.*|includedir=$out/usr/include|g" $out/usr/lib/pkgconfig/$(basename $pkgconfig)
                done
              fi

              if [[ -d $pkgconfigInput/share/pkgconfig ]]; then
                for pkgconfig in $(find $pkgconfigInput/share/pkgconfig -name '*.pc'); do
                  cp $pkgconfig $out/usr/share/pkgconfig/$(basename $pkgconfig)
                  sed -i "s|prefix=/.*|prefix=$out/usr|g" $out/usr/share/pkgconfig/$(basename $pkgconfig)
                  sed -i "s|includedir=.*|includedir=$out/usr/include|g" $out/usr/share/pkgconfig/$(basename $pkgconfig)
                done
              fi
            done
          '';
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
              ninja
            ];

            PYTHONDONTWRITEBYTECODE = "1";

            patchtools = [
              "buildtools/linux-x64/clang/bin/clang-16"
              "buildtools/linux-x64/clang/bin/clang-apply-replacements"
              "buildtools/linux-x64/clang/bin/clang-doc"
              "buildtools/linux-x64/clang/bin/clang-format"
              "buildtools/linux-x64/clang/bin/clang-include-fixer"
              "buildtools/linux-x64/clang/bin/clang-refactor"
              "buildtools/linux-x64/clang/bin/clang-scan-deps"
              "buildtools/linux-x64/clang/bin/clang-tidy"
              "buildtools/linux-x64/clang/bin/clangd"
              "buildtools/linux-x64/clang/bin/dsymutil"
              "buildtools/linux-x64/clang/bin/find-all-symbols"
              "buildtools/linux-x64/clang/bin/lld"
              "buildtools/linux-x64/clang/bin/llvm-ar"
              "buildtools/linux-x64/clang/bin/llvm-bolt"
              "buildtools/linux-x64/clang/bin/llvm-cov"
              "buildtools/linux-x64/clang/bin/llvm-cxxfilt"
              "buildtools/linux-x64/clang/bin/llvm-debuginfod-find"
              "buildtools/linux-x64/clang/bin/llvm-dwarfdump"
              "buildtools/linux-x64/clang/bin/llvm-dwp"
              "buildtools/linux-x64/clang/bin/llvm-gsymutil"
              "buildtools/linux-x64/clang/bin/llvm-ifs"
              "buildtools/linux-x64/clang/bin/llvm-libtool-darwin"
              "buildtools/linux-x64/clang/bin/llvm-lipo"
              "buildtools/linux-x64/clang/bin/llvm-ml"
              "buildtools/linux-x64/clang/bin/llvm-mt"
              "buildtools/linux-x64/clang/bin/llvm-nm"
              "buildtools/linux-x64/clang/bin/llvm-objcopy"
              "buildtools/linux-x64/clang/bin/llvm-objdump"
              "buildtools/linux-x64/clang/bin/llvm-pdbutil"
              "buildtools/linux-x64/clang/bin/llvm-profdata"
              "buildtools/linux-x64/clang/bin/llvm-rc"
              "buildtools/linux-x64/clang/bin/llvm-readobj"
              "buildtools/linux-x64/clang/bin/llvm-size"
              "buildtools/linux-x64/clang/bin/llvm-symbolizer"
              "buildtools/linux-x64/clang/bin/llvm-undname"
              "buildtools/linux-x64/clang/bin/llvm-xray"
              "buildtools/linux-x64/clang/bin/sancov"
              "flutter/prebuilts/linux-x64/dart-sdk/bin/dart"
              "flutter/third_party/gn/gn"
              "third_party/dart/tools/sdks/dart-sdk/bin/dart"
            ];

            patchgit = [
              "third_party/dart"
            ];

            runtimeModes = [
              "debug"
              "profile"
              "release"
              "jit_release"
            ];

            postUnpack = ''
              dart ${flutter-engine-src.name}/src/third_party/dart/tools/generate_package_config.dart
              cp ${git-revision} ${flutter-engine-src.name}/src/flutter/build/git_revision.py
              cp ${./pkg-config.py} ${flutter-engine-src.name}/src/build/config/linux/pkg-config.py

              for patchtool in ''${patchtools[@]}; do
                patchelf ${flutter-engine-src.name}/src/$patchtool --set-interpreter ${pkgs.stdenv.cc.libc_lib}/lib/ld-linux-${builtins.replaceStrings ["_"] ["-"] pkgs.targetPlatform.linuxArch}.so.2
              done

              for dir in ''${patchgit[@]}; do
                pushd ${flutter-engine-src.name}/src/$dir
                rev=$(cat .git/HEAD)
                rm -rf .git
                git init
                git add .
                mkdir -p .git/logs
                echo $rev >.git/logs/HEAD
                echo "# pack-refs with: peeled fully-peeled sorted" >.git/packed-refs
                echo "$rev refs/remotes/origin/master" >>.git/packed-refs
                popd
              done

              rm -rf ${flutter-engine-src.name}/src/third_party/angle/.git
              python3 ${flutter-engine-src.name}/src/flutter/tools/pub_get_offline.py
            '';

            # NOTE: Once https://github.com/flutter/flutter/issues/127606 is fixed, use "--no-prebuilt-dart-sdk"
            configurePhase = ''
              runHook preConfigure

              for mode in ''${runtimeModes[@]}; do
                python3 ./src/flutter/tools/gn --no-goma --runtime-mode $mode --embedder-for-target --prebuilt-dart-sdk --out-dir $out --target-sysroot ${flutter-engine-toolchain}
                python3 ./src/flutter/tools/gn --no-goma --runtime-mode $mode --embedder-for-target --prebuilt-dart-sdk --out-dir $out --target-sysroot ${flutter-engine-toolchain} --unoptimized
              done

              runHook postConfigure
            '';

            buildPhase = ''
              runHook preBuild
              for dir in $out/out/*; do
                for tool in flatc scenec gen_snapshot blobcat impellerc; do
                  ninja -C $dir -j$NIX_BUILD_CORES $tool
                  patchelf $dir/$tool --set-interpreter ${pkgs.stdenv.cc.libc_lib}/lib/ld-linux-${builtins.replaceStrings ["_"] ["-"] pkgs.targetPlatform.linuxArch}.so.2
                done

                ninja -C $dir -j$NIX_BUILD_CORES
              done
              runHook postBuild
            '';

            dontInstall = true;
          };

          sdk = pkgs.stdenv.mkDerivation rec {
            pname = "gokai-sdk";
            version = "0.1.0-git+${self.shortRev or "dirty"}";

            src = "${cleanSource self}/packages/gokai_sdk";

            mesonFlags = [
              "-Dflutter-engine=${self.packages.${system}.flutter-engine}/out/host_release"
            ];

            postInstall = ''
              cp ${self.packages.${system}.flutter-engine}/out/host_release/libflutter_engine.so $out/lib
            '';

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
              accountsservice
            ];

            propagatedBuildInputs = buildInputs;
          };

          sdk-debug = self.packages.${system}.sdk.overrideAttrs (_: _: {
            mesonFlags = [
              "-Dflutter-engine=${self.packages.${system}.flutter-engine}/out/host_debug"
            ];

            postInstall = ''
              cp ${self.packages.${system}.flutter-engine}/out/host_debug/libflutter_engine.so $out/lib
            '';
          });
        };

        devShells = {
          default = pkgs.mkShell {
            name = "gokai";

            packages = with pkgs; [
              flutter
              pkgs.wayland
              pkg-config
              self.packages.${system}.sdk-debug
              gdb
            ];

            LIBGL_DRIVERS_PATH = "${pkgs.mesa.drivers}/lib/dri";
            VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
          };

          sdk = pkgs.mkShell {
            name = "gokai-sdk";

            inherit (self.packages.${system}.sdk-debug) mesonFlags;

            packages = self.packages.${system}.sdk.nativeBuildInputs
              ++ self.packages.${system}.sdk.buildInputs;
          };
        };
      });
}
