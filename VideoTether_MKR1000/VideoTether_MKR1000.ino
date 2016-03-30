#include <AccelStepper.h>
#include <MultiStepper.h>

#include <SPI.h>
#include <WiFi101.h>

char ssid[] = "wifiNetwork"; // your network SSID (name)
char pass[] = "wifiPassword"; // your network password
int keyIndex = 0; // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);

AccelStepper stepper(1, 9, 8);

float distance = 0;
float distance_mm_multiplier = 25.4;
float duration = 0;
float duration_sec_multiplier = 60;

void setup() {
  setup_stepper();
  setup_wifi();
}

void loop() {
  stepper.runSpeedToPosition();
  WebListenForClients();
}





