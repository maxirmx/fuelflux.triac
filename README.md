# Orange Pi Zero 2W – Waveshare RPi Relay Board – CH1 demo (C++ / libgpiod)

This is a minimal C++ demo to control an **active-low** relay input from an
Orange Pi Zero 2W using `libgpiod`.

Target pin in your setup:
- Physical pin **40**
- Signal name **PI03**
- Line offset **259** (from your `gpio readall`)

Relay board:
- Waveshare "RPi Relay Board" (active LOW)
  - ON  = drive GPIO **LOW**
  - OFF = drive GPIO **HIGH**

## Package contents
- `main.cpp` – small GPIO wrapper + demo loop
- `CMakeLists.txt` – build with CMake + pkg-config

## Install dependencies (Armbian/Debian/Ubuntu)
```bash
sudo apt-get update
sudo apt-get install -y g++ cmake pkg-config libgpiod-dev gpiod
```

## Build
```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

## Run

Default is `gpiochip0` line `259` and 1s toggle period:

```bash
sudo ./relay_demo
```

Override chip/line/period:

```bash
sudo ./relay_demo gpiochip1 259 500
```

## Notes / troubleshooting

1) **The line starts as input** (`unused input` in `gpioinfo`). The program does:
   - request line as OUTPUT
   - set initial state to **LOW** immediately (relay ON)

2) Some systems release GPIO when a short `gpioset` exits. This demo keeps the line
   requested until the program exits, so the output state is maintained.

3) To find correct chip/line on your system:
```bash
gpiofind PI03
gpioinfo | grep -n "line .*259"
```

4) Permissions: either run as root, or adjust udev rules / group membership for
`/dev/gpiochip*`.

## Safety
Switching a 240VAC induction motor can generate transients.
Use appropriate protection:
- MOV across L–N near the relay/pump
- Proper fusing / breaker
- Correct earthing (PE)

## New helper
- `init_output_high()` – request the line as output and drive **HIGH** immediately (relay OFF).
