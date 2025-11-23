/* Final Smart Energy Meter + PIR-controlled Relay (Active-HIGH)
   - Voltage: ZMPT101B on GPIO35 (ADC)
   - Current: SCT013 on GPIO34 (ADC)
   - PIR sensor on GPIO25 (digital input)
   - Relay module IN on GPIO23 (digital output) -- ACTIVE HIGH
   Notes:
   - Ensure common GND between ESP32 and relay module.
   - Calibrate voltageCalibration and currentCalibration.
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>
#include <WiFi.h>
#include <HTTPClient.h>

// ----- User config -----
const char* ssid = "Iphone 14 Plus";
const char* password = "1234567890";
const char* serverUrl = "https://iot.om-mishra.com/data";

#define I2C_ADDR 0x27
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

const int PIN_V = 35;      // ZMPT101B ADC (ADC1_CH7)
const int PIN_I = 34;      // SCT013 ADC (ADC1_CH6)

const int PIR_PIN = 25;    // PIR digital output
const int RELAY_PIN = 23;  // Relay module IN pin

// Relay logic: set false for active-HIGH modules (IN HIGH => relay ON).
const bool RELAY_ACTIVE_LOW = false;  // YOUR MODULE IS ACTIVE-HIGH -> set false

// ADC and calibration
const float ADC_REF = 3.3;
const float ADC_MAX = 4095.0;
float voltageCalibration = 230.0;   // calibrate this
float currentCalibration = 100.0;   // calibrate this for your CT

// Sampling settings
const int NUM_SAMPLES = 2000;
const int SAMPLE_DELAY_US = 500;

// PIR/relay timing
const unsigned long RELAY_HOLD_MS = 5000; // milliseconds to hold relay ON after last motion
unsigned long lastMotionTime = 0;
bool relayState = false; // tracks logical ON/OFF

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("\n--- Smart Meter with PIR Relay (Active-HIGH) ---");

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Smart Energy Meter");
  lcd.setCursor(0,1); lcd.print("Boot...");
  delay(800);

  // pins
  pinMode(PIR_PIN, INPUT_PULLDOWN); // PIR usually idle LOW; if not, switch to INPUT_PULLUP
  pinMode(RELAY_PIN, OUTPUT);

  // Set relay OFF explicitly depending on logic:
  if (RELAY_ACTIVE_LOW) digitalWrite(RELAY_PIN, HIGH); // HIGH => OFF for active-LOW modules
  else digitalWrite(RELAY_PIN, LOW);                   // LOW => OFF for active-HIGH modules
  relayState = false;

  Serial.println("Initialization complete. Relay forced OFF.");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

void setRelay(bool on) {
  // Write physical pin according to relay logic
  if (RELAY_ACTIVE_LOW) digitalWrite(RELAY_PIN, on ? LOW : HIGH);
  else digitalWrite(RELAY_PIN, on ? HIGH : LOW);

  relayState = on;
}

void loop() {
  // ---------- Read PIR ----------
  int pirRaw = digitalRead(PIR_PIN);
  bool motionDetected = (pirRaw == HIGH); // with INPUT_PULLDOWN, HIGH means motion

  if (motionDetected) {
    lastMotionTime = millis();
    if (!relayState) {
      setRelay(true);
      Serial.println("Motion detected -> Relay ON");
    }
  } else {
    if (relayState && (millis() - lastMotionTime > RELAY_HOLD_MS)) {
      setRelay(false);
      Serial.println("No motion timeout -> Relay OFF");
    }
  }

  // Debug: print PIR and relay pin value
  int relayOutPin = digitalRead(RELAY_PIN); // reads the pin level we write
  Serial.print("PIR raw: "); Serial.print(pirRaw);
  Serial.print(" | RelayOutPin: "); Serial.print(relayOutPin);
  Serial.print(" | RelayState(ON?): "); Serial.println(relayState ? "YES" : "NO");

  // ---------- Energy measurement ----------
  double sumV2 = 0.0, sumI2 = 0.0, sumVI = 0.0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    int rawV = analogRead(PIN_V);
    int rawI = analogRead(PIN_I);

    double voltV = (rawV / ADC_MAX) * ADC_REF;
    double voltI = (rawI / ADC_MAX) * ADC_REF;

    double v = voltV - (ADC_REF / 2.0);
    double c = voltI - (ADC_REF / 2.0);

    sumV2 += v * v;
    sumI2 += c * c;
    sumVI += v * c;

    delayMicroseconds(SAMPLE_DELAY_US);
  }

  double Vrms_meas = sqrt(sumV2 / (double)NUM_SAMPLES);
  double Irms_meas = sqrt(sumI2 / (double)NUM_SAMPLES);

  double Vrms = Vrms_meas * voltageCalibration;
  double Irms = Irms_meas * currentCalibration;

  double P_real = (sumVI / (double)NUM_SAMPLES) * (voltageCalibration * currentCalibration);
  double P_apparent = Vrms * Irms;

  // Serial debug
  Serial.print("Vrms: "); Serial.print(Vrms,2); Serial.print(" V");
  Serial.print(" | Irms: "); Serial.print(Irms,3); Serial.print(" A");
  Serial.print(" | P: "); Serial.print(P_real,2); Serial.print(" W");
  Serial.print(" | S: "); Serial.print(P_apparent,2); Serial.println(" VA");

  // Send data to server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Create JSON string manually
    String jsonPayload = "{";
    jsonPayload += "\"vrms\": " + String(Vrms, 2) + ",";
    jsonPayload += "\"irms\": " + String(Irms, 3) + ",";
    jsonPayload += "\"power\": " + String(P_real, 2) + ",";
    jsonPayload += "\"relay_state\": " + String(relayState ? "true" : "false");
    jsonPayload += "}";

    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  // LCD display
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("V:"); lcd.print(Vrms,1); lcd.print("V ");
  lcd.print("I:"); if (Irms < 10) lcd.print(" "); lcd.print(Irms,2); lcd.print("A");
  lcd.setCursor(0,1);
  lcd.print("P:"); lcd.print(P_real,1); lcd.print("W ");
  lcd.print(relayState ? "ON" : "OFF");

  delay(600); // small pause before next loop
}