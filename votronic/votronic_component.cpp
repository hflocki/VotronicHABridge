#include "votronic_component.h"

namespace esphome {
namespace votronic {

void VotronicComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Votronic Component...");
}

void VotronicComponent::loop() {
  while (uart_bus_->available()) {
    uint8_t byte = uart_bus_->read();

    if (buffer_index_ == 0 && byte != SYNC_BYTE) {
      continue;
    }
    
    data_buffer_[buffer_index_++] = byte;

    if (buffer_index_ == VOTRONIC_FRAME_SIZE) {
      parse_data_frame();
      buffer_index_ = 0;
    }
  }
}

void VotronicComponent::parse_data_frame() {
  uint8_t checksum = 0;
  for (int i = 0; i < VOTRONIC_FRAME_SIZE - 1; i++) {
    checksum ^= data_buffer_[i];
  }

  if (checksum != data_buffer_[VOTRONIC_FRAME_SIZE - 1]) {
    ESP_LOGW(TAG, "Checksum failed! Data frame ignored.");
    return;
  }
  
  ESP_LOGD(TAG, "Received valid Votronic data frame.");

  // Bordbatteriespannung (V)
  uint16_t battery_voltage_raw = (data_buffer_[3] << 8) | data_buffer_[2];
  float battery_voltage = (float)battery_voltage_raw / 100.0;
  if (this->battery_voltage_sensor_ != nullptr) {
    this->battery_voltage_sensor_->publish_state(battery_voltage);
  }

  // Solarspannung (V)
  uint16_t solar_voltage_raw = (data_buffer_[5] << 8) | data_buffer_[4];
  float solar_voltage = (float)solar_voltage_raw / 100.0;
  if (this->solar_voltage_sensor_ != nullptr) {
    this->solar_voltage_sensor_->publish_state(solar_voltage);
  }

  // Solarstrom (A)
  int16_t solar_current_raw = (data_buffer_[7] << 8) | data_buffer_[6];
  float solar_current = (float)solar_current_raw / 10.0;
  if (this->solar_current_sensor_ != nullptr) {
    this->solar_current_sensor_->publish_state(solar_current);
  }
}

} // namespace votronic
} // namespace esphome
