#include "Thermometer.h"

#include <iostream>

using namespace std;

unsigned short resistanceToCelcius[]{
    10000, 10039, 10078, 10117, 10156, 10195, 10234, 10273, 10312, 10351,
    10390, 10429, 10468, 10507, 10546, 10585, 10624, 10663, 10702, 10740,
    10779, 10818, 10857, 10896, 10935, 10973, 11012, 11051, 11090, 11129,
    11167, 11206, 11245, 11283, 11322, 11361, 11400, 11438, 11477, 11515,
    11554, 11593, 11631, 11670, 11708, 11747, 11786, 11824, 11863, 11901,
    11940, 11978, 12017, 12055, 12094, 12132, 12171, 12209, 12247, 12286,
    12324, 12363, 12401, 12439, 12478, 12516, 12554, 12593, 12631, 12669,
    12708, 12746, 12784, 12822, 12861, 12899, 12937, 12975, 13013, 13052,
    13090, 13128, 13166, 13204, 13242, 13280, 13318, 13357, 13395, 13433,
    13471, 13509, 13547, 13585, 13623, 13661, 13699, 13737, 13775, 13813};

bool Thermometer::Initialize(string spiDevPath) {
  this->spiFd = spiFd;
  
  // Configure SPI transfer data.
  setupTransfer();

  this->spiFd = open(spiDevPath.c_str(), O_RDWR);

  if (this->spiFd < 0) {
    cerr << "Failed to open /dev/spidev0.0" << endl;
    return false;
  }

  int wr_mode = SPI_MODE_1;
  int rd_mode = SPI_MODE_1;
  // According to https://github.com/raspberrypi/pico-sdk/issues/941 SPI on the PI
  // only is able to send more than 1 word of data at a time using mode 1.
  if (ioctl(this->spiFd, SPI_IOC_WR_MODE, &wr_mode) < 0 ||
      ioctl(this->spiFd, SPI_IOC_RD_MODE, &rd_mode) < 0 ) {
    cerr << "Failed to set mode." << endl;
    return false;
  }

  ConfigBuilder builder;
  builder.SetBias(true);
  builder.SetConversionMode(true);
  builder.ThreeWire(true);
  builder.FaultStatusClear(true);
  uint8_t configByte = builder.GetConfig();

  #if DEBUG
  cout << hex << "Config: " << (ushort)configByte << endl;
  #endif
  try {
    writeToAddress(0x80, configByte);

    #if DEBUG
    // Check that the configuration matches what we wrote.
    readFromAddress(0x00);
    #endif
    
  } catch(const char* e) {
    cerr << "Failed to transfer data with SPI." << endl;
    return false;
  }
  
  return true;
}

optional<double> Thermometer::ReadTemperature() {
  uint16_t high = readFromAddress(0x01);
  uint8_t low = readFromAddress(0x02);
  
  high = high << 8;
  high |= low;
  bool hasFault = high & 0x01;
  
  if (hasFault) {
    cerr << "Fault returned by thermocouple." << endl;
    return nullopt;
  } else {
    // Shift to the right to throw away the fault flag.
    high = high >> 1;
    
#if DEBUG
    cout << hex << "Status: " << high << endl;
#endif
    
    unsigned int resistance = high;
    resistance *= 43000;
    resistance = resistance >> 15;
    
    auto tempatureInC = resToC(resistance);
    return tempatureInC;
  }
}

void Thermometer::setupTransfer() { 
  transfer.tx_buf = (unsigned long)this->tx_buf;
  transfer.rx_buf = (unsigned long)this->rx_buf;
  transfer.len = 2;
  transfer.speed_hz = 750000;
  transfer.bits_per_word = 8;
  transfer.delay_usecs = 0;
  transfer.cs_change = 0;
  transfer.tx_nbits = 0;
  transfer.rx_nbits = 0;
  transfer.pad = 0;
}

bool Thermometer::writeToAddress(uint8_t address, uint8_t value) {
  this->tx_buf[0] = address;
  this->tx_buf[1] = value;
  this->rx_buf[0] = 0x0;
  this->rx_buf[1] = 0x0;
  int status = ioctl(this->spiFd, SPI_IOC_MESSAGE(1), &this->transfer);
  if (status < 0) {
    cerr << "SPI send failed." << endl;
    throw "SPI send failed.";
  }
  #if DEBUG
  else {
    cout << hex << "Sent: " << (int)this->tx_buf[0] << (int)this->tx_buf[1] << " Received: " << (int)this->rx_buf[0] << (int)this->rx_buf[1]  << endl;
  }
  #endif

  return true;
}

uint8_t Thermometer::readFromAddress(uint8_t address) {
  this->tx_buf[0] = address;
  this->tx_buf[1] = 0x0;
  this->rx_buf[0] = 0x0;
  this->rx_buf[1] = 0x0;
  int status = ioctl(this->spiFd, SPI_IOC_MESSAGE(1), &this->transfer);
  if (status < 0) {
    cerr << "SPI send failed." << endl;
    throw "SPI send failed.";
  }
  #if DEBUG
  else {    
    cout << hex << "Sent: " << (int)this->tx_buf[0] << (int)this->tx_buf[1] << " Received: " << (int)this->rx_buf[0] << (int)this->rx_buf[1]  << endl;    
  }
  #endif

  return this->rx_buf[1];
}

double Thermometer::resToC(unsigned short res) {
  int before=0;
  int after=100;
  while (after - before > 1) {
    int mid = (after + before) / 2;
    if (resistanceToCelcius[mid] > res) {
      after = mid;      
    } else {
      before = mid;
    }
  }

  double extra = res - resistanceToCelcius[before];
  double range = resistanceToCelcius[after] - resistanceToCelcius[before];
  double fractional = extra / range;
  
  return before + fractional;
}

double Thermometer::CelsiusToFahrenheit(double temperatureInC) {
  return (temperatureInC * 9.0) / 5.0 + 32.0;
}
