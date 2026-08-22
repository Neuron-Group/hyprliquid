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
      nixosModule = { pkgs, ... }:
        {
          programs.hyprland.package = hyprland.packages.${pkgs.system}.hyprland;
        };
      mkDemoConfig = system:
        let
          pkgs = import nixpkgs { inherit system; };
          hyprlandPackage = hyprland.packages.${system}.hyprland;
          plugin = self.packages.${system}.hyprliquid;
          waybarConfig = pkgs.writeText "hyprliquid-demo-waybar.jsonc"
            (builtins.replaceStrings
              [ "@waybar-launcher@" "@waybar-control-center@" "@fuzzel-config@" ]
              [ "${./assets/waybar/lambda-launcher.svg}" "${./assets/waybar/control-center.svg}" "${fuzzelConfig}" ]
              (builtins.readFile ./demo/waybar.jsonc));
          waybarStyle = pkgs.writeText "hyprliquid-demo-waybar.css" (builtins.readFile ./demo/waybar.css);
          footConfig = pkgs.writeText "hyprliquid-demo-foot.ini" (builtins.readFile ./demo/foot.ini);
          kittyConfig = pkgs.writeText "hyprliquid-demo-kitty.conf" (builtins.readFile ./demo/kitty.conf);
          fuzzelConfig = pkgs.writeText "hyprliquid-demo-fuzzel.ini"
            (builtins.replaceStrings [ "@foot-config@" ] [ "${footConfig}" ] (builtins.readFile ./demo/fuzzel.ini));
          dockStyle = pkgs.writeText "hyprliquid-demo-dock.css" (builtins.readFile ./demo/dock.css);
          dockControllerText = builtins.replaceStrings
            [ "@dock-prefix@" "@dock-style@" "@fuzzel-config@" ]
            [ "hyprliquid-demo-dock" "${dockStyle}" "${fuzzelConfig}" ]
            (builtins.readFile ./demo/dock-controller.sh);
          dockController = pkgs.writeShellApplication {
            name = "hyprliquid-demo-dock";
            runtimeInputs = [ pkgs.coreutils pkgs.fuzzel pkgs.nwg-dock-hyprland pkgs.procps ];
            text = dockControllerText;
          };
        in
          pkgs.writeText "hyprliquid-demo.conf"
            (builtins.replaceStrings
              [ "@hyprland@" "@plugin@" "@wallpaper@" "@waybar-config@" "@waybar-style@" "@foot-config@" "@kitty-config@" "@fuzzel-config@" "@dock-controller@" "@xwayland-enabled@" "@xwayland-environment@" "@session-environment@" ]
              [ "${hyprlandPackage}/bin/.Hyprland-wrapped" "${plugin}/lib/libhyprliquid.so" "${self}/assets/background.jpg" "${waybarConfig}" "${waybarStyle}" "${footConfig}" "${kittyConfig}" "${fuzzelConfig}" "${dockController}/bin/hyprliquid-demo-dock" "false" "" "true" ]
              (builtins.readFile ./demo/hyprland.conf));
      mkDemo = system:
        let
          pkgs = import nixpkgs { inherit system; };
          hyprlandPackage = hyprland.packages.${system}.hyprland;
          config = mkDemoConfig system;
          gtkSettings = pkgs.writeText "hyprliquid-demo-gtk-settings.ini" ''
            [Settings]
            gtk-application-prefer-dark-theme=1
          '';
          fcitxPackage = pkgs.qt6Packages.fcitx5-with-addons.override {
            addons = [ pkgs.fcitx5-rime ];
          };
          fcitxProfile = pkgs.writeText "hyprliquid-demo-fcitx5-profile" ''
            [Groups/0]
            Name=Default
            Default Layout=us
            DefaultIM=rime

            [Groups/0/Items/0]
            Name=keyboard-us
            Layout=

            [Groups/0/Items/1]
            Name=rime
            Layout=

            [GroupOrder]
            0=Default
          '';
        in
          pkgs.writeShellApplication {
            name = "hyprliquid-demo";
            runtimeInputs = [ fcitxPackage hyprlandPackage pkgs.foot pkgs.kitty pkgs.fuzzel pkgs.nwg-dock-hyprland pkgs.swaybg pkgs.waybar ];
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
              export XDG_DATA_DIRS="${pkgs.foot}/share:${pkgs.kitty}/share:${pkgs.fuzzel}/share:''${XDG_DATA_DIRS:-/usr/local/share:/usr/share}"
              export XMODIFIERS='@im=fcitx'
              export QT_IM_MODULE=fcitx
              export SDL_IM_MODULE=fcitx
              export GLFW_IM_MODULE=ibus
              export XDG_CONFIG_HOME="''${XDG_RUNTIME_DIR}/hyprliquid-demo-config-$$"
              mkdir -p "$XDG_CONFIG_HOME/fcitx5" "$XDG_CONFIG_HOME/gtk-3.0" "$XDG_CONFIG_HOME/gtk-4.0"
              cp ${fcitxProfile} "$XDG_CONFIG_HOME/fcitx5/profile"
              cp ${gtkSettings} "$XDG_CONFIG_HOME/gtk-3.0/settings.ini"
              cp ${gtkSettings} "$XDG_CONFIG_HOME/gtk-4.0/settings.ini"
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
      nixosModules = {
        default = nixosModule;
        hyprland = nixosModule;
      };

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
        inherit self hyprland;
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
