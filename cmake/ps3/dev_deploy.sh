#!/usr/bin/env bash
#
# Fast PS3 dev iteration: rebuild the PPU binary, sign it as NPDRM, and
# upload EBOOT.BIN over FTP to the running PS3. No .pkg rebuild, no
# reinstall -- just hotswap the executable.
#
# Run this FROM THE HOST (it shells out to docker itself). Usage:
#
#   ./cmake/ps3/dev_deploy.sh
#
# Override the destination by setting env vars:
#
#   PS3_FTP_HOST=192.168.1.245 \
#   PS3_FTP_USER=anonymous \
#   PS3_FTP_PASS= \
#   PS3_INSTALL_DIR=/dev_hdd0/game/CHQK00001/USRDIR \
#   ./cmake/ps3/dev_deploy.sh

set -euo pipefail

# --- config (override via env) ------------------------------------------------
PS3_FTP_HOST="${PS3_FTP_HOST:-192.168.1.245}"
PS3_FTP_USER="${PS3_FTP_USER:-anonymous}"
PS3_FTP_PASS="${PS3_FTP_PASS:-}"
PS3_INSTALL_DIR="${PS3_INSTALL_DIR:-/dev_hdd0/game/CHQK00001/USRDIR}"
CONTENT_ID="${CONTENT_ID:-UP0000-CHQK00001_00-0000000000000001}"
LOCAL_ELF="${LOCAL_ELF:-build-ps3/src/chocolate-quake}"

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$REPO_ROOT"

# --- sanity checks ------------------------------------------------------------
[ -f "$LOCAL_ELF" ] || {
    echo "Missing $LOCAL_ELF -- run a full PS3 build first." >&2
    exit 1
}
command -v docker >/dev/null || {
    echo "docker not found on PATH" >&2
    exit 1
}

# --- build, sign, upload -- all inside the container --------------------------
# We mount the repo so the container can rebuild and read the binary, and
# pass our config in via env vars so the in-container script can use them.
DOCKER=(docker run --rm --platform linux/amd64
    -v "$REPO_ROOT:/build" -w /build
    -e PS3_FTP_HOST -e PS3_FTP_USER -e PS3_FTP_PASS
    -e PS3_INSTALL_DIR -e CONTENT_ID -e LOCAL_ELF
    hldtux/ps3dev-sdl2)

if [ "$(id -nG "$USER" | tr ' ' '\n' | grep -cx docker)" -eq 0 ] && \
   [ "$(id -u)" -ne 0 ]; then
    # Not in docker group and not root: wrap with sg.
    DOCKER=(sg docker -c "${DOCKER[*]}")
fi

echo "[1/3] Rebuilding PPU binary"
"${DOCKER[@]}" bash -c '
    set -e
    export PATH=/usr/local/ps3dev/ppu/bin:/usr/local/ps3dev/bin:$PATH
    cmake --build build-ps3 -j"$(nproc)" 2>&1 | tail -3
'

echo "[2/3] Signing EBOOT.BIN (ppu-strip + sprxlinker + make_self_npdrm)"
"${DOCKER[@]}" bash -c '
    set -e
    export PATH=/usr/local/ps3dev/ppu/bin:/usr/local/ps3dev/bin:$PATH
    rm -f /tmp/cq.elf /tmp/EBOOT.BIN
    ppu-strip -o /tmp/cq.elf "$LOCAL_ELF"
    sprxlinker /tmp/cq.elf
    make_self_npdrm /tmp/cq.elf /tmp/EBOOT.BIN "$CONTENT_ID"
    echo "  -> /tmp/EBOOT.BIN ($(stat -c%s /tmp/EBOOT.BIN) bytes)"
'

echo "[3/3] Uploading to ftp://$PS3_FTP_HOST$PS3_INSTALL_DIR/EBOOT.BIN"
"${DOCKER[@]}" bash -c "
    set -e
    curl --connect-timeout 10 --max-time 120 --ftp-pasv \
        -T /tmp/EBOOT.BIN \
        --user \"$PS3_FTP_USER:$PS3_FTP_PASS\" \
        \"ftp://$PS3_FTP_HOST$PS3_INSTALL_DIR/EBOOT.BIN\"
"

echo
echo "Deployed. Relaunch Chocolate Quake from the XMB to test."
echo "Log will be at: ftp://$PS3_FTP_HOST$PS3_INSTALL_DIR/chocolate-quake.log"
