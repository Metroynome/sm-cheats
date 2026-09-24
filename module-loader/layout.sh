#!/bin/sh
set -eu
start=$((0xf0000))
for region in ntscu pal ntscj; do
    end=$("${EE_PREFIX:-ee-}nm" "bin/module-loader.$region.elf" | awk '$3 == "_payload_end" { print $1 }')
    test -n "$end"
    end=$(( (0x$end + 63) & ~63 ))
    if [ "$end" -gt "$start" ]; then start=$end; fi
done
test "$start" -lt $((0xfe000)) || { echo "Loader overlaps state" >&2; exit 1; }
mkdir -p modules
printf 'MODULE_LOW_START = 0x%08x\nMODULE_LOW_END = 0x000fe000\nMODULE_HEAP_START = 0x00100000\nMODULE_HEAP_END = 0x00140000\n' "$start" > modules/layout.mk
printf 'Module layout: low 0x%08x-0x000fe000, heap 0x00100000-0x00140000\n' "$start"
