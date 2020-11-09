/**** device info *****/
#define SOFTWARE_NAME "ds18b20 tester"
#define PROJECT_PATH "\\ds18b20_test"

#ifndef VERSION_MAJOR
#define VERSION_MAJOR 0
#endif
#ifndef VERSION_MINOR
#define VERSION_MINOR 2
#endif
#ifndef VERSION_BUILD
#define VERSION_BUILD 3
#endif
#include <Arduino.h>

const String fwVersion = (const String)VERSION_MAJOR + "." + VERSION_MINOR + "." + VERSION_BUILD;
const uint32_t versionCode = (VERSION_MAJOR << 24) + (VERSION_MINOR << 16) + VERSION_BUILD;
String devId = "";
void printDevInfo();
/**** device info end *****/


#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 33
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address
void printTemperature(DeviceAddress deviceAddress);
void printAddress(DeviceAddress deviceAddress);

void setup(void)
{
  // start serial port
  Serial.begin(115200);
  printDevInfo();

  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin();
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();

  // locate devices on the bus
  Serial.print("Locating devices...");

  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  // Loop through each device, print out address
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i))
    {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();

      Serial.print("Setting resolution to ");
      Serial.println(TEMPERATURE_PRECISION, DEC);

      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
      sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);

      Serial.print("Resolution actually set to: ");
      Serial.print(sensors.getResolution(tempDeviceAddress), DEC);
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.println(" but could not detect address. Check power and cabling");
    }
  }

}

void loop(void)
{
  numberOfDevices = sensors.getDeviceCount();
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");

  // Loop through each device, print out temperature data
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i))
    {
      // Output the device ID
      Serial.print("Device #");
      Serial.print(i, DEC);
      Serial.print(" , addr: ");
      printAddress(tempDeviceAddress);
      Serial.print(" ");

      // It responds almost immediately. Let's print out the data
      printTemperature(tempDeviceAddress); // Use a simple function to print out the data
    }
    //else ghost device! Check your power requirements and cabling

  }
  delay(3000);
}

void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == DEVICE_DISCONNECTED_C)
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C: ");
  Serial.println(tempC);
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
void printDevInfo() {
  Serial.println();
  Serial.println("[INFO] device has started");
  Serial.print("DEV_ID: ");

	uint64_t chipid = ESP.getEfuseMac(); //The chip ID is essentially its MAC address(length: 6 bytes).
  for (uint8_t t = 0; t < 6; t++) {
    uint8_t b = (uint8_t) (chipid >> (t * 8));
    if ( b < 16 ) {
      devId += "0";
    }
    devId += String(b, HEX);
  }

  Serial.println(devId);
  Serial.print("Soft: ");
  Serial.print(SOFTWARE_NAME);
  Serial.print(" ver: ");
  Serial.println(fwVersion);
  Serial.print("Project path: ");
  Serial.println(PROJECT_PATH);
  Serial.println();

}
