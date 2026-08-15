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
    in
    {
      packages = forAllSystems (system: {
        hyprliquid = mkPackage system;
        default = self.packages.${system}.hyprliquid;
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
