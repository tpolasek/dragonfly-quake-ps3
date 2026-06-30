# Chocolate Quake

Chocolate Quake is a faithful source port of Quake 1 (DOS version) that preserves the
original software renderer at its original 320x200 resolution. Uses SDL2 for windowing,
input, audio, and filesystem. Builds for desktop (Linux/macOS/Windows via vcpkg) and
for PS3 homebrew (PSL1GHT/PS3DEV toolchain, powerpc64-ps3-elf target, SDL2 with RSX
backend).

## Source layout

Each Quake subsystem is its own static library under `src/<sub>/{src,include}/`. The
top-level `src/CMakeLists.txt` aggregates them into the `chocolate-quake` executable.
Key subsystems: `host` (frame loop, `Host_Init`/`Host_Frame`), `common` (filesystem,
pak loading, byte swap), `sys` (PS3 logger, sysutil callback, `Sys_Error`/`Sys_Printf`),
`renderer` (software rasterizer), `camera` (view setup), `screen` (SCR_UpdateScreen),
`video` (SDL2 window + buffer), `input` (keyboard/mouse/gamepad), `sound`, `client`,
`server`, `progs` (QuakeC VM).

## PS3 build & deploy

Run **from the host** (the scripts shell out to Docker themselves):

```bash
# Fast iteration: rebuild PPU binary, sign as NPDRM, upload EBOOT.BIN to PS3 via FTP.
# Reuses the existing build-ps3/ tree. Run a full build first (below) before the
# first dev_deploy.sh invocation -- it sanity-checks the artifact exists.
bash cmake/ps3/dev_deploy.sh

# One-time full build (configures build-ps3/ from scratch):
sg docker -c "docker run --rm --platform linux/amd64 -v \$(pwd):/build -w /build \
    hldtux/ps3dev-sdl2 bash -c '
    export PATH=/usr/local/ps3dev/ppu/bin:/usr/local/ps3dev/bin:\$PATH;
    cmake -S . -B build-ps3 -G \"Unix Makefiles\" \
        -DCMAKE_TOOLCHAIN_FILE=cmake/ps3.toolchain.cmake &&
    cmake --build build-ps3 -j\$(nproc)'"

# Package a full installable .pkg (bundles id1/ + EBOOT.BIN):
bash cmake/ps3/make_pkg.sh build-ps3/src/chocolate-quake <path-to-id1> chocolate-quake.pkg
```

Env vars for `dev_deploy.sh` (all optional, defaults shown):

```
PS3_FTP_HOST=192.168.1.245
PS3_FTP_USER=anonymous
PS3_FTP_PASS=
PS3_INSTALL_DIR=/dev_hdd0/game/CHQK00001/USRDIR
```

## PS3 runtime log

The PS3 build redirects stdout to a log file next to EBOOT.BIN. Fetch it via FTP:

```bash
curl -s --max-time 20 \
    'ftp://192.168.1.245/dev_hdd0/game/CHQK00001/USRDIR/chocolate-quake.log' \
    --user 'anonymous:'
```

To clear the log between runs, just relaunch the game -- `Sys_OpenLog` reopens with
`"w"` so the log is truncated on each launch.

## PS3-specific gotchas

- **Stack size.** The OS-spawned EBOOT main thread has only ~128 KB of stack. The
  game runs on a 2 MB worker PPU thread spawned by `main()` via `sysThreadCreate`
  (see `src/main.c`). Several Quake functions allocate large stack arrays that would
  otherwise overflow (`R_AliasDrawModel` ~88 KB, `R_RenderWorld` up to ~80 KB,
  `COM_LoadPackFile` 128 KB -- last one is `static` to avoid the issue too).
- **Base directory.** `SDL_GetBasePath()` returns NULL on PSL1GHT SDL2, so
  `Sys_GetDefaultBaseDir` hardcodes `/dev_hdd0/game/CHQK00001/USRDIR`. Change the
  `TITLE_ID` in `cmake/ps3/make_pkg.sh` and this constant together.
- **Tracing.** `SYS_TRACE(...)` is a macro (PS3-only) that does `fprintf(stdout, ...)`
  followed by `fflush` so the last successful step is captured even on hard crash.
  Defined in `src/sys/include/sys.h`. Files using it must include `sys.h`; if a
  new translation unit hits "undefined reference to SYS_TRACE" or "sys.h not found",
  add `target_include_directories(<target> PRIVATE ../sys/include)` to its
  `CMakeLists.txt`.
- **Networking.** `SDL2_net` is stubbed (see `cmake/ps3/sdl2_net_stub/`). Multiplayer
  is unavailable; demo playback and single-player work.
- **XMB / PS button.** `sysUtilRegisterCallback` is registered in `Sys_Init`;
  `Sys_XmbMenuOpen()` (worker) just reads the XMB-overlay flag, and
  `Sys_QuitRequested()` (main) is the sole pumper of `sysUtilCheckCallback`.
  Pumping lives on main because the worker can block inside `VID_PS3_Present`'s
  flip-wait and would otherwise starve event delivery. The worker parks its
  loop while the XMB overlay is up; main exits the process when it sees
  `SYSUTIL_EXIT_GAME`.
- **Renderer stack-distance check.** `R_RenderView` has a DOS-era
  `delta > 10000` guard that's `#ifndef CHOCOLATE_QUAKE_PS3`'d out -- on PS3 it
  fires spuriously because the call chain depth differs from where
  `r_stack_start` was captured in `R_Init`.
- **`dev_deploy.sh` shows full build errors on failure** (the in-container script
  captures cmake output to a temp file and cats it on non-zero exit).

## Desktop build

```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake --build build
```
