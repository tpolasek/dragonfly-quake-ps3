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

# --- write the in-container script --------------------------------------------
# We do everything (build + sign + upload) in ONE container invocation so
# intermediate files (/tmp/EBOOT.BIN) survive between steps. Write the
# script to a host file and mount it read-only into the container.
IN_CONTAINER_SCRIPT="$(mktemp /tmp/cq_deploy.XXXXXX.sh)"
trap 'rm -f "$IN_CONTAINER_SCRIPT"' EXIT

# Substitution happens here on the HOST so the container sees the final
# values directly. Use ${var@Q} to safely quote each value into the script.
cat > "$IN_CONTAINER_SCRIPT" <<EOF
set -euo pipefail
export PATH=/usr/local/ps3dev/ppu/bin:/usr/local/ps3dev/bin:\$PATH

echo "[1/3] Rebuilding PPU binary"
cmake --build build-ps3 -j"\$(nproc)" 2>&1 | tail -3

echo "[2/3] Signing EBOOT.BIN (ppu-strip + sprxlinker + make_self_npdrm)"
rm -f /tmp/cq.elf /tmp/EBOOT.BIN
ppu-strip -o /tmp/cq.elf ${LOCAL_ELF@Q}
sprxlinker /tmp/cq.elf
make_self_npdrm /tmp/cq.elf /tmp/EBOOT.BIN ${CONTENT_ID@Q}
ls -la /tmp/EBOOT.BIN

echo "[3/3] Uploading to ftp://${PS3_FTP_HOST}${PS3_INSTALL_DIR}/EBOOT.BIN"
curl --connect-timeout 10 --max-time 120 --ftp-pasv \\
    -T /tmp/EBOOT.BIN \\
    --user ${PS3_FTP_USER@Q}:${PS3_FTP_PASS@Q} \\
    "ftp://${PS3_FTP_HOST}${PS3_INSTALL_DIR}/EBOOT.BIN"
echo "Upload OK."
EOF
chmod +x "$IN_CONTAINER_SCRIPT"

# --- run it in docker ---------------------------------------------------------
# --network host so the container can reach the PS3 on the LAN without
# bridge-NAT complications (the default bridge usually works, but host
# networking removes a variable when debugging FTP issues).
DOCKER_CMD=(docker run --rm --platform linux/amd64
    --network host
    -v "$REPO_ROOT:/build" -w /build
    -v "$IN_CONTAINER_SCRIPT:/deploy.sh:ro"
    hldtux/ps3dev-sdl2
    bash /deploy.sh)

if docker version >/dev/null 2>&1; then
    "${DOCKER_CMD[@]}"
else
    # User's effective gid set doesn't include docker yet (no re-login since
    # usermod). Wrap the whole command with sg so the supplementary group is
    # active for this invocation only. printf %q re-quotes each argv element
    # so the string can be re-evaluated by sg's inner bash without surprises.
    exec sg docker -c "$(printf '%q ' "${DOCKER_CMD[@]}")"
fi
