#!/bin/sh
# Laptop speakers. dwm sets that sink as the default at login.
case ${1:-show} in
show)
	wpctl get-volume @DEFAULT_AUDIO_SINK@ | awk '{p=int($2*100+0.5); if ($0 ~ /MUTED/) print "\uf026 vol mute"; else printf "\uf028 vol %d%%\n", p}'
	;;
up)
	wpctl set-volume -l 1.67 @DEFAULT_AUDIO_SINK@ 5%+
	kill -USR1 $(pidof slstatus) 2>/dev/null || true
	;;
down)
	wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%-
	kill -USR1 $(pidof slstatus) 2>/dev/null || true
	;;
mute)
	wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle
	kill -USR1 $(pidof slstatus) 2>/dev/null || true
	;;
esac
