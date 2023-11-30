#include <iostream>
#include <string>

#include "Thermometer.h"

using namespace std;

int main(int argc, char** arv) {  
  try {        
    Thermometer therm;
    therm.Initialize(string("/dev/spidev0.0"));
    auto tempatureInC = therm.ReadTemperature();
    if (tempatureInC) {    
      cout << "Temp in C " << *tempatureInC << endl;
      cout << "Temp in F " << Thermometer::CelsiusToFahrenheit(*tempatureInC) << endl;
    }
    
    return 0;    
  } catch (const char* e) {
    cerr << "Failed to transfer data with SPI." << endl;
    return 2;
  }
}
