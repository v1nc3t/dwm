# dwm

My build of [dwm](https://dwm.suckless.org/), the small dynamic window manager for X from [suckless](https://suckless.org/).

This repository is a fork of the GitHub mirror [Digital-Chaos/dwm](https://github.com/Digital-Chaos/dwm). The official source is [git.suckless.org/dwm](https://git.suckless.org/dwm), and updates come from there. dwm was created by Anselm R. Garbe and is maintained by the suckless community. The original code stays under the MIT/X license in `LICENSE`. The upstream `README` is kept unchanged.

Every change is its own commit on top of upstream, so you can read, revert or port one feature at a time:

```sh
git log --oneline upstream/master..master
```

## What this build adds

- Super is the modifier. Alt only launches programs. Terminal is qterminal, Super+c opens rofi.
- Gruvbox window borders. The bar is gray, and only the selected tag is orange.
- A colored status bar fed by [slstatus](https://tools.suckless.org/slstatus/): mic, volume, brightness, CPU, RAM, network, download, upload, battery, date and time. Click or scroll the fields to act on them.
- Each tag remembers its own master width.
- A bottom stack layout (`TTT`): the master on top, the other windows side by side below.
- Super+Left and Super+Right move to the previous or next tag.
- Screenshots go to the clipboard (maim and xclip).
- A second panel (HDMI or USB-C) is placed right of the laptop and scaled by `extrascale` when it is plugged in. Unplugging it turns that output off. Anything you change in arandr (open it from rofi) stays until the next plug.
- Every lit screen is warmed to 4000K (`screentemp`). No redshift needed.
- Sound goes to the headphones if present, otherwise the laptop speakers, never the monitor.
- The F row does its printed job on a plain press. Fn+F sends the real F key.

Full key list: `shortcuts.txt`.

## F row

With Fn lock off (see `fnlock.rules`), a plain press does this:

| Key | Does | The laptop sends |
| --- | --- | --- |
| F1 to F6 | mute, volume down, volume up, mic mute, brightness down, brightness up | XF86 media keys |
| F7 | swap between tile and floating | Super+p |
| F8 | airplane mode, handled by the kernel | the airplane key code |
| F9 | swap between side by side (tile) and top/bottom (bottom stack `TTT`) | XF86Tools |
| F10 | lock the screen (`dm-tool lock`) | Super+l |
| F11 | nothing in dwm, the focused app gets it | Ctrl+Alt+Tab |
| F12 | open mate-calc | XF86Calculator |

F7 and F10 send the same codes as typing Super+p and Super+l, so those two shortcuts do the same thing. That is why the master width keys are Super+Shift+h and Super+Shift+l.

## Requirements

- X11 headers and libraries: `libx11-dev`, `libxinerama-dev`, `libxft-dev`, `libfontconfig-dev`, `libxrandr-dev`
- A C compiler and make
- qterminal, rofi, maim, xclip
- PipeWire (`wpctl`), brightnessctl, NetworkManager (`nmtui`), btop
- mate-calc, LightDM (`dm-tool`), and arandr if you want a display settings app
- JetBrainsMono Nerd Font, so the bar icons render
- slstatus, built separately: [v1nc3t/slstatus](https://github.com/v1nc3t/slstatus)

The Alt launch keys also expect Firefox, Thunar, Discord, VS Code (`code`) and Spotify. A missing program only affects its own key.

## Install

The tree lives in `~/.config/dwm`:

```sh
git clone git@github.com:v1nc3t/dwm.git ~/.config/dwm
cd ~/.config/dwm
make
```

`make` copies `config.def.h` to `config.h` when `config.h` is missing, then builds `./dwm`.

dwm is not installed system-wide. The LightDM session runs `dwm`, and `~/.local/bin` comes first on the session path, so this wrapper in `~/.local/bin/dwm` starts the build:

```sh
#!/bin/sh
exec "$HOME/.config/dwm/dwm" "$@"
```

Log out and choose the dwm session. A running dwm does not reload its binary.

slstatus lives in its own tree, `~/.config/slstatus`, from [v1nc3t/slstatus](https://github.com/v1nc3t/slstatus). The path in `statusscript` in `config.def.h` must point at its binary. dwm starts it at login and stops it on quit.

Install the Fn lock rule once. It applies at every boot:

```sh
sudo cp ~/.config/dwm/fnlock.rules /etc/udev/rules.d/90-fnlock.rules
```

Fn+Esc still toggles Fn lock for the current session.

## Change the config

Edit `config.def.h`, not `config.h`. `config.h` is generated and not tracked.

```sh
rm -f config.h
make
```

Then log out and back in.

| Change | Where |
| --- | --- |
| Keys, programs, colors, bar click commands | `config.def.h` |
| Bar text, icons, order, refresh | slstatus `config.def.h` |
| How the bar is drawn and clicked | `drawstatus()` and `buttonpress()` in `dwm.c` |
| Monitor layout, scaling, warm screen | `arrangemonitors()` in `dwm.c` |

The slstatus fields and `statusclicks` must stay in the same order: mic, volume, brightness, CPU, RAM, network, download, upload, battery, date and time. Fields are separated by the byte `0x1f`.

## Update from upstream

```sh
git fetch upstream
git merge upstream/master
make
git push
```

If the remote is missing on a fresh clone, add it first:

```sh
git remote add upstream https://git.suckless.org/dwm
```

## License

MIT/X Consortium. See `LICENSE`. Upstream copyright stays with the suckless authors.
