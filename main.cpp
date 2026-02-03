#include <gpiod.h>

#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

class GpioLineOut {
public:
  GpioLineOut(std::string chip_name, unsigned int line_offset, bool active_low = false)
      : chip_name_(std::move(chip_name)), line_offset_(line_offset), active_low_(active_low) {}

  // Initialize: request the line as OUTPUT and set initial level (logical) to LOW.
  // For an active-low relay board, logical LOW means "relay ON".
  void init_output_low() {
    open_or_throw();

#if GPIOD_VERSION_MAJOR >= 2
    // libgpiod v2 API
    gpiod_line_settings* settings = gpiod_line_settings_new();
    if (!settings) throw std::runtime_error("gpiod_line_settings_new failed");

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);

    // Active-low is a line attribute in v2; it affects how values are interpreted.
    if (active_low_) {
      gpiod_line_settings_set_active_low(settings, true);
    }

    // Initial value: logical LOW
    gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE); // logical LOW

    gpiod_line_config* config = gpiod_line_config_new();
    if (!config) {
      gpiod_line_settings_free(settings);
      throw std::runtime_error("gpiod_line_config_new failed");
    }

    int rc = gpiod_line_config_add_line_settings(config, &line_offset_, 1, settings);
    gpiod_line_settings_free(settings);
    if (rc) {
      gpiod_line_config_free(config);
      throw std::runtime_error("gpiod_line_config_add_line_settings failed");
    }

    gpiod_request_config* req_cfg = gpiod_request_config_new();
    if (!req_cfg) {
      gpiod_line_config_free(config);
      throw std::runtime_error("gpiod_request_config_new failed");
    }
    gpiod_request_config_set_consumer(req_cfg, "opiz2w-relay-demo");

    request_ = gpiod_chip_request_lines(chip_, req_cfg, config);
    gpiod_request_config_free(req_cfg);
    gpiod_line_config_free(config);

    if (!request_) throw std::runtime_error("gpiod_chip_request_lines failed (permission? wrong chip/line?)");
#else
    // libgpiod v1 API
    line_ = gpiod_chip_get_line(chip_, line_offset_);
    if (!line_) throw std::runtime_error("gpiod_chip_get_line failed");

    int flags = 0;
    if (active_low_) flags |= GPIOD_LINE_REQUEST_FLAG_ACTIVE_LOW;

    // Set initial physical value to 0 (LOW).
    // If ACTIVE_LOW flag is set, '0' corresponds to logical HIGH; for most relay modules you want *physical* LOW to turn ON.
    // Therefore: do NOT use ACTIVE_LOW for relay modules unless you understand the inversion you want.
    // We keep active_low_ default false and implement inversion in on()/off().
    int rc = gpiod_line_request_output_flags(line_, "opiz2w-relay-demo", flags, 0);
    if (rc) throw std::runtime_error("gpiod_line_request_output_flags failed (permission? already requested?)");
#endif
    initialized_ = true;
  }

  void init_output_high() {
    open_or_throw();

#if GPIOD_VERSION_MAJOR >= 2
    // libgpiod v2 API
    gpiod_line_settings* settings = gpiod_line_settings_new();
    if (!settings) throw std::runtime_error("gpiod_line_settings_new failed");

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);

    // Active-low is a line attribute in v2; it affects how values are interpreted.
    if (active_low_) {
      gpiod_line_settings_set_active_low(settings, true);
    }

    // Initial value: logical HIGH
    gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_ACTIVE); // logical HIGH

    gpiod_line_config* config = gpiod_line_config_new();
    if (!config) {
      gpiod_line_settings_free(settings);
      throw std::runtime_error("gpiod_line_config_new failed");
    }

    int rc = gpiod_line_config_add_line_settings(config, &line_offset_, 1, settings);
    gpiod_line_settings_free(settings);
    if (rc) {
      gpiod_line_config_free(config);
      throw std::runtime_error("gpiod_line_config_add_line_settings failed");
    }

    gpiod_request_config* req_cfg = gpiod_request_config_new();
    if (!req_cfg) {
      gpiod_line_config_free(config);
      throw std::runtime_error("gpiod_request_config_new failed");
    }
    gpiod_request_config_set_consumer(req_cfg, "opiz2w-relay-demo");

    request_ = gpiod_chip_request_lines(chip_, req_cfg, config);
    gpiod_request_config_free(req_cfg);
    gpiod_line_config_free(config);

    if (!request_) throw std::runtime_error("gpiod_chip_request_lines failed (permission? wrong chip/line?)");
#else
    // libgpiod v1 API
    line_ = gpiod_chip_get_line(chip_, line_offset_);
    if (!line_) throw std::runtime_error("gpiod_chip_get_line failed");

    int flags = 0;
    if (active_low_) flags |= GPIOD_LINE_REQUEST_FLAG_ACTIVE_LOW;

    // Set initial physical value to 0 (LOW).
    // If ACTIVE_LOW flag is set, '0' corresponds to logical HIGH; for most relay modules you want *physical* LOW to turn ON.
    // Therefore: do NOT use ACTIVE_LOW for relay modules unless you understand the inversion you want.
    // We keep active_low_ default false and implement inversion in on()/off().
    int rc = gpiod_line_request_output_flags(line_, "opiz2w-relay-demo", flags, 1);
    if (rc) throw std::runtime_error("gpiod_line_request_output_flags failed (permission? already requested?)");
#endif
    initialized_ = true;
  }

  // Relay ON: drive physical LOW for active-low relay boards
  void on() { set_physical(active_low_relay_ ? 0 : 1); }

  // Relay OFF: drive physical HIGH for active-low relay boards
  void off() { set_physical(active_low_relay_ ? 1 : 0); }

  // Set whether the *external device* is active-low (e.g., Waveshare RPi Relay Board).
  // For your board: set to true (default).
  void set_active_low_relay(bool v) { active_low_relay_ = v; }

  ~GpioLineOut() { release(); }

  void release() {
    if (!initialized_) return;
#if GPIOD_VERSION_MAJOR >= 2
    if (request_) {
      gpiod_line_request_release(request_);
      request_ = nullptr;
    }
#else
    if (line_) {
      gpiod_line_release(line_);
      line_ = nullptr;
    }
#endif
    if (chip_) {
      gpiod_chip_close(chip_);
      chip_ = nullptr;
    }
    initialized_ = false;
  }

private:
  void open_or_throw() {
    if (chip_) return;
#if GPIOD_VERSION_MAJOR >= 2
    chip_ = gpiod_chip_open(chip_name_.c_str());
#else
    chip_ = gpiod_chip_open_by_name(chip_name_.c_str());
#endif
    if (!chip_) throw std::runtime_error("Failed to open gpio chip: " + chip_name_);
  }

  void set_physical(int value) {
    if (!initialized_) throw std::runtime_error("GPIO line not initialized; call init_output_low() first");
    if (value != 0 && value != 1) throw std::runtime_error("value must be 0 or 1");

#if GPIOD_VERSION_MAJOR >= 2
    gpiod_line_value v = (value == 0) ? GPIOD_LINE_VALUE_INACTIVE : GPIOD_LINE_VALUE_ACTIVE;
    // In v2, active_low_ affects logical interpretation; we keep it false by default.
    int rc = gpiod_line_request_set_value(request_, line_offset_, v);
    if (rc) throw std::runtime_error("gpiod_line_request_set_value failed");
#else
    int rc = gpiod_line_set_value(line_, value);
    if (rc) throw std::runtime_error("gpiod_line_set_value failed");
#endif
  }

  std::string chip_name_;
  unsigned int line_offset_;
  bool active_low_;          // line attribute (mostly unused here)
  bool active_low_relay_ = true; // external device polarity (Waveshare relay is active-low)
  bool initialized_ = false;

  gpiod_chip* chip_ = nullptr;
#if GPIOD_VERSION_MAJOR >= 2
  gpiod_line_request* request_ = nullptr;
#else
  gpiod_line* line_ = nullptr;
#endif
};

static volatile std::sig_atomic_t g_stop = 0;
static void on_sigint(int) { g_stop = 1; }

int main(int argc, char** argv) {
  // Defaults for Orange Pi Zero 2W PI03 (physical pin 40): line offset 259
  // NOTE: chip name may differ (gpiochip0/gpiochip1). We use "gpiochip0" by default.
  std::string chip = "gpiochip0";
  unsigned int line = 259;
  int period_ms = 1000;

  if (argc >= 2) chip = argv[1];
  if (argc >= 3) line = static_cast<unsigned int>(std::stoul(argv[2]));
  if (argc >= 4) period_ms = std::stoi(argv[3]);

  std::cout << "Using chip=" << chip << " line=" << line << " period_ms=" << period_ms << "\n";
  std::cout << "Relay polarity: ACTIVE-LOW (ON=drive LOW, OFF=drive HIGH)\n";

  std::signal(SIGINT, on_sigint);
  std::signal(SIGTERM, on_sigint);

  try {
    GpioLineOut pin(chip, line);
    pin.set_active_low_relay(true);

    // Initialize: make output and drive LOW (relay ON) immediately
    pin.init_output_low();
    std::cout << "Initialized output LOW (relay ON). Press Ctrl+C to stop.\n";

    bool state_on = true;
    while (!g_stop) {
      if (state_on) {
        pin.on();
        std::cout << "ON  (drive LOW)\n";
      } else {
        pin.off();
        std::cout << "OFF (drive HIGH)\n";
      }
      state_on = !state_on;
      std::this_thread::sleep_for(std::chrono::milliseconds(period_ms));
    }

    // Safe shutdown: turn relay OFF
    std::cout << "Stopping: turning relay OFF (drive HIGH)\n";
    pin.off();
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << "\n";
    std::cerr << "Hints:\n";
    std::cerr << "  - Ensure libgpiod is installed (libgpiod-dev)\n";
    std::cerr << "  - Ensure you run as root or have /dev/gpiochip* permissions\n";
    std::cerr << "  - Confirm chip/line with: gpioinfo | grep -n \"line .*259\" or gpiofind PI03\n";
    return 2;
  }
}
