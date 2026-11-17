#!/bin/sh
set -eu

GAME_DIR=/home/aktika/pk2

if [ "${1:-}" = "--run-game" ]; then

    # Monitor: 1024x768, 60 Hz
    xrandr --output LVDS --off \
           --output DVI-0 --primary \
           --mode 800x600 --rate 60.32

    # Bez gasenja ekrana
    xset s off
    xset -dpms
    xset s noblank

    # SDL grafika i ALSA zvuk
    export SDL_VIDEODRIVER=x11
    export SDL_AUDIODRIVER=alsa
    export SDL_RENDER_SCALE_QUALITY=0

    # Honey Bee kontroler
    export SDL_GAMECONTROLLERCONFIG='03009ce15c1a00003530000000010000,Honey Bee USB Game Pad,a:b0,b:b1,x:b2,y:b3,start:b7,back:b6,leftshoulder:b4,rightshoulder:b5,dpup:-a1,dpdown:+a1,dpleft:-a0,dpright:+a0,leftx:a0,lefty:a1,platform:Linux,'

    cd "$GAME_DIR"

    # Pokreni igru u pozadini
    ./bin/pekka-kana-2 > /home/aktika/pk2-run.log 2>&1 &
    GAME_PID=$!

    # Sacekaj da se pojavi prozor igre
    WIN=""
    i=0
    while [ "$i" -lt 30 ]; do
        WIN=$(xdotool search --onlyvisible --name "Pekka Kana 2" 2>/dev/null | head -n 1 || true)
        [ -n "$WIN" ] && break
        sleep 0.2
        i=$((i + 1))
    done

    # Razvuci prozor i postavi ga u gornji levi ugao
    if [ -n "$WIN" ]; then
        sleep 1
        xdotool windowfocus "$WIN" || true
    fi

    # Drzi Xorg sesiju aktivnom dok igra radi
    wait "$GAME_PID"
    exit $?
fi

# Pokretanje samo lokalno, kao aktika
if [ "$(id -un)" != "aktika" ]; then
    echo "Pokreni kao aktika."
    exit 1
fi

if [ "$(tty)" != "/dev/tty1" ]; then
    echo "Pokreni na FUTRO konzoli tty1."
    exit 1
fi

exec startx /home/aktika/start-pk2.sh --run-game -- :0 -nolisten tcp
