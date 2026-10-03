/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10", "JetBrainsMono Nerd Font Mono:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_bg[]         = "#282828";
static const char col_fg[]         = "#ebdbb2";
static const char col_border[]     = "#3c3836";
static const char col_selbg[]      = "#d79921";
static const char col_selfg[]      = "#282828";
static const char col_selborder[]  = "#fe8019";
static const char col_barbg[]      = "#222222";
static const char col_barfg[]      = "#bbbbbb";
static const char col_barborder[]  = "#444444";
static const char *colors[][3]      = {
	/*                  fg            bg            border   */
	[SchemeNorm]    = { col_fg,       col_bg,       col_border },
	[SchemeSel]     = { col_selfg,    col_selbg,    col_selborder },
	[SchemeBarNorm] = { col_barfg,    col_barbg,    col_barborder },
	[SchemeBarSel]  = { col_selfg,    col_selborder, col_selborder },
	[SchemeMic]     = { "#b8bb26",    col_barbg,    col_barborder },
	[SchemeAlert]   = { "#fb4934",    col_barbg,    col_barborder },
	[SchemeVol]     = { "#fabd2f",    col_barbg,    col_barborder },
	[SchemeBri]     = { "#fe8019",    col_barbg,    col_barborder },
	[SchemeCpu]     = { "#83a598",    col_barbg,    col_barborder },
	[SchemeRam]     = { "#d3869b",    col_barbg,    col_barborder },
	[SchemeNet]     = { "#8ec07c",    col_barbg,    col_barborder },
	[SchemeBat]     = { "#d79921",    col_barbg,    col_barborder },
	[SchemeClk]     = { "#ebdbb2",    col_barbg,    col_barborder },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 0 fills the tile; 1 leaves gaps from size hints */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const int refreshrate = 120;  /* refresh rate (per second) for client move/resize */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#include <X11/XF86keysym.h>
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_bg, "-nf", col_fg, "-sb", col_selbg, "-sf", col_selfg, NULL };
static const char *termcmd[]  = { "qterminal", NULL };
static const char *btopcmd[]  = { "qterminal", "-e", "btop", NULL };
static const char *firefoxcmd[] = { "firefox", NULL };
static const char *filecmd[]  = { "thunar", NULL };
static const char *discordcmd[] = { "discord", NULL };
static const char *codecmd[]  = { "code", NULL };
static const char *spotcmd[]  = { "spotify", NULL };
static const char *settingscmd[] = { "xfce4-settings-manager", NULL };
static const char *mutecmd[]  = { "wpctl", "set-mute", "@DEFAULT_AUDIO_SINK@", "toggle", NULL };
static const char *voldowncmd[] = { "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "5%-", NULL };
static const char *volupcmd[] = { "wpctl", "set-volume", "-l", "1.67", "@DEFAULT_AUDIO_SINK@", "5%+", NULL };
static const char *miccmd[] = { "wpctl", "set-mute", "@DEFAULT_AUDIO_SOURCE@", "toggle", NULL };
static const char *brightdowncmd[] = { "/usr/bin/brightnessctl", "set", "5%-", NULL };
static const char *brightupcmd[] = { "/usr/bin/brightnessctl", "set", "+5%", NULL };
static const char *cpuclick[] = { "qterminal", "-e", "btop", NULL };
static const char *netclick[] = { "qterminal", "-e", "nmtui", NULL };
static const char *statusscript[] = { "/home/vincent/.config/slstatus/slstatus", NULL };
static const char *roficmd[] = { "rofi", "-show", "drun", "-theme", "/usr/share/rofi/themes/gruvbox-dark.rasi", NULL };
static const char *shotsel[] = { "maim", "--select", NULL };
static const char *shotfull[] = { "maim", NULL };

/* empty desktop is this color. dwm fills the root window. no wallpaper program. */
static const char rootbg[] = "#000000";
/* laptop panel, then the panel placed to its right. extrascale is text size: 1 native, lower is smaller. */
static const char laptopout[] = "eDP-1";
static const char extraout[] = "HDMI-1";
static const float extrascale = 0.75;
static const int screentemp = 4000;

/* same order as slstatus: mic, vol, brightness, cpu, ram, network, down, up, battery, date/time */
/* brightness stays off the extra monitor's bar; the keys still change it */
static const int statushide = 2;
static const char **statusclicks[] = {
	miccmd, NULL, NULL, cpuclick, cpuclick, netclick, NULL, NULL, NULL, NULL,
};
static const char **statusscrollup[] = {
	NULL, volupcmd, brightupcmd, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};
static const char **statusscrolldn[] = {
	NULL, voldowncmd, brightdowncmd, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};
/* widest text each slot may show; the box stays this wide so neighbours do not move */
static const char *statusslot[] = {
	"\uf131 mic unmute",
	"\uf028 vol 100%",
	"\uf185 bri 100%",
	"\uf2db cpu 100%",
	"\uf233 ram 100%",
	"\uf1eb net Polakweg 14-15",
	"\uf019 999.9 MiB/s",
	"\uf093 999.9 MiB/s",
	"\uf240 bat 100%",
	"\uf017 Thu 01 Oct  15:43",
};

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY|ControlMask,           XK_q,      quit,           {0} },
	{ MODKEY,                       XK_Left,   shiftview,      {.i = -1 } },
	{ MODKEY,                       XK_Right,  shiftview,      {.i = +1 } },
	{ MODKEY,                       XK_c,      spawn,          {.v = roficmd } },
	{ MODKEY,                       XK_u,      shot,           {.v = shotsel } },
	{ MODKEY|ControlMask,           XK_u,      shot,           {.v = shotfull } },
	{ Mod1Mask,                     XK_w,      spawn,          {.v = firefoxcmd } },
	{ Mod1Mask,                     XK_f,      spawn,          {.v = filecmd } },
	{ Mod1Mask,                     XK_d,      spawn,          {.v = discordcmd } },
	{ Mod1Mask,                     XK_c,      spawn,          {.v = codecmd } },
	{ Mod1Mask,                     XK_m,      spawn,          {.v = spotcmd } },
	{ Mod1Mask,                     XK_b,      spawn,          {.v = btopcmd } },
	{ 0,                            XF86XK_AudioMute,          spawn, {.v = mutecmd } },
	{ 0,                            XK_F1,                     spawn, {.v = mutecmd } },
	{ 0,                            XF86XK_AudioLowerVolume,   spawn, {.v = voldowncmd } },
	{ 0,                            XK_F2,                     spawn, {.v = voldowncmd } },
	{ 0,                            XF86XK_AudioRaiseVolume,   spawn, {.v = volupcmd } },
	{ 0,                            XK_F3,                     spawn, {.v = volupcmd } },
	{ 0,                            XF86XK_AudioMicMute,       spawn, {.v = miccmd } },
	{ 0,                            XK_F4,                     spawn, {.v = miccmd } },
	{ 0,                            XF86XK_MonBrightnessDown,  spawn, {.v = brightdowncmd } },
	{ 0,                            XK_F5,                     spawn, {.v = brightdowncmd } },
	{ 0,                            XF86XK_MonBrightnessUp,    spawn, {.v = brightupcmd } },
	{ 0,                            XK_F6,                     spawn, {.v = brightupcmd } },
	{ 0,                            XK_F9,                     spawn, {.v = settingscmd } },
	{ 0,                            XF86XK_Tools,              spawn, {.v = settingscmd } },
	{ 0,                            XK_Print,                  shot,  {.v = shotsel } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

