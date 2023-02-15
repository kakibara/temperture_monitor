#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

// WiFi connection
const char *ssid = "SSID";
const char *password = "PASSWORD";

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
// influxdb cloud
#define INFLUXDB_URL ""
#define INFLUXDB_TOKEN ""
#define INFLUXDB_ORG ""
#define INFLUXDB_BUCKET ""

// InfluxDB database name
// remove InfluxDbCloud2CACert if you use self hosted Influx DB
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
#define ROOM ""
Point sensor("temp_status");

// DHT20 config
#include <DHT20.h>
DHT20 dht = DHT20();
// I2C Address
#define DS1307_ADDRESS 0x68 // Realtime clock
// DH20
#define SLAVE_ADRS 0x38
#define STATUS_CHK 0x71
#define POLYNOMIAL 0x31
#define RESET_REG_ADDR 0xba

/*****************************************************************************
 *                          Predetermined Sequence                           *
 *****************************************************************************/
void setup()
{
    Serial.begin(115200);

    // Prepare I2C protocol.
    Wire.begin(21, 22); // Define(SDA, SCL)

    // Prepare WiFi system.
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\r\nWiFi connected!");

    // Add constant tags - only once
    // sensor.addTag("device", DEVICE);
    sensor.addTag("room", ROOM);
    // sensor.addTag("hu", WiFi.SSID());

    // Check server connection
    if (client.validateConnection())
    {
        Serial.print("Connected to InfluxDB: ");
        Serial.println(client.getServerUrl());
    }
    else
    {
        Serial.print("InfluxDB connection failed: ");
        Serial.println(client.getLastErrorMessage());
    }
    delay(3000);
}

void loop()
{
    float hu, tp;
    uint8_t buf[8];
    long a;
    int flg;
    calibrate_wifi();
    delay(20000);
    get_data(hu, tp);
    send_data(tp, hu);

    Serial.print("T=" + String(tp) + "'C ");
    Serial.println("H=" + String(hu) + "%");
}

/*****************************************************************************/
/* WiFi */
unsigned long previousMillis = 0;
unsigned long interval = 30000;
void calibrate_wifi()
{
    unsigned long currentMillis = millis();
    // if WiFi is down, try reconnecting
    if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval))
    {
        Serial.print(millis());
        Serial.println("Reconnecting to WiFi...");
        WiFi.disconnect();
        WiFi.reconnect();
        previousMillis = currentMillis;
    }
}

void send_data(const float temp, const float hu)
{
    sensor.clearFields();
    sensor.addField("temp", temp);
    sensor.addField("hu", hu);
    // sensor.addField()
    // Print what are we exactly writing
    Serial.print("Writing: ");
    Serial.println(client.pointToLineProtocol(sensor));
    // Write point
    if (!client.writePoint(sensor))
    {
        Serial.print("InfluxDB write failed: ");
        Serial.println(client.getLastErrorMessage());
    }
    else
        Serial.println("\r\nset message");
}

void get_data(float &hu, float &temp)
{
    if (millis() - dht.lastRead() >= 1000)
    {
        if (DHT20_OK == dht.read())
        {
            hu = dht.getHumidity();
            temp = dht.getTemperature();
            // Serial.print("Humidity: ");
            // Serial.print(hu, 1);
            // Serial.print("  Temerature: ");
            // Serial.println(temp, 1);
        }
    }
}
