#define BLYNK_TEMPLATE_ID "TMPL6vjXFS8in"
#define BLYNK_TEMPLATE_NAME "TEKRA 2026"
#define BLYNK_AUTH_TOKEN "-QGSw3E8HqxuITsVrYx6oGR-4KgNybB6"

// =====================================================
// LIBRARY
// =====================================================

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>

#include <PZEM004Tv30.h>

// =====================================================
// WIFI & BLYNK
// =====================================================

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "Wokwi-GUEST";

char pass[] = "";

// =====================================================
// PIN CONFIGURATION
// =====================================================

#define PIR_PIN        13
#define DHT_PIN        14

#define RELAY_PIN      26

// I2C BH1750
#define SDA_PIN        21
#define SCL_PIN        22

// UART PZEM
#define PZEM_RX        16
#define PZEM_TX        17

// =====================================================
// SENSOR OBJECT
// =====================================================

DHT dht(DHT_PIN, DHT22);

BH1750 lightMeter;

// =====================================================
// PZEM SERIAL STABLE FOR ESP32
// =====================================================

HardwareSerial PZEMSerial(2);

PZEM004Tv30 pzem(PZEMSerial, PZEM_RX, PZEM_TX);

BlynkTimer timer;

// =====================================================
// GREEN CLASSROOM VARIABLE
// =====================================================

bool classActive = false;

bool relayManualMode = false;

unsigned long classStartTime = 0;

unsigned long lastMotionTime = 0;

unsigned long lastSerialLog = 0;

// Durasi kelas 100 menit
const unsigned long classDuration = 6000000UL;

// Delay mati otomatis 5 menit
const unsigned long emptyTimeout = 300000UL;

// =====================================================
// FUNCTION : RELAY CONTROL
// =====================================================

void setRelay(bool state) {

  static bool lastRelayState = LOW;

  if (state != lastRelayState) {

    digitalWrite(RELAY_PIN, state);

    lastRelayState = state;

    Serial.print("Relay -> ");

    if (state) {

      Serial.println("ON");

    } else {

      Serial.println("OFF");
    }
  }
}

// =====================================================
// FUNCTION : CHECK CONNECTION
// =====================================================

void checkConnection() {

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WiFi disconnected");

    WiFi.begin(ssid, pass);
  }

  if (!Blynk.connected()) {

    Serial.println("Blynk disconnected");

    Blynk.connect();
  }
}

// =====================================================
// FUNCTION : GREEN CLASSROOM
// =====================================================

void greenClassroomControl(int pirStatus) {

  if (relayManualMode) return;

  // ==========================================
  // DETEKSI AWAL KELAS
  // ==========================================

  if (pirStatus == HIGH && !classActive) {

    classActive = true;

    classStartTime = millis();

    lastMotionTime = millis();

    setRelay(HIGH);

    Serial.println();
    Serial.println("=================================");
    Serial.println("KELAS DIMULAI");
    Serial.println("Durasi : 100 menit");
    Serial.println("=================================");
  }

  // ==========================================
  // SAAT KELAS BERLANGSUNG
  // ==========================================

  if (classActive) {

    if ((millis() - classStartTime) <= classDuration) {

      setRelay(HIGH);

      if (pirStatus == HIGH) {

        lastMotionTime = millis();
      }
    }

    else {

      if (pirStatus == HIGH) {

        lastMotionTime = millis();

        setRelay(HIGH);
      }

      if ((millis() - lastMotionTime) > emptyTimeout) {

        setRelay(LOW);

        classActive = false;

        Serial.println();
        Serial.println("=================================");
        Serial.println("KELAS KOSONG");
        Serial.println("Relay dimatikan otomatis");
        Serial.println("=================================");
      }
    }
  }
}

// =====================================================
// FUNCTION : SEND SENSOR DATA
// =====================================================

void sendSensorData() {

  // ==========================================
  // DHT22
  // ==========================================

  float temperature = dht.readTemperature();

  float humidity = dht.readHumidity();

  if (isnan(temperature)) temperature = 0;

  if (isnan(humidity)) humidity = 0;

  temperature = round(temperature * 10) / 10.0;

  humidity = round(humidity * 10) / 10.0;

  // ==========================================
  // PIR
  // ==========================================

  int pirStatus = digitalRead(PIR_PIN);

  // ==========================================
  // BH1750
  // ==========================================

  float lux = lightMeter.readLightLevel();

  if (lux < 0 || isnan(lux)) {

    lux = 0;
  }

  int lightPercent = map((int)lux, 0, 2000, 0, 100);

  lightPercent = constrain(lightPercent, 0, 100);
  
  // ==========================================
  // PZEM
  // ==========================================

  float voltage = pzem.voltage();

  float current = pzem.current();

  float power = pzem.power();

  float energy = pzem.energy();

  float frequency = pzem.frequency();

  float pf = pzem.pf();

  // ==========================================
  // VALIDASI DATA PZEM
  // ==========================================

  if (isnan(voltage)) {

    voltage = 0;

    Serial.println("ERROR: Tegangan PZEM tidak terbaca");
  }

  if (isnan(current)) current = 0;

  if (isnan(power)) power = 0;

  if (isnan(energy)) energy = 0;

  if (isnan(frequency)) frequency = 0;

  if (isnan(pf)) pf = 0;

  // ==========================================
  // GREEN CLASSROOM CONTROL
  // ==========================================

  greenClassroomControl(pirStatus);

  // ==========================================
  // SERIAL MONITOR
  // ==========================================

  if (millis() - lastSerialLog > 5000) {

    lastSerialLog = millis();

    Serial.println();
    Serial.println("========== GREEN CLASSROOM ==========");

    Serial.print("Temperature   : ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity      : ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Lux           : ");
    Serial.print(lux);
    Serial.println(" lux");

    Serial.print("Light         : ");
    Serial.print(lightPercent);
    Serial.println(" %");

    Serial.print("Motion        : ");
    Serial.println(pirStatus);

    Serial.print("Relay         : ");

    if (digitalRead(RELAY_PIN)) {

      Serial.println("ON");

    } else {

      Serial.println("OFF");
    }

    Serial.print("Voltage       : ");
    Serial.print(voltage);
    Serial.println(" V");

    Serial.print("Current       : ");
    Serial.print(current);
    Serial.println(" A");

    Serial.print("Power         : ");
    Serial.print(power);
    Serial.println(" W");

    Serial.print("Energy        : ");
    Serial.print(energy);
    Serial.println(" kWh");

    Serial.print("Frequency     : ");
    Serial.print(frequency);
    Serial.println(" Hz");

    Serial.print("Power Factor  : ");
    Serial.println(pf);

    Serial.println("=====================================");
  }

  // ==========================================
  // BLYNK UPDATE
  // ==========================================

  if (Blynk.connected()) {

    Blynk.virtualWrite(V1, temperature);

    Blynk.virtualWrite(V2, humidity);

    Blynk.virtualWrite(V3, lightPercent);

    Blynk.virtualWrite(V4, lux);

    Blynk.virtualWrite(V5, pirStatus);

    Blynk.virtualWrite(V6, digitalRead(RELAY_PIN));

    Blynk.virtualWrite(V7, voltage);

    Blynk.virtualWrite(V8, current);

    Blynk.virtualWrite(V9, power);

    Blynk.virtualWrite(V10, energy);

    Blynk.virtualWrite(V11, frequency);

    Blynk.virtualWrite(V12, pf);

    Blynk.virtualWrite(V14, classActive);
  }
}

// =====================================================
// MANUAL RELAY CONTROL
// =====================================================

BLYNK_WRITE(V13) {

  int relayState = param.asInt();

  relayManualMode = true;

  setRelay(relayState);

  if (relayState == HIGH) {

    lastMotionTime = millis();

    Serial.println("Relay ON manual");

  } else {

    Serial.println("Relay OFF manual");
  }
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  // ==========================================
  // PIN MODE
  // ==========================================

  pinMode(PIR_PIN, INPUT);

  pinMode(RELAY_PIN, OUTPUT);

  setRelay(LOW);

  // ==========================================
  // SENSOR INIT
  // ==========================================

  dht.begin();

  Wire.begin(SDA_PIN, SCL_PIN);

  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  // ==========================================
  // PZEM INIT
  // ==========================================

  delay(2000);

  Serial.println("PZEM READY");

  // ==========================================
  // WIFI
  // ==========================================

  WiFi.begin(ssid, pass);

  // ==========================================
  // BLYNK
  // ==========================================

  Blynk.begin(auth, ssid, pass);

  // ==========================================
  // TIMER
  // ==========================================

  timer.setInterval(3000L, sendSensorData);

  timer.setInterval(10000L, checkConnection);

  Serial.println();
  Serial.println("=================================");
  Serial.println("GREEN CLASSROOM READY");
  Serial.println("=================================");
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  Blynk.run();

  timer.run();
}