#ifndef THERMOMETER_H
#define THERMOMETER_H

#include <string>
#include <optional>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "ConfigBuilder.h"

using namespace std;

class Thermometer {
 public:
  bool Initialize(string spiPath);
  std::optional<double> ReadTemperature();
  static double CelsiusToFahrenheit(double temperatureC);
  
 private:
  uint8_t readFromAddress(uint8_t address);
  bool writeToAddress(uint8_t address, uint8_t value);
  void setupTransfer();
  double resToC(unsigned short res);
  
  uint8_t tx_buf[2];
  uint8_t rx_buf[2];
  int spiFd = -1;  
  struct spi_ioc_transfer transfer;
};

#endif
