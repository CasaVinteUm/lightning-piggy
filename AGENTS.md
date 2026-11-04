# Repository Guidelines

SatsConf 2024 pt-BR fork of Lightning Piggy: ESP32 ePaper piggy bank running upstream Lightning Piggy 6.4.0 code with the fork suffix `-sc24`. AI assistants: treat this file as **durable memory** — read it before working, and after finishing a session consolidate new durable lessons into it (prune contradictions, remove stale lines, keep it terse).

## Project Overview

Lightning Network piggy bank for the SatsConf Kids Area: shows a Lightning payment QR on an ePaper display, then displays incoming payments in Portuguese. Wallet source is LNBits (websocket) or Nostr Wallet Connect. Users configure it over a captive web portal. Fork-localized strings, cutting-edge device build tooling is fully Docker-based.

## Architecture & Data Flow

- Single Arduino sketch in `Main/` (`.ino` translation units share globals; no cross-module headers).
- Cooperative state machine `piggyMode` defines in `Main/Constants.h:11-32` (`PIGGYMODE_*`, 0-17). Dispatched once per `Main.ino:85-219` heartbeat.
- Payment path: LNBits websocket push (`Main/Network.ino` `parseWebsocketText`) or NWC notification (`Main/NWC.ino` `subscribeNWC`) → `prependPayment()`/`setBalance()` (`Main/Wallet.ino`) → `PIGGYMODE_STARTED_STA_RECEIVED_PAYMENTS` → ePaper repaint via `Main/Display.ino`.
- Refresh chain (states 8-17) does receive-code → status → balance → payments → update check. Periodic: LNBits 15 min, NWC 5 min; update check 24 h.
- Sleep logic `Main/WakeupAndSleep.ino`: **endless sleep is the default when `config_sleep_mode` is unset** (fork decision — kids' edition; woken by GPIO39 button or GPIO32 tilt). The 6h `DEFAULT_SLEEP_DURATION_SECONDS` path only runs after a failed WiFi connect (display shows "Dormindo 6h").
- Web portal `Main/Config.ino` serves the gzip-compressed `webconfig/index.html.gzip.h` over an ESPAsyncWebServer with basic-auth cookie; config is persisted on LittleFS at `/config.json` in the `spiffs` partition `0x390000` (see `Main/partitions.csv`).
- Update check: server `m.lightningpiggy.com` returns the upstream version; `Main/Updates.ino` strips the `-sc24` suffix from both sides before comparing so upstream releases don't flip `ATU!`.

## Key Directories

- `Main/` — firmware sketch + vendored `Main/libraries/` (uBitcoin, nostrduino, ArduinoJson, GxEPD2, TFT_eSPI, ESP_Async_WebServer, Async_TCP, arduinoWebsockets, Adafruit_GFX, U8g2, QRCode).
- `webconfig/` — Tailwind-sourced portal UI (English UI text translated to pt-BR here), py gzip/inline pipeline, Flask dev server.
- `timezones/` — `zones.csv` → generated `timezones/timezones.h` (gitignored).
- `config_partition/` — `config.json.empty` schema `{name, value, label, hint, type}` for the config partition image.
- `scripts/` — host-side flash/emulator helpers (see below).
- `Containerfile`, `docker-build.sh`, `.dockerignore`, `.devcontainer/`, `.github/workflows/devcontainer.yml` — Docker toolchain + GHCR dev container.

## Development Commands

```sh
# Whole-project build (headers + firmware binaries) — the only supported build path
./docker-build.sh                 # or: ./docker-build.sh build
./docker-build.sh shell           # interactive toolchain container
./docker-build.sh image           # rebuild Containerfile stages only

# Output binaries land in Main/build/esp32.esp32.esp32/
#   Main.ino.bin / .bootloader.bin / .partitions.bin / .merged.bin / .elf / .map
```

Generated headers are prerequisites for compile and are produced by `docker-build.sh` inside Docker (mirror manually with):

```sh
cd timezones && python3 csv_to_h.py
cd ../webconfig && npx --yes tailwindcss@3.4.17 -c tailwind.config.js -i input.css -o tailwind_output.css --minify \
  && python3 inline_css.py && python3 gzip_file_to_PROGMEM_header.py index_inlined.html index.html.gzip.h
```

Portal local dev loop (macOS port 5000 is squatted by AirPlay — run `server.py` on another port):

```sh
npx --yes tailwindcss@3.4.17 -c tailwind.config.js -i input.css -o tailwind_output.css --watch   # from webconfig/
python3 server.py   # then open http://localhost:<port>/
```

Flash on real hardware (Docker can't reach macOS serial ports — run esptool on host via `uvx esptool`):

```sh
uvx esptool --port /dev/cu.usbserial-* --chip esp32 --baud 921600 \
  --before default-reset --after soft-reset write-flash 0x10000 Main/build/esp32.esp32.esp32/Main.ino.bin
```

## Code Conventions & Common Patterns

- Version string: keep the fork suffix — `currentVersion = "6.4.0-sc24"` (SatsConf **2024** edition) bumps the base on upstream rebases but preserves `-sc24`.
- pt-BR everywhere user-facing: ePaper messages (`Main.ino`, `Interrupts.ino`), status bar (`Bat:`, `Wifi:`, `ATU!`), boot slogans (`Main/BootSlogans.ino` `bitcoinSlogans_pt`), weekdays (`Main/Constants.h` `ptWeekdays`), currency `R$` (`Main/StringHelpers.ino`), portal and login page.
- Embedded HTML served from PROGMEM must set `<meta charset="UTF-8">` and `autocapitalize="none"` on inputs, or Android renders mojibake/UPPERCASE.
- pt-BR defaults appear when a user leaves portal fields empty (`webconfig/index.html` `config_defaults` map): locale `pt`, tz `America/Sao_Paulo`, currency `BRL`, separators `.`/`,`, sleep `endless_sleep`.
- Sketch-wide config constants live in `Main/Constants.h` (timeouts, AP name `Piggy Config`, portal login `piggy`/`oinkoink`, `CONFIG_FILE`, seven-weekday arrays, `PIGGYMODE_*`).
- Never hard-reset or erase real hardware; keep config partition at `0x390000` intact between flashes.
- Git: commit titles = single capitalized phrase (e.g. `Docker firmware toolchain with dev container`), why-focused body, no conventional-commit prefixes. Prefer `git commit --fixup=<base>` for follow-up tweaks and autosquash later. Never hard-reset/checkpoint test work into git status.

## Important Files

- `Main/Main.ino` — setup/loop, state-machine dispatch.
- `Main/Constants.h` — version, modes, credentials, timings, login_html.
- `Main/Network.ino` — Wi-Fi, HTTPS, websocket.
- `Main/Wallet.ino` — wallet abstraction, payments buffer, balance.
- `Main/LNBits.ino` / `Main/NWC.ino` — wallet backends.
- `Main/Display.ino` — ePaper text/bitmap renderer.
- `Main/Config.ino` — config persistence + portal server.
- `Main/partitions.csv` — custom OTA layout (must stay byte-identical for OTA safety).
- `webconfig/index.html` — portal source of truth (pt-BR).
- `CHANGELOG.md` — terse bullet list, newest-first under `**X.Y.Z**` headings.

## Runtime/Tooling Preferences

- Docker (user runs **OrbStack**) is the only build path; arduino-cli **1.1.1** with esp32 core **3.2.0** pinned in `Containerfile`.
- Python tools on host run through `uv` / `uvx` (no system pip installs).
- tailwind CLI must be run from `webconfig/` (content paths are cwd-relative) or it outputs zero utilities.
- `.gitignore` excludes generated artifacts: `Main/build/`, `webconfig/tailwind_output.css`, `webconfig/index_inlined.html`, `webconfig/index.html.gzip.h`, `webconfig/config.json`, `timezones/timezones.h`; never commit these.

## Testing & QA

No automated test suite for the firmware. The pipeline is:
1. `./docker-build.sh` — compiles the sketch in Docker (the only automated gate, `--warnings default`).
2. **Serial monitoring** (`uv run --with pyserial python` at 115200 on `/dev/cu.usbserial-*`) on real hardware or via QEMU (`scripts/run_on_emulator.sh`, see `Emulation.md`).
3. Manual web portal QA (`webconfig/server.py`) and device flash verification.
- uBitcoin's minunit suite (`Main/libraries/uBitcoin/tests/`, host `make run`) exists but is upstream-vendored and not wired into CI.
- Before flashing, prove changed code is actually in the binary: `strings Main/build/esp32.esp32.esp32/Main.ino.elf | grep '<new literal>'`.
