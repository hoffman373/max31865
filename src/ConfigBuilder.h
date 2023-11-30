#ifndef CONFIG_BUILDER_H
#define CONFIG_BUILDER_H

class ConfigBuilder {
 private:
  unsigned char settings = 0x0;

  void set(bool on, unsigned char field) {
    if (on) {
      settings |= field;
    } else {
      settings &= ~field;
    }
  }
  
 public:
  void SetBias(bool on) {
    set(on, 0x80);
  }

  void SetConversionMode(bool on) {
    set(on, 0x40);
  }

  void OneShot(bool on) {
    set(on, 0x20);
  }

  void ThreeWire(bool on) {
    set(on, 0x10);
  }

  void FaultStatusClear(bool on) {
    set(on, 0x02);
  }

  void FiftyHzFilter(bool on) {
    set(on, 0x01);
  }

  unsigned char GetConfig() {
    return settings;
  }
};

#endif
