{ self }:
{ config, lib, pkgs, ... }:

let
  inherit (lib) mkAfter mkEnableOption mkIf mkOption types;
  cfg = config.wayland.windowManager.hyprland.hyprliquid;
  sessionFuzzelConfig = pkgs.writeText "hyprliquid-fuzzel.ini"
    (builtins.replaceStrings [ "@foot-config@" ] [ "foot" ] (builtins.readFile ../demo/fuzzel.ini));
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
  waybarPackage = pkgs.waybar.overrideAttrs (old: {
    postPatch = (old.postPatch or "") + ''
      substituteInPlace src/modules/hyprland/workspace.cpp \
        --replace-fail \
        "m_ipc.getSocket1Reply(\"dispatch workspace \" + std::to_string(id()));" \
        "m_ipc.getSocket1Reply(\"dispatch 'hl.dsp.focus({ workspace = \" + std::to_string(id()) + \" })'\");"
    '';
  });
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

    gtk.enable = mkOption {
      type = types.bool;
      default = true;
      description = "Enable the GTK defaults used by the hyprliquid desktop profile.";
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
    programs.waybar.package = mkIf cfg.waybar.patchPackage waybarPackage;
    programs.waybar.enable = cfg.waybar.enable;
    gtk = mkIf cfg.gtk.enable {
      enable = true;
      gtk3.extraConfig = {
        gtk-application-prefer-dark-theme = 1;
      };
      gtk4.extraConfig = {
        gtk-application-prefer-dark-theme = 1;
      };
    };
    wayland.windowManager.hyprland.settings = {
      plugin.hyprliquid = cfg.settings;
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

    home.packages = mkIf cfg.session.enable [
      pkgs.foot
      pkgs.kitty
      pkgs.fuzzel
      pkgs.nwg-dock-hyprland
      sessionDockController
    ];

    home.file = {
      ".config/waybar/hyprliquid.jsonc" = mkIf cfg.waybar.installConfig {
        source = ../demo/waybar.jsonc;
      };
      ".config/waybar/hyprliquid.css" = mkIf cfg.waybar.installConfig {
        source = ../demo/waybar.css;
      };
      ".config/foot/hyprliquid.ini" = mkIf cfg.session.enable {
        source = ../demo/foot.ini;
      };
      ".config/kitty/hyprliquid.conf" = mkIf cfg.session.enable {
        source = ../demo/kitty.conf;
      };
      ".config/fuzzel/hyprliquid.ini" = mkIf cfg.session.enable {
        source = sessionFuzzelConfig;
      };
      ".config/nwg-dock-hyprland/style.css" = mkIf cfg.session.enable {
        source = ../demo/dock.css;
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

    wayland.windowManager.hyprland.extraConfig = mkAfter (persistentWorkspaceConfig + (if cfg.session.enable then ''
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
