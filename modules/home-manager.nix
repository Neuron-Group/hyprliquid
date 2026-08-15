{ self }:
{ config, lib, pkgs, ... }:

let
  inherit (lib) mkEnableOption mkIf mkOption types;
  cfg = config.wayland.windowManager.hyprland.hyprliquid;
in
{
  options.wayland.windowManager.hyprland.hyprliquid = {
    enable = mkEnableOption "hyprliquid";

    settings = {
      enabled = mkOption { type = types.bool; default = true; };
      background_sharing = mkOption { type = types.bool; default = false; };
      watch_system_color_scheme = mkOption { type = types.bool; default = false; };
      effect = mkOption { type = types.either types.int types.str; default = 0; };
      corner_radius = mkOption { type = types.int; default = -1; };
      z_radius = mkOption { type = types.int; default = -1; };
      glass_thickness = mkOption { type = types.float; default = 500.0; };
      glass_ior = mkOption { type = types.float; default = 1.035; };
      glass_ior_r = mkOption { type = types.float; default = 1.02; };
      glass_ior_g = mkOption { type = types.float; default = 1.035; };
      glass_ior_b = mkOption { type = types.float; default = 1.05; };
      glass_dispersion = mkOption { type = types.bool; default = false; };
      vdf_map_mode = mkOption { type = types.int; default = 0; };
      vdf_map_update_policy = mkOption { type = types.either types.int types.str; default = -2; };
      vdf_map_debug_mode = mkOption { type = types.int; default = 0; };
      tint_color = mkOption { type = types.str; default = "rgba(0, 0, 0, 0)"; };
      brightness = mkOption { type = types.float; default = 1.0; };
      highlight_style = mkOption { type = types.int; default = 0; };
      color_scheme = mkOption { type = types.either types.int types.str; default = 0; };
      aero_reflection_map_path = mkOption { type = types.str; default = ""; };
      rounding_lua = mkOption { type = types.int; default = -1; };
    };
  };

  config = mkIf cfg.enable {
    wayland.windowManager.hyprland.plugins = [ self.packages.${pkgs.system}.hyprliquid ];
    wayland.windowManager.hyprland.settings.plugin.hyprliquid = cfg.settings;
  };
}
