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

  inputs.flutter = {
    flake = false;
    url = github:flutter/flutter/stable;
  };

  outputs = { self, expidus-sdk, nixpkgs, flutter }@inputs:
    with expidus-sdk.lib;
    flake-utils.eachSystem flake-utils.allSystems (system:
      let
        pkgs = expidus-sdk.legacyPackages.${system};
      in {
        packages = {
          sdk = pkgs.expidus.gokai.overrideAttrs (f: p: {
            version = "0.1.0-git+${self.shortRev or "dirty"}";
            src = cleanSource self;

            buildInputs = p.buildInputs
              ++ (with pkgs; [
                libevdev
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
              ln -s ${flutter}/packages/flutter_tools $sourceRoot/vendor/flutter_tools
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
                ln -s ${flutter}/packages/$flutterPackage $out/packages/$flutterPackage
              done

              ln -s $src/packages/gokai $out/packages/gokai

              wrapProgram $out/bin/gokai \
                --set-default FLUTTER_ROOT $out
            '';

            dartEntryPoints = {
              "bin/gokai" = "lib/executable.dart";
            };

            pubspecLockFile = ./packages/gokai_tools/pubspec.lock;
            vendorHash = "sha256-Z1YmUYksABwWG1my0NssMJFRqJzf2JCmvklX3frEYT8=";
          };
        };

        devShells = {
          default = pkgs.mkShell {
            name = "gokai";

            packages = with pkgs; [
              pkgs.flutter
              pkgs.wayland
              pkg-config
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
            ];

            shellHook = ''
              cd packages/gokai_tools
            '';
          };
        };
      });
}
