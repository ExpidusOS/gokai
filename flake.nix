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

        supportsLinuxDesktop = pkgs.stdenv.hostPlatform.isLinux;
        supportsAndroid = (pkgs.stdenv.hostPlatform.isx86_64 || pkgs.stdenv.hostPlatform.isDarwin);
        supportsDarwin = pkgs.stdenv.hostPlatform.isDarwin;
        supportsIOS = pkgs.stdenv.hostPlatform.isDarwin;

        includedEngineArtifacts = {
          common = [
            "flutter_patched_sdk"
            "flutter_patched_sdk_product"
          ];
          platform = {
            android = optionalAttrs supportsAndroid
              ((genAttrs [ "arm" "arm64" "x64" ] (architecture: [ "profile" "release" ])) // { x86 = [ "jit-release" ]; });
            darwin = optionalAttrs supportsDarwin
              ((genAttrs [ "arm64" "x64" ] (architecture: [ "profile" "release" ])));
            ios = optionalAttrs supportsIOS
              ((genAttrs [ "" ] (architecture: [ "profile" "release" ])));
            linux = optionalAttrs supportsLinuxDesktop
              (genAttrs ((optional pkgs.stdenv.hostPlatform.isx86_64 "x64") ++ (optional pkgs.stdenv.hostPlatform.isAarch64 "arm64"))
                (architecture: [ "debug" "profile" "release" ]));
          };
        };

        engineArtifacts = pkgs.callPackage "${nixpkgs.outPath}/pkgs/development/compilers/flutter/engine-artifacts" {
          inherit (pkgs.flutter.unwrapped) engineVersion;
        };

        mkCommonArtifactLinkCommand = { artifact }: ''
          mkdir -p $out/artifacts/engine/common
          lndir -silent ${artifact} $out/artifacts/engine/common
        '';

        mkPlatformArtifactLinkCommand = { artifact, os, architecture, variant ? null }:
          let
            artifactDirectory = "${os}-${architecture}${optionalString (variant != null) "-${variant}"}";
          in ''
          mkdir -p $out/artifacts/engine/${artifactDirectory}
          lndir -silent ${artifact} $out/artifacts/engine/${artifactDirectory}
        '';

        flutterEngineArtifactDirectory = pkgs.runCommandLocal "flutter-engine-artifacts-${pkgs.flutter.version}" {
          nativeBuildInputs = with pkgs; [ xorg.lndir ];
        } (
            builtins.concatStringsSep "\n"
            ((map
              (name: mkCommonArtifactLinkCommand {
                artifact = engineArtifacts.common.${name};
              })
              (includedEngineArtifacts.common or [ ])) ++
            (builtins.foldl'
              (commands: os: commands ++
                (builtins.foldl'
                  (commands: architecture: commands ++
                    (builtins.foldl'
                      (commands: variant: commands ++
                        (map
                          (artifact: mkPlatformArtifactLinkCommand {
                            inherit artifact os architecture variant;
                          })
                          engineArtifacts.platform.${os}.${architecture}.variants.${variant}))
                      (map
                        (artifact: mkPlatformArtifactLinkCommand {
                          inherit artifact os architecture;
                        })
                        engineArtifacts.platform.${os}.${architecture}.base)
                      includedEngineArtifacts.platform.${os}.${architecture}))
                  [ ]
                  (builtins.attrNames includedEngineArtifacts.platform.${os})))
              [ ]
              (builtins.attrNames (includedEngineArtifacts.platform or { }))))
          );

          flutterCacheDir = pkgs.symlinkJoin {
            name = "flutter-cache-dir";
            paths = [
              flutterEngineArtifactDirectory
              "${pkgs.flutter}/bin/cache"
            ];
          };
      in {
        packages = {
          sdk = pkgs.expidus.gokai.overrideAttrs (f: p: {
            version = "0.1.0-git+${self.shortRev or "dirty"}";
            src = cleanSource self;

            buildInputs = p.buildInputs
              ++ (with pkgs; [
                libevdev
                upower
              ]);
          });

          sdk-debug = self.packages.${system}.sdk.overrideAttrs (_: _: {
            mesonFlags = [
              "-Dflutter-engine=${pkgs.flutter-engine}/out/host_debug"
            ];

            postInstall = ''
              cp ${pkgs.flutter-engine}/out/host_debug/libflutter_engine.so $out/lib
            '';
          });

          tools = pkgs.buildDartApplication {
            pname = "gokai-tools";
            version = "0.1.0-git+${self.shortRev or "dirty"}";

            src = cleanSource self;
            sourceRoot = "source/packages/gokai_tools";

            postUnpack = ''
              rm $sourceRoot/vendor/flutter_tools
              ln -s ${pkgs.flutter}/packages/flutter_tools $sourceRoot/vendor/flutter_tools
              mkdir -p $sourceRoot/bin $out/bin
              cp source/bin/dart $out/bin/dart
            '';

            flutterPackages = [
              "flutter"
              "flutter_driver"
              "flutter_goldens"
              "flutter_goldens_client"
              "flutter_localizations"
              "flutter_test"
              "flutter_tools"
              "flutter_web_plugins"
              "fuchsia_remote_debug_protocol"
              "integration_test"
            ];

            postInstall = ''
              mkdir -p $out/packages
              for flutterPackage in ''${flutterPackages[@]}; do
                if [[ -d $out/packages/$flutterPackage ]]; then
                  rm $out/packages/$flutterPackage
                fi

                ln -s ${pkgs.flutter}/packages/$flutterPackage $out/packages/$flutterPackage
              done

              ln -s $src/packages/gokai $out/packages/gokai

              ln -s ${flutterCacheDir} $out/bin/cache
              ln -s ${pkgs.flutter}/bin/internal $out/bin/internal
              echo "${self.shortRev or "dirty"}" >$out/version

              wrapProgram $out/bin/gokai \
                --set-default FLUTTER_ROOT $out \
                --set FLUTTER_ALREADY_LOCKED true \
                --set FLUTTER_CACHE_DIR ${flutterCacheDir} \
                --set GOKAI_VERSION_EXISTS true \
                --set GOKAI_DONT_UPDATE true \
                --set GOKAI_VERSION ${self.shortRev or "dirty"}
            '';

            dartEntryPoints = {
              "bin/gokai" = "lib/executable.dart";
            };

            pubspecLockFile = ./packages/gokai_tools/pubspec.lock;
            vendorHash = "sha256-mhlwNd3DYBtCtdl6nhCibY9GzFUngFpsv+utVVkpikw=";
          };
        };

        devShells = {
          default = pkgs.mkShell {
            name = "gokai";

            packages = with pkgs; [
              pkgs.wayland
              pkg-config
              clang
              cmake
              ninja
              pkg-config
              self.packages.${system}.sdk
              self.packages.${system}.tools
              self.packages.${system}.sdk-debug
              gdb
            ];

            shellHook = ''
              cd packages/gokai
            '';

            LIBGL_DRIVERS_PATH = "${pkgs.mesa.drivers}/lib/dri";
            VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
          };

          sdk = pkgs.mkShell {
            name = "gokai-sdk";

            inherit (self.packages.${system}.sdk-debug) mesonFlags;

            packages = self.packages.${system}.sdk.nativeBuildInputs
              ++ self.packages.${system}.sdk.buildInputs;

            shellHook = ''
              cd packages/gokai_sdk
            '';
          };

          tools = pkgs.mkShell {
            name = "gokai-tools";

            packages = with pkgs; [
              dart
              unzip
              clang
              cmake
              ninja
              pkg-config
              self.packages.${system}.sdk
            ];

            shellHook = ''
              cd packages/gokai_tools
            '';
          };
        };
      });
}
