#include "esphome.h"
#include "esphome/core/log.h"
#include "esphome/components/uart/uart_component.h"

namespace esphome {
namespace votronic {

static const char *TAG = "votronic";
static const int VOTRONIC_FRAME_SIZE = 16;
static const uint8_t SYNC_BYTE = 0xAA;

class VotronicComponent : public Component {
 public:
  VotronicComponent(uart::UARTComponent *uart_bus) : uart_bus_(uart_bus) {}
  void set_solar_voltage_sensor(sensor::Sensor *sensor) { solar_voltage_sensor_ = sensor; }
  void set_solar_current_sensor(sensor::Sensor *sensor) { solar_current_sensor_ = sensor; }
  void set_battery_voltage_sensor(sensor::Sensor *sensor) { battery_voltage_sensor_ = sensor; }

  void setup() override;
  void loop() override;

 protected:
  void parse_data_frame();

  uart::UARTComponent *uart_bus_;
  uint8_t data_buffer_[VOTRONIC_FRAME_SIZE];
  int buffer_index_ = 0;
  
  sensor::Sensor *solar_voltage_sensor_ = nullptr;
  sensor::Sensor *solar_current_sensor_ = nullptr;
  sensor::Sensor *battery_voltage_sensor_ = nullptr;
};

} // namespace votronic
} // namespace esphome
