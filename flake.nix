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
          sdk = pkgs.stdenv.mkDerivation rec {
            pname = "gokai-sdk";
            version = "0.1.0-git+${self.shortRev or "dirty"}";

            src = "${cleanSource self}/packages/gokai_sdk";

            mesonFlags = [
              "-Dflutter-engine=${pkgs.flutter-engine}/out/host_release"
            ];

            postInstall = ''
              cp ${pkgs.flutter-engine}.flutter-engine}/out/host_release/libflutter_engine.so $out/lib
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
              pam
              libdrm
            ];

            propagatedBuildInputs = buildInputs;
          };

          sdk-debug = self.packages.${system}.sdk.overrideAttrs (_: _: {
            mesonFlags = [
              "-Dflutter-engine=${pkgs.flutter-engine}/out/host_debug"
            ];

            postInstall = ''
              cp ${pkgs.flutter-engine}/out/host_debug/libflutter_engine.so $out/lib
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
