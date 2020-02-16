#include "Arduino.h"
#include "heltec.h"
#include "WiFi.h"

struct networks {
  String Name;
  int    Signal;
};

networks networks_received [20];
networks networks_sorted[20];

void setup() {
  Serial.begin(115200);               // For serial diagnostic prints
  WiFi.mode(WIFI_STA);                // Put ESP32 into Station Mode
  Heltec.display -> init();                     // Initialise the display
  Heltec.display -> setContrast(255); // If you want turn the display contrast down, 255 is maxium and 0 in minimum, in practice about 128 is OK
  Heltec.display -> flipScreenVertically();
  Heltec.display -> setFont(ArialMT_Plain_10);  // Set the Font size
}

void loop() {
  byte available_networks = WiFi.scanNetworks(); // Count the number of networks being received
  if (available_networks == 0) {
    Serial.println("no networks found");
  } else
  {
    get_and_sort_networks(available_networks);
    Heltec.display -> clear();
    Heltec.display -> drawString(0, 0, "SSID"); Heltec.display -> drawString(102, 0, "RSSI"); Heltec.display -> drawLine(0, 11, 127, 11);
    for (int i = 1; i < 7; i = i + 1) {
      Heltec.display -> drawString(0, i * 8 + 1, networks_received[i].Name + " "); // Display SSID name
      Heltec.display -> drawString(110, i * 8 + 1, String(networks_received[i].Signal)); // Display RSSI for that network
      Heltec.display -> display();
    }
  }
  delay(200); // Wait before scanning again
}

void get_and_sort_networks(byte available_networks) {
  // First get all network names and their signal strength
  for (int i = 1; i < available_networks; i++) {
    networks_received[i].Name   = String(WiFi.SSID(i)); // Display SSID name
    networks_received[i].Signal = WiFi.RSSI(i);         // Display RSSI for that network
  }
  // Now sort in order of signal strength
  String temp_name;
  int    temp_signal, iteration = 0;
  while (iteration < available_networks) { // Now bubblesort results by RSSI
    for (int i = 2; i < available_networks; i++) {
      if (networks_received[i].Signal > networks_received[i - 1].Signal) {
        temp_name   = networks_received[i - 1].Name;
        temp_signal = networks_received[i - 1].Signal;
        networks_received[i - 1].Name  = networks_received[i].Name; // Swap around values
        networks_received[i - 1].Signal = networks_received[i].Signal;
        networks_received[i].Name   = temp_name;
        networks_received[i].Signal = temp_signal;
      }
    }
    iteration++;
  }
}
