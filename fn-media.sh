#!/bin/sh
# Make the F-row icons (volume, brightness, mic) work without holding Fn.
led=/sys/class/leds/platform::fnlock/brightness
if [ "$(id -u)" -ne 0 ]; then
	echo "run: sudo $0" >&2
	exit 1
fi
if [ ! -e "$led" ]; then
	echo "fn lock control not found: $led" >&2
	exit 1
fi
echo 1 > "$led"
cat > /etc/systemd/system/fn-media.service << 'EOF'
[Unit]
Description=Media keys without holding Fn
After=systemd-udev-trigger.service

[Service]
Type=oneshot
ExecStart=/bin/sh -c 'i=0; while [ "$i" -lt 20 ]; do if [ -e /sys/class/leds/platform::fnlock/brightness ]; then echo 1 > /sys/class/leds/platform::fnlock/brightness; exit 0; fi; i=$((i+1)); sleep 1; done; exit 1'
RemainAfterExit=yes

[Install]
WantedBy=multi-user.target
EOF
systemctl enable --now fn-media.service
