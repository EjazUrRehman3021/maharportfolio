#include <ESP8266WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#include <EEPROM.h>

// ===== WiFi =====
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"

// ===== Sinric =====
#define APP_KEY     "YOUR_APP_KEY"
#define APP_SECRET  "YOUR_APP_SECRET"

// ===== Device IDs =====
#define RELAY1_ID "DEVICE_ID_1"
#define RELAY2_ID "DEVICE_ID_2"
#define RELAY3_ID "DEVICE_ID_3"
#define RELAY4_ID "DEVICE_ID_4"

// ===== Pins =====
int relayPins[] = {D1, D2, D5, D6};
int buttonPins[] = {D3, D4, D7, D8};

bool relayState[4] = {false, false, false, false};

#define EEPROM_SIZE 10

// ===== EEPROM =====
void saveStates() {
  for (int i = 0; i < 4; i++) {
    EEPROM.write(i, relayState[i]);
  }
  EEPROM.commit();
}

void loadStates() {
  for (int i = 0; i < 4; i++) {
    relayState[i] = EEPROM.read(i);
  }
}

// ===== Google Assistant Handler =====
bool onPowerState(const String &deviceId, bool &state) {

  int index = -1;

  if (deviceId == RELAY1_ID) index = 0;
  else if (deviceId == RELAY2_ID) index = 1;
  else if (deviceId == RELAY3_ID) index = 2;
  else if (deviceId == RELAY4_ID) index = 3;

  if (index != -1) {
    relayState[index] = state;
    digitalWrite(relayPins[index], state ? LOW : HIGH);
    saveStates();
  }

  return true;
}

// ===== Setup =====
void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  loadStates();

  // Relay setup
  for (int i = 0; i < 4; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], relayState[i] ? LOW : HIGH);
  }

  // Button setup
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Sinric setup
  SinricProSwitch &r1 = SinricPro[RELAY1_ID];
  SinricProSwitch &r2 = SinricPro[RELAY2_ID];
  SinricProSwitch &r3 = SinricPro[RELAY3_ID];
  SinricProSwitch &r4 = SinricPro[RELAY4_ID];

  r1.onPowerState(onPowerState);
  r2.onPowerState(onPowerState);
  r3.onPowerState(onPowerState);
  r4.onPowerState(onPowerState);

  SinricPro.begin(APP_KEY, APP_SECRET);
}

// ===== Loop =====
void loop() {
  SinricPro.handle();

  // Manual button control
  for (int i = 0; i < 4; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      delay(200); // debounce

      relayState[i] = !relayState[i];
      digitalWrite(relayPins[i], relayState[i] ? LOW : HIGH);

      saveStates();

      // Sync with Google Assistant
      switch(i) {
        case 0: SinricPro[RELAY1_ID].sendPowerStateEvent(relayState[i]); break;
        case 1: SinricPro[RELAY2_ID].sendPowerStateEvent(relayState[i]); break;
        case 2: SinricPro[RELAY3_ID].sendPowerStateEvent(relayState[i]); break;
        case 3: SinricPro[RELAY4_ID].sendPowerStateEvent(relayState[i]); break;
      }

      while(digitalRead(buttonPins[i]) == LOW); // wait release
    }
  }
}
