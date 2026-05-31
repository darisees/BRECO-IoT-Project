#define BLYNK_TEMPLATE_ID "TMPL6vjXFS8in"
#define BLYNK_TEMPLATE_NAME "TEKRA 2026"
#define BLYNK_AUTH_TOKEN "-QGSw3E8HqxuITsVrYx6oGR-4KgNybB6"
//#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

// =====================================================
// LIBRARY
// =====================================================

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <PubSubClient.h>

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
// MQTT CONFIGURATION
// =====================================================

const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// =====================================================
// PIN CONFIGURATION
// =====================================================

#define PIR_PIN        13
#define DHT_PIN        14

#define RELAY_PIN      26
#define AC_PIN         27

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

HardwareSerial PZEMSerial(2);
PZEM004Tv30 pzem(PZEMSerial, PZEM_RX, PZEM_TX);

BlynkTimer timer;

// =====================================================
// ROOM STATUS ENUM
// =====================================================

enum RoomState {
  ROOM_OFF,
  ROOM_ACTIVE,
  ROOM_WAITING,
  ROOM_ANOMALY
};

RoomState currentState = ROOM_OFF;

// =====================================================
// SYSTEM VARIABLES
// =====================================================

bool lampStatus = false;
bool acStatus   = false;

bool anomalyDetected = false;

unsigned long classStartTime = 0;
unsigned long lastMotionTime = 0;
unsigned long lastSerialLog  = 0;

// =====================================================
// SIMULATION TIMER
// =====================================================

// Simulasi dipercepat untuk Wokwi
// Nilai asli worksheet:
// classDuration = 100 menit
// emptyDelay    = 5 menit

const unsigned long classDuration = 30000UL;
const unsigned long emptyDelay    = 10000UL;

// =====================================================
// FUNCTION : ROOM STATUS STRING
// =====================================================

String getRoomStatus() {

  switch(currentState) {

    case ROOM_OFF:
      return "OFF_VACANT";

    case ROOM_ACTIVE:
      return "ROOM_ACTIVE";

    case ROOM_WAITING:
      return "WAITING_EMPTY";

    case ROOM_ANOMALY:
      return "ANOMALY_ALERT";
  }

  return "UNKNOWN";
}

// =====================================================
// FUNCTION : DEVICE CONTROL
// =====================================================

void turnOnDevices() {

  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(AC_PIN, HIGH);

  lampStatus = true;
  acStatus   = true;

  Serial.println("Lampu ON");
  Serial.println("AC ON");
}

void turnOffDevices() {

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(AC_PIN, LOW);

  lampStatus = false;
  acStatus   = false;

  Serial.println("Lampu OFF");
  Serial.println("AC OFF");
}

// =====================================================
// FUNCTION : MQTT RECONNECT
// =====================================================

void reconnectMQTT() {

  while (!mqttClient.connected()) {

    Serial.print("Connecting MQTT...");

    if (mqttClient.connect("BRECO-ESP32")) {

      Serial.println("connected");

    } else {

      Serial.print("failed, rc=");
      Serial.println(mqttClient.state());

      delay(2000);
    }
  }
}

// =====================================================
// FUNCTION : ROOM CONTROL LOGIC
// =====================================================

void roomControlLogic(int pirStatus) {

  switch(currentState) {

    // ==========================================
    // ROOM OFF
    // ==========================================

    case ROOM_OFF:

      if (pirStatus == HIGH) {

        currentState = ROOM_ACTIVE;

        classStartTime = millis();
        lastMotionTime = millis();

        turnOnDevices();

        Serial.println();
        Serial.println("=================================");
        Serial.println("STATUS : ROOM ACTIVE");
        Serial.println("Kelas dimulai");
        Serial.println("=================================");
      }

      break;

    // ==========================================
    // ROOM ACTIVE
    // ==========================================

    case ROOM_ACTIVE:

      if (pirStatus == HIGH) {
        lastMotionTime = millis();
      }

      // Kelas selesai
      if ((millis() - classStartTime) > classDuration) {

        currentState = ROOM_WAITING;

        Serial.println();
        Serial.println("=================================");
        Serial.println("STATUS : WAITING EMPTY");
        Serial.println("Menunggu ruangan kosong");
        Serial.println("=================================");
      }

      break;

    // ==========================================
    // WAITING EMPTY
    // ==========================================

    case ROOM_WAITING:

      if (pirStatus == HIGH) {

        lastMotionTime = millis();

        currentState = ROOM_ACTIVE;

        Serial.println("Aktivitas terdeteksi lagi");
      }

      if ((millis() - lastMotionTime) > emptyDelay) {

        turnOffDevices();

        currentState = ROOM_OFF;

        Serial.println();
        Serial.println("=================================");
        Serial.println("STATUS : OFF / VACANT");
        Serial.println("Semua perangkat dimatikan");
        Serial.println("=================================");
      }

      break;

    // ==========================================
    // ANOMALY
    // ==========================================

    case ROOM_ANOMALY:
      break;
  }
}

// =====================================================
// FUNCTION : POWER SIMULATION
// =====================================================

float simulatedPower() {

  if (lampStatus && acStatus) {
    return 3400.0;
  }

  if (lampStatus && !acStatus) {
    return 400.0;
  }

  if (!lampStatus && acStatus) {
    return 3000.0;
  }

  return 0.0;
}

// =====================================================
// FUNCTION : ANOMALY DETECTION
// =====================================================

void checkAnomaly(float powerValue) {

  if (!lampStatus && !acStatus && powerValue > 1000) {

    anomalyDetected = true;

    currentState = ROOM_ANOMALY;

    Serial.println();
    Serial.println("=================================");
    Serial.println("ALERT : ENERGY ANOMALY DETECTED");
    Serial.println("Daya masih tinggi setelah shutdown");
    Serial.println("=================================");
  }

  else {

    anomalyDetected = false;
  }
}

// =====================================================
// FUNCTION : SEND SENSOR DATA
// =====================================================

void sendSensorData() {

  // ==========================================
  // SENSOR READING
  // ==========================================

  float temperature = dht.readTemperature();
  float humidity    = dht.readHumidity();

  if (isnan(temperature)) temperature = 0;
  if (isnan(humidity)) humidity = 0;

  int pirStatus = digitalRead(PIR_PIN);

  float lux = lightMeter.readLightLevel();

  if (isnan(lux) || lux < 0) {
    lux = 0;
  }

  // ==========================================
  // ROOM LOGIC
  // ==========================================

  roomControlLogic(pirStatus);

  // ==========================================
  // PZEM DATA
  // ==========================================

  float voltage   = 220.0;
  float current   = 15.45;
  float power     = simulatedPower();
  float energy    = power / 1000.0;
  float frequency = 50.0;
  float pf        = 0.98;

  // ==========================================
  // ANOMALY CHECK
  // ==========================================

  checkAnomaly(power);

  // ==========================================
  // JSON PAYLOAD
  // ==========================================

  String payload = "{";

  payload += "\"room\":\"FILKOM-F2-01\",";
  payload += "\"status\":\"" + getRoomStatus() + "\",";
  payload += "\"occupancy\":" + String(pirStatus) + ",";
  payload += "\"temperature\":" + String(temperature,1) + ",";
  payload += "\"humidity\":" + String(humidity,1) + ",";
  payload += "\"lux\":" + String(lux,1) + ",";
  payload += "\"voltage\":" + String(voltage,1) + ",";
  payload += "\"current\":" + String(current,2) + ",";
  payload += "\"power\":" + String(power,1) + ",";
  payload += "\"energy\":" + String(energy,2) + ",";
  payload += "\"frequency\":" + String(frequency,1) + ",";
  payload += "\"pf\":" + String(pf,2) + ",";
  payload += "\"lamp\":" + String(lampStatus) + ",";
  payload += "\"ac\":" + String(acStatus) + ",";
  payload += "\"alert\":\"" + String(anomalyDetected ? "ANOMALY" : "NORMAL") + "\"";

  payload += "}";

  // ==========================================
  // SERIAL MONITOR
  // ==========================================

  if (millis() - lastSerialLog > 3000) {

    lastSerialLog = millis();

    Serial.println();
    Serial.println("========== BRECO TELEMETRY ==========");

    Serial.println(payload);

    Serial.println("=====================================");

    Serial.println("[MQTT] Publish telemetry");
    Serial.println("[Node-RED] Processing flow");
    Serial.println("[InfluxDB] Historical logging");
    Serial.println("[Blynk] Dashboard update");
  }

  // ==========================================
  // MQTT PUBLISH
  // ==========================================

  mqttClient.publish(
    "breco/filkom/f2/classroom01/telemetry",
    payload.c_str()
  );

  // ==========================================
  // BLYNK UPDATE
  // ==========================================

  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
  Blynk.virtualWrite(V3, lux);
  Blynk.virtualWrite(V4, power);
  Blynk.virtualWrite(V5, energy);
  Blynk.virtualWrite(V6, getRoomStatus());
  Blynk.virtualWrite(V7, lampStatus);
  Blynk.virtualWrite(V8, acStatus);
  Blynk.virtualWrite(V9, anomalyDetected);
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
  pinMode(AC_PIN, OUTPUT);

  turnOffDevices();

  // ==========================================
  // SENSOR INIT
  // ==========================================

  dht.begin();

  Wire.begin(SDA_PIN, SCL_PIN);

  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  // ==========================================
  // WIFI
  // ==========================================

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

  // ==========================================
  // MQTT
  // ==========================================

  mqttClient.setServer(mqtt_server, 1883);

  reconnectMQTT();

  // ==========================================
  // BLYNK
  // ==========================================

  Blynk.begin(auth, ssid, pass);

  // ==========================================
  // TIMER
  // ==========================================

  timer.setInterval(3000L, sendSensorData);

  // ==========================================
  // READY MESSAGE
  // ==========================================

  Serial.println();
  Serial.println("=================================");
  Serial.println("BRECO SYSTEM READY");
  Serial.println("Smart Green Classroom Simulation");
  Serial.println("=================================");
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  if (!mqttClient.connected()) {
    reconnectMQTT();
  }

  mqttClient.loop();

  Blynk.run();

  timer.run();
}
