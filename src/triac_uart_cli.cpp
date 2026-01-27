\
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <termios.h>
#include <unistd.h>

namespace {

constexpr uint8_t kHdr1 = 0x57;
constexpr uint8_t kHdr2 = 0x68;

// Registers from Waveshare wiki
constexpr uint8_t REG_MODE = 0x01;   // 0 = switch mode, 1 = phase-angle mode
constexpr uint8_t REG_EN   = 0x02;   // 0..3 channel enable bits (1=CH1,2=CH2,3=both)

speed_t baudToTermios(int baud) {
    switch (baud) {
        case 1200: return B1200;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        case 460800: return B460800;
        case 921600: return B921600;
        default:
            throw std::runtime_error("Unsupported baud for termios: " + std::to_string(baud));
    }
}

int openUart(const std::string& dev, int baud) {
    int fd = ::open(dev.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        throw std::runtime_error("open(" + dev + "): " + std::string(std::strerror(errno)));
    }

    termios tty{};
    if (tcgetattr(fd, &tty) != 0) {
        ::close(fd);
        throw std::runtime_error("tcgetattr: " + std::string(std::strerror(errno)));
    }

    // Raw mode
    cfmakeraw(&tty);

    // 8N1
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD); // no parity
    tty.c_cflag &= ~CSTOPB;           // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;          // no HW flow control

    // Read behavior: return as soon as any data arrives or after timeout (not used much here)
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 5; // 0.5s

    speed_t spd = baudToTermios(baud);
    cfsetispeed(&tty, spd);
    cfsetospeed(&tty, spd);

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        ::close(fd);
        throw std::runtime_error("tcsetattr: " + std::string(std::strerror(errno)));
    }

    // Flush any pending I/O
    tcflush(fd, TCIOFLUSH);
    return fd;
}

// Waveshare calls the last byte "Parity Bit" in the wiki.
// The wiki doesn't define the exact algorithm on that page.
// This implementation uses a common embedded convention: 8-bit sum of the first 5 bytes.
uint8_t calcParity(uint8_t reg, uint8_t paramH, uint8_t paramL) {
    uint16_t sum = static_cast<uint16_t>(kHdr1) + kHdr2 + reg + paramH + paramL;
    return static_cast<uint8_t>(sum & 0xFF);
}

void writeRegister16(int fd, uint8_t reg, uint16_t value) {
    uint8_t paramH = static_cast<uint8_t>((value >> 8) & 0xFF);
    uint8_t paramL = static_cast<uint8_t>(value & 0xFF);
    uint8_t parity = calcParity(reg, paramH, paramL);

    uint8_t frame[6] = {kHdr1, kHdr2, reg, paramH, paramL, parity};

    ssize_t n = ::write(fd, frame, sizeof(frame));
    if (n != static_cast<ssize_t>(sizeof(frame))) {
        throw std::runtime_error("UART write failed: wrote " + std::to_string(n) +
                                 " bytes, errno=" + std::to_string(errno) + " (" +
                                 std::string(std::strerror(errno)) + ")");
    }
}

[[noreturn]] void usage(const char* argv0) {
    std::cerr
        << "Usage:\n"
        << "  " << argv0 << " [--dev /dev/ttyS5] [--baud 115200] <on1|off1>\n\n"
        << "Defaults:\n"
        << "  --dev  /dev/ttyS5   (UART5)\n"
        << "  --baud 115200       (8N1)\n";
    std::exit(2);
}

} // namespace

int main(int argc, char** argv) {
    std::string dev = "/dev/ttyS5";  // UART5 default as requested
    int baud = 115200;

    static struct option long_opts[] = {
        {"dev",  required_argument, nullptr, 'd'},
        {"baud", required_argument, nullptr, 'b'},
        {"help", no_argument,       nullptr, 'h'},
        {nullptr, 0, nullptr, 0}
    };

    while (true) {
        int c = getopt_long(argc, argv, "d:b:h", long_opts, nullptr);
        if (c == -1) break;
        switch (c) {
            case 'd': dev = optarg; break;
            case 'b': baud = std::stoi(optarg); break;
            case 'h': usage(argv[0]);
            default: usage(argv[0]);
        }
    }

    if (optind >= argc) usage(argv[0]);
    std::string cmd = argv[optind];
    if (cmd != "on1" && cmd != "off1") usage(argv[0]);

    try {
        int fd = openUart(dev, baud);

        // Put device into Switch Mode (on/off only)
        writeRegister16(fd, REG_MODE, 0);

        // Enable/disable CH1 only.
        // According to Waveshare register definition:
        //   1 = enable CH1, disable CH2
        //   0 = disable both
        uint16_t en = (cmd == "on1") ? 1 : 0;
        writeRegister16(fd, REG_EN, en);

        ::close(fd);
        std::cout << "OK: CH1 " << ((cmd == "on1") ? "ON" : "OFF")
                  << " via " << dev << " @ " << baud << "\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
}
