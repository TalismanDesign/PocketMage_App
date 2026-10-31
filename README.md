# PocketMage_App

The official template for making PocketMage ELF apps. Clone it (or use
"Use this template" on GitHub), rename the directory to your app name, and
build.

External PocketMage apps are ELFs that the OS firmware loads at runtime. They run to completion in their own task while the OS stands down, then return to HOME. Apps talk to the device only through the C ABI in `pm_app_api.h` (included by the SDK at compile time); the SDK C++ API is not reachable from a loaded ELF.

## What this template gives you

- `main.cpp` - a working starter that draws your app name and the clock on the e-ink, writes a line to the OLED, chirps the buzzer, and waits for a keypress before exiting. Modify it freely; the header comments explain the surface you can call.
- `Makefile` - builds the app ELF and delivery tar against the SDK.
- `assets/icon.png` - the 40x40 card icon shown in the APPLOADER grid. Replace it with your own drawing; `make` regenerates the 1-bit binary.
- `tools/make_icon.py` - converts any image to the 200-byte icon format.
- `.github/workflows/build.yml` - CI that builds and gates the app.

## Prerequisites

- A checkout of the PocketMage SDK (<https://github.com/TalismanDesign/PocketMage_SDK>)
- Python 3 (for the icon tool; needs Pillow: `python3 -m pip install pillow`)
- The ESP32-S3 xtensa toolchain. The SDK's build recipe probes the usual locations (espressif toolchains, the PlatformIO package, PATH).

## Build

```sh
export PM_SDK_ROOT=/path/to/PocketMage_SDK
make            # builds <dirname>.app.elf in build/
make pack       # also produces <dirname>.tar (app + icon)
```

`APP_NAME` defaults to the directory name. To ship under a different card name, either rename the directory or set `APP_NAME` in the Makefile.

## Gate the app (recommended)

The `pm` tool from the SDK validates every undefined symbol against the host export surface:

```sh
python3 -m pip install -e $PM_SDK_ROOT
cd /path/to/your/app
python3 -m tools.pm check --host-elf /path/to/PocketMageOS/.pio/build/PM_PRODUCTION/firmware.elf
```

Without `--host-elf` the check uses the curated export list plus the libc allowlist; with it the gate is exact.

## Install

Copy `<app>.tar` to the SD card and install it through the APPLOADER app, or place `build/<app>.app.elf` and `<app>_ICON.bin` where APPLOADER scans for them. The loader caches installs in `/apps/slotN`.

## Layout

```text
main.cpp              app entry (extern "C" int main)
Makefile              SDK build recipe
assets/icon.png       source artwork for the APPLOADER card
tools/make_icon.py    icon converter
```

## License

Apache-2.0 (see LICENSE).
