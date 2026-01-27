# Waveshare 2-CH TRIAC HAT — UART CLI (ON/OFF only)

A tiny C++ test application to control **Channel 1** of the Waveshare **2-CH TRIAC HAT** via **UART**.
It supports exactly two commands:

- `on1`  — turn **ON** line/channel 1
- `off1` — turn **OFF** line/channel 1

## Protocol (per Waveshare wiki)

UART frame format:

`0x57 0x68 <reg> <paramH> <paramL> <parity>`

This tool implements **parity = (sum of the previous 5 bytes) & 0xFF**.
If your board uses a different parity/checksum, adjust `calcParity()` in `src/triac_uart_cli.cpp`.

Relevant registers:

- `0x01` Mode: `0` = switch mode (on/off only), `1` = phase-angle mode
- `0x02` Channel enable: `1` = CH1 on, `0` = all off

## Build (Ubuntu)

```bash
sudo apt-get update
sudo apt-get install -y g++ cmake
cmake -S . -B build
cmake --build build -j
```

Binary will be at: `build/triac_uart_cli`

## Run

Default device is **UART5**: `/dev/ttyS5` at **115200 8N1**.

```bash
sudo ./build/triac_uart_cli on1
sudo ./build/triac_uart_cli off1
```

Optional flags:

```bash
./build/triac_uart_cli --dev /dev/ttyS5 --baud 115200 on1
```

## Permissions (recommended)

Instead of running as root, add your user to `dialout`:

```bash
sudo usermod -aG dialout $USER
# log out and log in again
```

## Safety

This board controls **mains voltage**. Use an insulated enclosure, proper wiring, and do not touch exposed circuitry.
