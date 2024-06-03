#!/bin/sh

set -e

OVERLAYS=/sys/kernel/config/device-tree/overlays

add()
{
    if [ -e "$OVERLAYS/$1" ]; then
        echo "Overlay $1 already exists"
        exit 1
    fi

    if [ ! -e "$1.dtbo" ]; then
        echo "Overlay $1.dtbo not found"
        exit 1
    fi

    mkdir "$OVERLAYS/$1"
    cat "$1.dtbo" > "$OVERLAYS/$1/dtbo"
}

remove()
{
    rmdir "$OVERLAYS/$1"
}

cmd=add

for i in "$@"; do
    case $i in
        add)
            cmd=add
            ;;
        rm)
            cmd=remove
            ;;
        *)
            $cmd "$i"
            ;;
    esac
done
