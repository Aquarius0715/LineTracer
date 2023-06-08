#!/bin/sh

export MY_IME=fcitx

export PKG_CONFIG_PATH=/usr/lib/pkgconfig
export LANG=ja_JP.UTF-8
export XMODIFIERS="@im=${MY_IME}"
export GTK_IM_MODULE="${MY_IME}"
export QT_IM_MODULE="xim"

# The following commands are written in ~/shell/xfce_startup.sh
# and it is loaded by ~/.config/autostart/startup.sh.desktop
#xmodmap ${HOME}/.Xmodmap
#xkbset m
#xkbset exp m

#scim -d &
#ibus-daemon -d -x &
fcitx &

xset -dpms
xset s off

/usr/bin/startxfce4

