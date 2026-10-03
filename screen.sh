#!/bin/sh
# HDMI to the right of the laptop. Reapplies only when the cable state changes.
# SCALE is the HDMI text size. 1 is native. 0.75 is smaller. 2 is twice as big.
SCALE=0.75
TEMP=4000
cache=$HOME/.cache

mkdir -p "$cache"
exec 9>"$cache/dwm-screen.lock"
if ! flock -n 9; then
	# A copy from the previous session must not block the one dwm starts at login.
	old=$(cat "$cache/dwm-screen.pid" 2>/dev/null)
	case "$(ps -p "$old" -o args= 2>/dev/null)" in
	*"/home/vincent/.config/dwm/screen.sh"*) kill "$old" 2>/dev/null || true ;;
	esac
	flock -w 2 9 || exit 0
fi
echo $$ > "$cache/dwm-screen.pid"

layout() {
	xrandr --query | awk '
		/^eDP-1 connected/ {
			for (i = 1; i <= NF; i++)
				if ($i ~ /^[0-9]+x[0-9]+\+/) { split($i, a, /[x+]/); ew = a[1] }
		}
		/^HDMI-1 connected/ {
			on = 1
			for (i = 1; i <= NF; i++)
				if ($i ~ /^[0-9]+x[0-9]+\+/) { split($i, a, /[x+]/); hx = a[3]; hy = a[4] }
		}
		END {
			if (!on) { print "off"; exit }
			if (hx+0 == ew+0 && hy+0 == 0) print "ok", hx
			else print "bad"
		}
	'
}

apply() {
	factor=$(awk "BEGIN { printf \"%.4f\", 1/${SCALE} }")
	xrandr --output HDMI-1 --auto --scale "${factor}x${factor}" --right-of eDP-1
	warm
}

# -P resets first, so repeating this does not stack. It sets every connected output.
warm() {
	redshift -m randr -P -O "$TEMP" >/dev/null 2>&1 || true
}

bus() {
	[ -s "$cache/dwm-hdmi-bus" ] && return
	ddcutil detect --brief 2>/dev/null | awk '
		/I2C bus:/ { n = $NF; sub(/.*\//, "", n); sub(/i2c-/, "", n); bus = n }
		/HDMI/ { print bus; exit }
	' > "$cache/dwm-hdmi-bus"
}

hdmi_bri() {
	b=$(cat "$cache/dwm-hdmi-bus" 2>/dev/null)
	[ -n "$b" ] || return
	v=$(ddcutil --bus "$b" --noverify getvcp 10 2>/dev/null | awk '
		/current value/ {
			cur = $0; max = $0
			sub(/.*current value = */, "", cur); sub(/,.*/, "", cur); gsub(/ /, "", cur)
			sub(/.*max value = */, "", max); gsub(/ /, "", max)
			print cur, max; exit
		}')
	[ -n "$v" ] && echo "$v" > "$cache/dwm-hdmi-bri"
}

sink_id() {
	wpctl status | awk -v w="$1" '
		/Sinks:/{s=1} /Sources:/{s=0}
		s && index($0, w) && index($0, "cAVS") {
			for (i = 1; i <= NF; i++) if ($i ~ /^[0-9]+\.$/) { sub(/\./, "", $i); print $i; exit }
		}'
}

# Laptop speakers, or headphones when the jack is in. Never the display.
# Unmute once at startup. Later mute presses stay muted.
speakers() {
	jack=$(amixer -c0 cget name='Headphone Jack' 2>/dev/null | awk -F= '/: values=/{print $2; exit}')
	if [ "$jack" = on ]; then
		want=Headphones
	else
		want=Speaker
	fi
	sink=$(sink_id "$want")
	if [ -z "$sink" ]; then
		ids=$(pw-dump | python3 -c '
import json, sys
want = sys.argv[1]
for o in json.load(sys.stdin):
    if o.get("type") != "PipeWire:Interface:Device":
        continue
    if "cAVS" not in o.get("info", {}).get("props", {}).get("device.description", ""):
        continue
    for p in o["info"]["params"].get("EnumProfile", []):
        name = p.get("name", "")
        if name.startswith("HiFi") and want in name:
            print(o["id"], p["index"])
            raise SystemExit
' "$want")
		card=${ids%% *}
		idx=${ids##* }
		[ -n "$card" ] && wpctl set-profile "$card" "$idx"
		sleep 0.3
		sink=$(sink_id "$want")
	fi
	[ -n "$sink" ] || return
	echo "$sink" > "$cache/dwm-speaker"
	cur=$(wpctl status | awk '/Sinks:/{s=1} /Sources:/{s=0} s && /\*/{print; exit}')
	case "$cur" in
	*"$want"*) ;;
	*) wpctl set-default "$sink" ;;
	esac
	if [ -z "$opened" ]; then
		amixer -c0 sset Master on >/dev/null 2>&1 || true
		amixer -c0 sset Speaker on >/dev/null 2>&1 || true
		wpctl set-mute "$sink" 0
		opened=1
	fi
}

# ponytail: poll. xrandr has no blocking hotplug wait.
# Reapply color when a screen connects. Same state does not set gamma again.
opened=
prev=
bus
hdmi_bri
want=$(awk "BEGIN { printf \"%.4f\", 1/${SCALE} }")
while true; do
	now=$(layout)
	case "$now" in
	off)
		rm -f "$cache/dwm-hdmi-x"
		;;
	ok\ *)
		echo "${now#ok }" > "$cache/dwm-hdmi-x"
		bus
		[ -s "$cache/dwm-hdmi-bri" ] || hdmi_bri
		;;
	bad)
		apply
		;;
	esac
	# Position can already be correct at login while the scale is still 1.
	if [ "$now" != off ]; then
		got=$(xrandr --verbose | awk '/^HDMI-1 /{h=1} h && /Transform:/{printf "%.4f", $2; exit}')
		if [ -n "$got" ] && [ "$got" != "$want" ]; then
			apply
		fi
	fi
	if [ "$now" != "$prev" ]; then
		warm
		prev=$now
	fi
	speakers
	sleep 2
done
