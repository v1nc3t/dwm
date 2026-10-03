#!/bin/sh
# Brightness for the focused screen. No args prints N%. up/down changes it.
# 1 = laptop, 2 = HDMI. A failed focus lookup keeps the previous screen.
cache=$HOME/.cache
hdmi_bri=$cache/dwm-hdmi-bri
hdmi_x=$cache/dwm-hdmi-x
hdmi_bus=$cache/dwm-hdmi-bus
monf=$cache/dwm-mon
seen=$cache/dwm-mon-seen
bl=/sys/class/backlight/nvidia_wmi_ec_backlight

mon() {
	if [ -s "$monf" ]; then
		cat "$monf"
		return
	fi
	x=$(xdotool getactivewindow getwindowgeometry --shell 2>/dev/null | awk -F= '/^X=/{print $2; exit}')
	if [ -z "$x" ]; then
		cat "$seen" 2>/dev/null || echo 1
		return
	fi
	hx=$(cat "$hdmi_x" 2>/dev/null)
	if [ -n "$hx" ] && [ "$x" -ge "$hx" ]; then
		m=2
	else
		m=1
	fi
	echo "$m" > "$seen"
	echo "$m"
}

laptop() {
	b=$(cat "$bl/brightness")
	m=$(cat "$bl/max_brightness")
	echo $((b * 100 / m))
}

case ${1:-show} in
show)
	if [ "$(mon)" = 2 ]; then
		read cur max < "$hdmi_bri"
		[ -n "$max" ] || max=100
		[ -n "$cur" ] || cur=0
		v=$((cur * 100 / max))
	else
		v=$(laptop)
	fi
	[ -n "$v" ] || v=0
	printf '%s%%\n' "$v"
	;;
up|down)
	if [ "$(mon)" = 2 ]; then
		bus=$(cat "$hdmi_bus" 2>/dev/null)
		[ -n "$bus" ] || exit 0
		# Monitor scale is 0 to 10. Read the live level, then move one notch (10%).
		live=$(ddcutil --bus "$bus" --noverify getvcp 10 2>/dev/null | awk '
			/current value/ {
				cur = $0; max = $0
				sub(/.*current value = */, "", cur); sub(/,.*/, "", cur); gsub(/ /, "", cur)
				sub(/.*max value = */, "", max); gsub(/ /, "", max)
				print cur, max; exit
			}')
		if [ -n "$live" ]; then
			read cur max <<EOF
$live
EOF
		else
			read cur max < "$hdmi_bri"
		fi
		[ -n "$max" ] || max=100
		[ -n "$cur" ] || cur=0
		step=$((max / 10))
		[ "$step" -lt 1 ] && step=1
		if [ "$1" = up ]; then cur=$((cur + step)); else cur=$((cur - step)); fi
		[ "$cur" -gt "$max" ] && cur=$max
		[ "$cur" -lt 0 ] && cur=0
		ddcutil --bus "$bus" --noverify --sleep-multiplier 0.1 setvcp 10 "$cur" >/dev/null 2>&1 || exit 0
		echo "$cur $max" > "$hdmi_bri"
	else
		if [ "$1" = up ]; then /usr/bin/brightnessctl set +5% >/dev/null
		else /usr/bin/brightnessctl set 5%- >/dev/null
		fi
	fi
	kill -USR1 $(pidof slstatus) 2>/dev/null || true
	;;
esac
