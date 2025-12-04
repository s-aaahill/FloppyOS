#!/usr/bin/env bash
IMG="build/main_floppy.img"
if [ ! -f "$IMG" ]; then
echo "ERROR: image not found: $IMG"
exit 1
fi
qemu-system-i386 -fda "$IMG" -boot a -vga std -serial file:serial.log