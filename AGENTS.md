# AGENTS.md

## Fork goal
- Fork of upstream `danilolc/pk2` (remote `upstream`); active branch `futro-s500`, origin `mihajlov39547/pk2-futro`. Goal: repurpose a Fujitsu Futro S500 thin client as a small Pekka Kana 2 console. Prioritize the low-end Linux/X11 target when changing code.
- Futro-specific files are in `scripts/futro/`; the rest of `src/` is upstream game code.

## Build & verify (Linux target)
- `make` at repo root -> `bin/pekka-kana-2`. Needs g++ and `libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev`; builds C++11 with `-O2 -Wall` and embeds `git rev-parse --short HEAD` (build from a git checkout).
- `make clean` removes `bin/` and `build/`.
- No test suite, linter, or typecheck config exists. CI (`.travis.yml`) only runs `make && ./bin/pekka-kana-2 version`; use that plus the `dev test` smoke run below.
- Root Makefile defines `PK2_PORTABLE` and leaves `PK2_USE_ZIP` commented out (`Makefile:28`), so zipped episodes do not load on Linux by default (mingw/Android builds enable it).

## Run / debug
- CLI args (`src/main.cpp:172`): `version`, `dev`, `test <episode/level.map>`, `path <dir>`, `fps`, `speedrun`, `mobile`; unknown args exit 1.
- Fast smoke test: `./bin/pekka-kana-2 dev test "rooster island 2/level13.map"` skips intro/menu and opens the level directly.
- Without `path`, startup chdirs to the binary directory then `../res` (`src/engine/PUtils.cpp:26`), so assets come from `res/`. With `PK2_PORTABLE`, writable files (`config.txt`, `settings.ini`, `saves.dat`, `log.txt`, `scores/`, `mapstore/`) live in `res/data/`, not SDL prefs; a `redirect.txt` in that dir can relocate them.
- Linux default renderer is OpenGL (`src/engine/PRender.cpp:204`); the Futro needs `render_method: sdl` via config (`scripts/futro/config.txt`).

## Architecture / conventions
- `src/main.cpp` -> `Piste::init` (`src/engine/Piste.cpp`) -> `Piste::loop(Screen_Loop)`; screen dispatch is `src/screens/screens.cpp:177`, with one loop per screen file.
- Keep SDL2/Zlib includes inside `src/engine/`; render backends are `src/engine/render/{PSdl,PSdlSoft,PGl}`. State lives in `Episode` (`src/episode/episodeclass.hpp`) and `Game` (`src/game/game.hpp`).
- `contributing.md` asks for deliberately simple, self-explanatory C++11. `external/` is vendored third-party — do not edit.

## Futro S500 deployment
- Launcher runs as user `aktika` on `/dev/tty1`; hardcoded paths: game `/home/aktika/pk2`, launcher installed at `/home/aktika/start-pk2.sh`, output log `/home/aktika/pk2-run.log`.
- Started via `startx /home/aktika/start-pk2.sh --run-game -- :0 -nolisten tcp`. With `--run-game` it sets DVI-0 to 800x600@60.32 (LVDS off), disables DPMS/blanking, exports `SDL_VIDEODRIVER=x11`, `SDL_AUDIODRIVER=alsa`, `SDL_RENDER_SCALE_QUALITY=0`, injects `SDL_GAMECONTROLLERCONFIG` for the Honey Bee USB pad, waits for the "Pekka Kana 2" window via `xdotool`, then blocks on the game PID. Needs `xrandr`, `xset`, `xdotool`, `startx`.
- Deploy `scripts/futro/config.txt` to `/home/aktika/pk2/res/data/config.txt` (cwd becomes `res/`); the launcher is committed non-executable, so `chmod +x` on target.

## Other build variants
- Windows: run `make` inside `mingw/i686/` or `mingw/x86_64/` (include `mingw/mingw.mk`); output `PK2/` bundles DLLs and `res/` assets.
- Android: README's `git submodule` step is stale (no `.gitmodules`; libs are vendored under `external/`); `./gradlew build` in `android/` with SDK/NDK.
