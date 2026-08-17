controller_log="${XDG_RUNTIME_DIR:?}/@dock-prefix@-controller.log"
log() {
  printf '[%s] %s\n' "$(date --iso-8601=seconds)" "$*" >> "$controller_log"
}

log "controller invoked: action=${1:-start}, pid=$$, parent=$PPID, signature=${HYPRLAND_INSTANCE_SIGNATURE:-unset}, display=${WAYLAND_DISPLAY:-unset}"

if [ -z "${HYPRLAND_INSTANCE_SIGNATURE:-}" ]; then
  parent_pid="$PPID"
  while [ "${parent_pid:-1}" -gt 1 ] && [ -z "${HYPRLAND_INSTANCE_SIGNATURE:-}" ]; do
    for lock_file in "${XDG_RUNTIME_DIR:?}"/hypr/*/hyprland.lock; do
      [ -r "$lock_file" ] || continue
      read -r lock_pid instance_display < "$lock_file"
      if [ "$lock_pid" = "$parent_pid" ]; then
        HYPRLAND_INSTANCE_SIGNATURE="$(basename "$(dirname "$lock_file")")"
        export HYPRLAND_INSTANCE_SIGNATURE
        if [ -n "${instance_display:-}" ]; then
          export WAYLAND_DISPLAY="$instance_display"
        fi
        log "resolved nested instance: signature=$HYPRLAND_INSTANCE_SIGNATURE, display=${WAYLAND_DISPLAY:-unset}, lock=$lock_file"
        break
      fi
    done
    parent_pid="$(ps -o ppid= -p "$parent_pid" | tr -d ' ' || true)"
  done
fi

if [ -z "${HYPRLAND_INSTANCE_SIGNATURE:-}" ]; then
  log "failed to resolve HYPRLAND_INSTANCE_SIGNATURE"
  exit 1
fi

pid_file="${XDG_RUNTIME_DIR:?}/@dock-prefix@-$HYPRLAND_INSTANCE_SIGNATURE.pid"
dock_log="${XDG_RUNTIME_DIR:?}/@dock-prefix@-$HYPRLAND_INSTANCE_SIGNATURE.log"
dock_config_home="${XDG_CONFIG_HOME:-$HOME/.config}"
dock_config_dir="$dock_config_home/nwg-dock-hyprland"

case "${1:-start}" in
  start)
    if [ -r "$pid_file" ] && kill -0 "$(cat "$pid_file")" 2>/dev/null; then
      log "dock already running: pid=$(cat "$pid_file")"
      exit 0
    fi

    rm -f "$pid_file"
    mkdir -p "$dock_config_dir"
    if [ ! -e "$dock_config_dir/style.css" ]; then
      cp @dock-style@ "$dock_config_dir/style.css"
    fi
    log "starting dock: log=$dock_log"
    (
      printf '[%s] starting nwg-dock-hyprland: signature=%s, display=%s\n' \
        "$(date --iso-8601=seconds)" "$HYPRLAND_INSTANCE_SIGNATURE" "${WAYLAND_DISPLAY:-unset}"
      exec nwg-dock-hyprland -debug -m -r -x -s style.css -p bottom -mb 10 -i 32 \
        -c "fuzzel --config @fuzzel-config@"
    ) >> "$dock_log" 2>&1 &
    dock_pid="$!"
    echo "$dock_pid" > "$pid_file"
    sleep 1
    if kill -0 "$dock_pid" 2>/dev/null; then
      log "dock started successfully: pid=$dock_pid"
    else
      log "dock exited during startup; inspect $dock_log"
      exit 1
    fi
    ;;
  toggle)
    if [ -r "$pid_file" ] && kill -0 "$(cat "$pid_file")" 2>/dev/null; then
      log "toggling dock: pid=$(cat "$pid_file")"
      kill -s RTMIN+1 "$(cat "$pid_file")"
    else
      log "toggle requested without a running dock; starting it"
      "$0" start
    fi
    ;;
  *)
    echo "usage: $0 [start|toggle]" >&2
    exit 2
    ;;
esac
