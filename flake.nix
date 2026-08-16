{
  description = "Liquid Glass, Acrylic, Mica, and Aero material effects for Hyprland";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    hyprland = {
      url = "github:hyprwm/Hyprland";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, hyprland }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" ];
      forAllSystems = nixpkgs.lib.genAttrs systems;
      mkPackage = system:
        let
          pkgs = import nixpkgs { inherit system; };
          hyprlandPackage = hyprland.packages.${system}.hyprland;
        in
          pkgs.stdenv.mkDerivation {
            pname = "hyprliquid";
            version = "0.1.0";
            src = self;

            nativeBuildInputs = with pkgs; [
              cmake
              pkg-config
            ];

            buildInputs = with pkgs; [
              hyprlandPackage
              aquamarine
              cairo
              glslang
              hyprcursor
              hyprgraphics
              hyprlang
              hyprutils
              hyprwire
              libdrm
              libglvnd
              libinput
              libxkbcommon
              pixman
              pango
              stb
              systemd
              wayland
              xcbutil
              xcbutilkeysyms
              xcbutilerrors
              xcbutilwm
            ];

            cmakeFlags = [ "-DCMAKE_BUILD_TYPE=Release" ];

            installPhase = ''
              cmake --install . --prefix "$out"
            '';

            meta = with pkgs.lib; {
              description = "Liquid Glass, Acrylic, Mica, and Aero material effects for Hyprland";
              homepage = "https://github.com/Neuron-Group/hyprliquid";
              license = licenses.bsd3;
              platforms = platforms.linux;
            };
          };
      mkDemoConfig = system:
        let
          pkgs = import nixpkgs { inherit system; };
          hyprlandPackage = hyprland.packages.${system}.hyprland;
          plugin = self.packages.${system}.hyprliquid;
          waybarConfig = pkgs.writeText "hyprliquid-demo-waybar.jsonc" (builtins.readFile ./demo/waybar.jsonc);
          waybarStyle = pkgs.writeText "hyprliquid-demo-waybar.css" (builtins.readFile ./demo/waybar.css);
          footConfig = pkgs.writeText "hyprliquid-demo-foot.ini" (builtins.readFile ./demo/foot.ini);
          kittyConfig = pkgs.writeText "hyprliquid-demo-kitty.conf" (builtins.readFile ./demo/kitty.conf);
        in
          pkgs.writeText "hyprliquid-demo.conf"
            (builtins.replaceStrings
              [ "@hyprland@" "@plugin@" "@wallpaper@" "@waybar-config@" "@waybar-style@" "@foot-config@" "@kitty-config@" ]
              [ "${hyprlandPackage}/bin/.Hyprland-wrapped" "${plugin}/lib/libhyprliquid.so" "${self}/assets/background.jpg" "${waybarConfig}" "${waybarStyle}" "${footConfig}" "${kittyConfig}" ]
              (builtins.readFile ./demo/hyprland.conf));
      mkDemo = system:
        let
          pkgs = import nixpkgs { inherit system; };
          hyprlandPackage = hyprland.packages.${system}.hyprland;
          config = mkDemoConfig system;
        in
          pkgs.writeShellApplication {
            name = "hyprliquid-demo";
            runtimeInputs = [ hyprlandPackage pkgs.foot pkgs.kitty pkgs.swaybg pkgs.waybar ];
            text = ''
              if [ -z "''${XDG_RUNTIME_DIR:-}" ]; then
                echo "hyprliquid-demo: XDG_RUNTIME_DIR is not set" >&2
                exit 1
              fi

              if [ -n "''${WAYLAND_DISPLAY:-}" ]; then
                export WLR_BACKENDS=wayland
              elif [ -n "''${DISPLAY:-}" ]; then
                export WLR_BACKENDS=x11
              else
                echo "hyprliquid-demo: start it from an existing graphical session" >&2
                exit 1
              fi

              export HYPRLAND_NO_SD_NOTIFY=1
              export WLR_RENDERER_ALLOW_SOFTWARE=1
              export XDG_CURRENT_DESKTOP=Hyprland
              export XDG_SESSION_DESKTOP=Hyprland
              exec Hyprland -c ${config}
            '';
          };
      mkCheckConfig = system:
        let
          pkgs = import nixpkgs { inherit system; };
          hyprlandPackage = hyprland.packages.${system}.hyprland;
          plugin = self.packages.${system}.hyprliquid;
          config = mkDemoConfig system;
        in
          pkgs.writeShellApplication {
            name = "hyprliquid-check-config";
            runtimeInputs = [ hyprlandPackage ];
            text = ''
              test -r ${plugin}/lib/libhyprliquid.so
              echo "hyprliquid plugin: ${plugin}/lib/libhyprliquid.so"
              exec Hyprland --verify-config -c ${config}
            '';
          };
    in
    {
      packages = forAllSystems (system: {
        hyprliquid = mkPackage system;
        demo-config = mkDemoConfig system;
        demo = mkDemo system;
        check-config = mkCheckConfig system;
        default = self.packages.${system}.hyprliquid;
      });

      apps = forAllSystems (system: {
        try = {
          type = "app";
          program = "${self.packages.${system}.demo}/bin/hyprliquid-demo";
          meta.description = "Run the hyprliquid demo Hyprland session";
        };
        default = self.apps.${system}.try;
        check = {
          type = "app";
          program = "${self.packages.${system}.check-config}/bin/hyprliquid-check-config";
          meta.description = "Validate the hyprliquid demo Hyprland configuration";
        };
      });

      overlays.default = final: prev: {
        hyprliquid = self.packages.${final.system}.hyprliquid;
      };

      homeManagerModules.default = import ./modules/home-manager.nix {
        inherit self;
      };

      checks = forAllSystems (system: {
        hyprliquid = self.packages.${system}.hyprliquid;
      });

      devShells = forAllSystems (system:
        let pkgs = import nixpkgs { inherit system; };
        in {
          default = pkgs.mkShell {
            inputsFrom = [ self.packages.${system}.hyprliquid ];
          };
        });
    };
}
