{ self, hyprland }:
{ config, lib, pkgs, ... }:

let
  inherit (lib) mkAfter mkEnableOption mkIf mkOption types;
  cfg = config.wayland.windowManager.hyprland.hyprliquid;
  sessionFuzzelConfig = pkgs.writeText "hyprliquid-fuzzel.ini"
    (builtins.replaceStrings [ "@foot-config@" ] [ cfg.lua.footConfig ] (builtins.readFile ../demo/fuzzel.ini));
  sessionDockStyle = pkgs.writeText "hyprliquid-dock.css" (builtins.readFile ../demo/dock.css);
  sessionDockControllerText = builtins.replaceStrings
    [ "@dock-prefix@" "@dock-style@" "@fuzzel-config@" ]
    [ "hyprliquid-dock" "${sessionDockStyle}" "${sessionFuzzelConfig}" ]
    (builtins.readFile ../demo/dock-controller.sh);
  sessionDockController = pkgs.writeShellApplication {
    name = "hyprliquid-dock";
    runtimeInputs = [ pkgs.coreutils pkgs.fuzzel pkgs.nwg-dock-hyprland pkgs.procps ];
    text = sessionDockControllerText;
  };
  luaConfig = pkgs.writeText "hyprliquid-home-hyprland.lua"
    (builtins.replaceStrings
      [ "@hyprland@" "@plugin@" "@wallpaper@" "@waybar-config@" "@waybar-style@" "@foot-config@" "@kitty-config@" "@fuzzel-config@" "@dock-controller@" "@panel-command@" ]
      [
        "${hyprland.packages.${pkgs.system}.hyprland}/bin/.Hyprland-wrapped"
        "${self.packages.${pkgs.system}.hyprliquid}/lib/libhyprliquid.so"
        cfg.lua.wallpaper
        cfg.lua.waybarConfig
        cfg.lua.waybarStyle
        cfg.lua.footConfig
        cfg.lua.kittyConfig
        "${sessionFuzzelConfig}"
        "${sessionDockController}/bin/hyprliquid-dock"
        cfg.lua.panelCommand
      ]
      (builtins.readFile ../demo/hyprland.conf));
  defaultHotkeys = [
    "SUPER, H, movefocus, l"
    "SUPER, J, movefocus, d"
    "SUPER, K, movefocus, u"
    "SUPER, L, movefocus, r"
    "SUPER SHIFT, H, movewindow, l"
    "SUPER SHIFT, J, movewindow, d"
    "SUPER SHIFT, K, movewindow, u"
    "SUPER SHIFT, L, movewindow, r"
    "SUPER ALT, H, moveactive, -50 0"
    "SUPER ALT, J, moveactive, 0 50"
    "SUPER ALT, K, moveactive, 0 -50"
    "SUPER ALT, L, moveactive, 50 0"
    "SUPER CTRL, H, resizeactive, -50 0"
    "SUPER CTRL, J, resizeactive, 0 50"
    "SUPER CTRL, K, resizeactive, 0 -50"
    "SUPER CTRL, L, resizeactive, 50 0"
    "SUPER, F, fullscreen, 0"
    "SUPER, P, pseudo"
    "SUPER, E, togglesplit"
    "SUPER, TAB, workspace, m+1"
    "SUPER SHIFT, TAB, workspace, m-1"
    "SUPER, bracketleft, workspace, m-1"
    "SUPER, bracketright, workspace, m+1"
    "SUPER SHIFT, bracketleft, movetoworkspace, m-1"
    "SUPER SHIFT, bracketright, movetoworkspace, m+1"
    "SUPER, 1, workspace, 1"
    "SUPER, 2, workspace, 2"
    "SUPER, 3, workspace, 3"
    "SUPER, 4, workspace, 4"
    "SUPER, 5, workspace, 5"
    "SUPER SHIFT, 1, movetoworkspace, 1"
    "SUPER SHIFT, 2, movetoworkspace, 2"
    "SUPER SHIFT, 3, movetoworkspace, 3"
    "SUPER SHIFT, 4, movetoworkspace, 4"
    "SUPER SHIFT, 5, movetoworkspace, 5"
  ];
  sessionHotkeys = [
    "SUPER, Return, exec, foot --config ~/.config/foot/hyprliquid.ini"
    "SUPER SHIFT, Return, exec, kitty --config ~/.config/kitty/hyprliquid.conf"
    "SUPER, Q, killactive"
    "SUPER, M, exit"
    "SUPER SHIFT, V, togglefloating"
    "CTRL ALT, V, togglefloating"
    "SUPER, SPACE, exec, fuzzel --config ~/.config/fuzzel/hyprliquid.ini"
    "SUPER SHIFT, SPACE, exec, ${sessionDockController}/bin/hyprliquid-dock toggle"
  ];
  persistentWorkspaceConfig = ''
    workspace = 1, persistent:true
    workspace = 2, persistent:true
    workspace = 3, persistent:true
    workspace = 4, persistent:true
    workspace = 5, persistent:true
  '';
in
{
  options.wayland.windowManager.hyprland.hyprliquid = {
    enable = mkEnableOption "hyprliquid";

    lua.enable = mkOption {
      type = types.bool;
      default = false;
      description = "Load hyprliquid from a user-managed Hyprland Lua configuration.";
    };
    lua.wallpaper = mkOption {
      type = types.str;
      default = "${self}/assets/background.jpg";
      description = "Wallpaper path used by the generated Hyprland Lua configuration.";
    };
    lua.waybarConfig = mkOption {
      type = types.str;
      default = "${config.xdg.configHome}/waybar/hyprliquid.jsonc";
      description = "Waybar config path used by the generated Hyprland Lua configuration.";
    };
    lua.waybarStyle = mkOption {
      type = types.str;
      default = "${config.xdg.configHome}/waybar/hyprliquid.css";
      description = "Waybar stylesheet path used by the generated Hyprland Lua configuration.";
    };
    lua.footConfig = mkOption {
      type = types.str;
      default = "${config.xdg.configHome}/foot/hyprliquid.ini";
      description = "Foot config path used by the generated Hyprland Lua configuration.";
    };
    lua.kittyConfig = mkOption {
      type = types.str;
      default = "${config.xdg.configHome}/kitty/hyprliquid.conf";
      description = "Kitty config path used by the generated Hyprland Lua configuration.";
    };
    lua.panelCommand = mkOption {
      type = types.str;
      default = "waybar -c ${config.xdg.configHome}/waybar/hyprliquid.jsonc -s ${config.xdg.configHome}/waybar/hyprliquid.css";
      description = "Panel command started by the generated Hyprland Lua session.";
    };
    wayle.enable = mkOption {
      type = types.bool;
      default = false;
      description = "Install and configure the Wayle panel for the session.";
    };
    wayle.config = mkOption {
      type = types.lines;
      default = ''
        [bar]
        location = "top"
        exclusive = true
        scale = 0.85
        inset-edge = 4
        inset-ends = 8
        padding = 0.2
        padding-ends = 0.35
        module-gap = 0.25
        rounding = "lg"
        bg = "transparent"
        border-color = "#8bd5cfa6"
        border-width = 1
        button-group-background = "#0a58cd66"
        button-group-border-color = "#8bd5cf66"
        button-group-rounding = "lg"
        button-rounding = "md"
        button-label-size = 0.9
        button-icon-size = 0.9

        [[bar.layout]]
        monitor = "*"
        left = ["dashboard", "hyprland-workspaces", "window-title"]
        center = ["clock"]
        right = ["network", "volume", "keyboard-input", "systray", "power"]

        [modules.hyprland-workspaces]
        min-workspace-count = 5
        monitor-specific = true
        show-special = false
        display-mode = "label"
        numbering = "absolute"
        active-indicator = "background"
        app-icons-show = true
        container-bg-color = "bg-surface-elevated"

        [modules.window-title]
        max-length = 80

        [modules.clock]
        format = "%a %b %d  %H:%M"

        [styling]
        scale = 0.9
        rounding = "lg"

        [styling.palette]
        bg = "#061b3a"
        surface = "#0a58cd66"
        elevated = "#1a6edb880"
        fg = "#f5f5f5"
        fg-muted = "#c5dcff"
        primary = "#9ccbff"
        blue = "#8bd5cf"
      '';
      description = "Wayle TOML configuration installed for the session.";
    };

    hotkeys.enable = mkOption {
      type = types.bool;
      default = true;
      description = "Enable the default workspace and window-management hotkeys.";
    };

    waybar.patchPackage = mkOption {
      type = types.bool;
      default = true;
      description = "Use a Waybar package whose workspace clicks dispatch through Hyprland Lua.";
    };
    waybar.enable = mkOption {
      type = types.bool;
      default = true;
      description = "Enable Waybar and use the hyprliquid workspace profile.";
    };
    waybar.installConfig = mkOption {
      type = types.bool;
      default = true;
      description = "Install the dynamic workspace Waybar configuration and stylesheet.";
    };

    session.enable = mkOption {
      type = types.bool;
      default = true;
      description = "Enable the demo-compatible launcher, dock, terminal, and Waybar session.";
    };
    session.inputMethod.enable = mkOption {
      type = types.bool;
      default = true;
      description = "Enable the Fcitx5 Rime Chinese input method in the session preset.";
    };

    settings = {
      enabled = mkOption { type = types.bool; default = true; };
      background_sharing = mkOption { type = types.bool; default = false; };
      watch_system_color_scheme = mkOption { type = types.bool; default = false; };
      effect = mkOption { type = types.either types.int types.str; default = 0; };
      corner_radius = mkOption { type = types.int; default = 18; };
      z_radius = mkOption { type = types.int; default = -1; };
      glass_thickness = mkOption { type = types.float; default = 500.0; };
      glass_ior = mkOption { type = types.float; default = 1.035; };
      glass_ior_r = mkOption { type = types.float; default = 1.02; };
      glass_ior_g = mkOption { type = types.float; default = 1.035; };
      glass_ior_b = mkOption { type = types.float; default = 1.05; };
      glass_dispersion = mkOption { type = types.bool; default = true; };
      vdf_map_mode = mkOption { type = types.int; default = 0; };
      vdf_map_update_policy = mkOption { type = types.either types.int types.str; default = -2; };
      vdf_map_debug_mode = mkOption { type = types.int; default = 0; };
      tint_color = mkOption { type = types.str; default = "rgba(0, 0, 0, 0)"; };
      brightness = mkOption { type = types.float; default = 1.0; };
      highlight_style = mkOption { type = types.int; default = 4; };
      color_scheme = mkOption { type = types.either types.int types.str; default = 3; };
      aero_reflection_map_path = mkOption { type = types.str; default = ""; };
      rounding_lua = mkOption { type = types.int; default = -1; };
    };
  };

  config = mkIf cfg.enable {
    wayland.windowManager.hyprland.package = mkIf cfg.lua.enable hyprland.packages.${pkgs.system}.hyprland;
    wayland.windowManager.hyprland.plugins = mkIf (!cfg.lua.enable) [
      self.packages.${pkgs.system}.hyprliquid
    ];
    programs.waybar.package = mkIf cfg.waybar.patchPackage self.packages.${pkgs.system}.waybar;
    programs.waybar.enable = cfg.waybar.enable;
    wayland.windowManager.hyprland.settings = {
      input = {
        kb_layout = "us";
      };
      general = {
        gaps_in = 5;
        gaps_out = 6;
        border_size = 2;
        layout = "dwindle";
        allow_tearing = false;
        resize_on_border = true;
        "col.active_border" = "rgba(8bd5cfff)";
        "col.inactive_border" = "rgba(6e738d99)";
      };
      decoration = {
        rounding = 18;
        active_opacity = 1.0;
        inactive_opacity = 1.0;
        shadow = {
          enabled = true;
          range = 20;
          render_power = 3;
          color = "rgba(00000055)";
        };
        blur = {
          enabled = true;
          size = 3;
          passes = 1;
          new_optimizations = true;
        };
      };
      animations = {
        enabled = true;
        bezier = [ "liquid, 0.16, 1, 0.3, 1" ];
        animation = [
          "windows, 1, 5, liquid"
          "windowsOut, 1, 5, liquid, popin 80%"
          "border, 1, 8, liquid"
          "fade, 1, 5, liquid"
          "workspaces, 1, 5, liquid, slide"
        ];
      };
      misc = {
        force_default_wallpaper = 0;
        disable_hyprland_logo = true;
        disable_splash_rendering = true;
      };
      xwayland.enabled = false;
      plugin.hyprliquid = mkIf (!cfg.lua.enable) cfg.settings;
      bind = mkAfter (if cfg.hotkeys.enable then defaultHotkeys else [ ]);
    } // mkIf cfg.session.enable {
      bind = mkAfter sessionHotkeys;
      exec-once = mkAfter (lib.optional
        (cfg.waybar.enable && cfg.waybar.installConfig)
        "waybar -c ~/.config/waybar/hyprliquid.jsonc -s ~/.config/waybar/hyprliquid.css" ++ [
        "foot --config ~/.config/foot/hyprliquid.ini --server"
        "${sessionDockController}/bin/hyprliquid-dock start"
      ]);
    };

    home.packages = (if cfg.session.enable then [
      pkgs.foot
      pkgs.kitty
      pkgs.fuzzel
      pkgs.nwg-dock-hyprland
      sessionDockController
    ] else [ ]) ++ lib.optional cfg.wayle.enable self.packages.${pkgs.system}.wayle;

    home.file = {
      ".config/hypr/hyprland.lua" = mkIf cfg.lua.enable {
        source = luaConfig;
      };
      ".config/waybar/hyprliquid.jsonc" = mkIf cfg.waybar.installConfig {
        source = ../demo/waybar.jsonc;
      };
      ".config/waybar/hyprliquid.css" = mkIf cfg.waybar.installConfig {
        source = ../demo/waybar.css;
      };
      ".config/foot/hyprliquid.ini" = mkIf cfg.session.enable {
        source = ../demo/foot.ini;
      };
      ".config/foot/foot.ini" = mkIf cfg.session.enable {
        source = ../demo/foot.ini;
      };
      ".config/kitty/hyprliquid.conf" = mkIf cfg.session.enable {
        source = ../demo/kitty.conf;
      };
      ".config/kitty/kitty.conf" = mkIf cfg.session.enable {
        source = ../demo/kitty.conf;
      };
      ".config/fuzzel/hyprliquid.ini" = mkIf cfg.session.enable {
        source = sessionFuzzelConfig;
      };
      ".config/fuzzel/fuzzel.ini" = mkIf cfg.session.enable {
        source = sessionFuzzelConfig;
      };
      ".config/nwg-dock-hyprland/style.css" = mkIf cfg.session.enable {
        source = ../demo/dock.css;
      };
      ".config/wayle/config.toml" = mkIf cfg.wayle.enable {
        text = cfg.wayle.config;
      };
    };

    i18n.inputMethod = mkIf (cfg.session.enable && cfg.session.inputMethod.enable) {
      enable = true;
      type = "fcitx5";
      fcitx5 = {
        addons = [ pkgs.fcitx5-rime ];
        waylandFrontend = true;
        settings.inputMethod = {
          GroupOrder."0" = "Default";
          "Groups/0" = {
            Name = "Default";
            "Default Layout" = "us";
            DefaultIM = "rime";
          };
          "Groups/0/Items/0" = {
            Name = "keyboard-us";
            Layout = "";
          };
          "Groups/0/Items/1" = {
            Name = "rime";
            Layout = "";
          };
        };
      };
    };

    wayland.windowManager.hyprland.extraConfig = mkAfter (if cfg.lua.enable then "" else persistentWorkspaceConfig + (if cfg.session.enable then ''
      layerrule {
          name = hyprliquid-dock-blur
          match:namespace = ^nwg-dock$
          blur = on
      }

      layerrule {
          name = hyprliquid-dock-glass
          match:namespace = ^nwg-dock$
          hyprliquid {
              effect = liquid_glass
              corner_radius = 16
              z_radius = -1
              glass_thickness = 700.0
              glass_ior = 1.035
              glass_ior_r = 1.02
              glass_ior_g = 1.035
              glass_ior_b = 1.05
              glass_dispersion = true
              brightness = 1.05
              tint_color = rgba(10, 88, 205, 0.42)
              highlight_style = 4
              vdf_map_mode = 0
          }
      }

      layerrule {
          name = hyprliquid-launcher-blur
          match:namespace = ^hyprliquid-launcher$
          blur = on
      }

      layerrule {
          name = hyprliquid-launcher-glass
          match:namespace = ^hyprliquid-launcher$
          hyprliquid {
              effect = liquid_glass
              corner_radius = 16
              z_radius = -1
              glass_thickness = 700.0
              glass_ior = 1.035
              glass_ior_r = 1.02
              glass_ior_g = 1.035
              glass_ior_b = 1.05
              glass_dispersion = true
              brightness = 1.05
              tint_color = rgba(10, 88, 205, 0.42)
              highlight_style = 4
              vdf_map_mode = 0
          }
      }
    '' else ""));
  };
}
