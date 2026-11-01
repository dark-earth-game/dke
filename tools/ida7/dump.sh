#!/bin/sh
# Dump dkev.exe from IDA Pro 7.0 (has the 32-bit Hex-Rays decompiler; no Python needed).
# Output: disassembly/ida7/{dkev.c,dkev.asm,dkev.lst,c/,asm/,index.json,dkev_ida7.idb}
# names.idc/types.h replay the names and structs of the IDA 9 database (dkev.i64).
set -e
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
T="$ROOT/tools/ida7"; O="$ROOT/disassembly/ida7"; W=$(mktemp -d)
IDAT=${IDAT:-"/Applications/IDA Pro 7.0/ida.app/Contents/MacOS/idat"}
export TVHEADLESS=1
mkdir -p "$O"; cp "$ROOT/disassembly/dkev.exe" "$W/"
sed "s#\"types.h\"#\"$T/types.h\"#" "$T/names.idc" > "$W/names.idc"
sed "s#\"out/#\"$O/#g" "$T/dump_asm.idc" > "$W/dump_asm.idc"
"$IDAT" -B -L"$W/a.log" "$W/dkev.exe"                       # auto-analysis -> dkev.idb
"$IDAT" -A -S"$W/names.idc" -L"$W/n.log" "$W/dkev.idb"
"$IDAT" -A -S"$W/dump_asm.idc" -L"$W/d.log" "$W/dkev.idb"
"$IDAT" -A -Ohexrays:"$O/dkev.c":ALL -L"$W/h.log" "$W/dkev.idb"
cp "$W/dkev.idb" "$O/dkev_ida7.idb"
python3 "$T/split.py" "$O"
rm -rf "$W"
