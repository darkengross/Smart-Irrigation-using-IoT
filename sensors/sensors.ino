#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiClient.h>
#include <DHT.h>
#include <ThingSpeak.h>

//pins used
//A0 for soil
//D4 for dht
//D1 for buzzer

#define on 1
#define off 2
#define hot 3
#define cold 4
#define isHumid 5
#define isNotHumid 6
//Blynk Details
#define BLYNK_AUTH_TOKEN "mKZc8vmjzqLRypf21cUN0sV1DeuYluEu"
#define WIFI_SSID "Gryffindor's Tower"
#define WIFI_PASSWORD "qazwsx1234"

const IPAddress receiver_ip(192, 168, 219, 9);
WiFiClient client;

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


int buzzerPin = 5;
float temperature;
float humidity;

int soilPin = A0;
int soilValue = 0;

unsigned long Channel_ID = 2139609;            // Channel ID
const char *WriteAPIKey = "OOF33YSR4CNF3HDV";  // Your write API Key
long delayStart = millis();

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);

  WiFi.disconnect();
  delay(2000);
  Serial.print("Start Connection");  //Starting the fresh Wifi Connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  // Connect to WiFi network
  ThingSpeak.begin(client);
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);  //Starting the Blynk App Connection
  dht.begin();                                              //initializing the DHT sensor for Readings
}

void loop() {
  Blynk.run();
  soilValue = analogRead(soilPin);
  Serial.print("Soil Moisture: ");

  //Read soil value from the sensor
  Serial.println(soilValue);

  //Read temprature and humidity via DHT sensor
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (isnan(temperature)) {
    Serial.println("Failed to read temperature from DHT sensor!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C");
  }

  if (isnan(humidity)) {
    Serial.println("Failed to read humidity from DHT sensor!");
  } else {
    Serial.print(" | Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }



  if ((millis() - delayStart) >= 15000) {
    ThingSpeak.writeField(Channel_ID, 1, soilValue, WriteAPIKey);
    // Determine status of our soil using the values already defined for wet and dry soil
    // if (moisture < 600) {
    //   Serial.println("Status: Soil is too wet");
    //   ThingSpeak.writeField(Channel_ID, 4, false, WriteAPIKey);
    // } else if (moisture <= soilWet && moisture > soilDry) {
    //   Serial.println("Status: Soil moisture is perfect");
    //   digitalWrite(motorPin, HIGH);
    //   ThingSpeak.writeField(Channel_ID, 4, false, WriteAPIKey);
    // } else {
    //   Serial.println("Status: Soil is too dry - time to water!");
    //   digitalWrite(motorPin, LOW);
    //   ThingSpeak.writeField(Channel_ID, 4, true, WriteAPIKey);
    // }
  }
  if ((millis() - delayStart) >= 15000) {
    if (!isnan(temperature))
      ThingSpeak.writeField(Channel_ID, 2, temperature, WriteAPIKey);
  }
  if ((millis() - delayStart) >= 15000) {
    if (!isnan(humidity))
      ThingSpeak.writeField(Channel_ID, 3, humidity, WriteAPIKey);
  }

  Blynk.virtualWrite(V2, soilValue);
  Blynk.virtualWrite(V3, temperature);
  Blynk.virtualWrite(V4, humidity);


  if (temperature > 35) {  //This means that the atmospheric temprature is above the threashhold so we will do actuation like covering with shead
    if (client.connect(receiver_ip, 80)) {
      client.print(hot);  // Send sensor value to receiver NodeMCU
      // delay(100);
      // client.stop();
    }
  } else {
    if (client.connect(receiver_ip, 80)) {
      client.print(cold);  // Send sensor value to receiver NodeMCU
      // delay(100);
      // client.stop();
    }
  }


  //If the moisture in soil is below the threshold so we will send signal to the other node to start pump
  if (soilValue > 600) {
    if (client.connect(receiver_ip, 80)) {
      client.print(on);  // Send sensor value to receiver NodeMCU
      // delay(100);
      // client.stop();
    }
  } else {
    if (client.connect(receiver_ip, 80)) {
      client.print(off);  // Send sensor value to receiver NodeMCU
      // delay(100);
      // client.stop();
    }
  }


  if (humidity > 80) {
    if (client.connect(receiver_ip, 80)) {
      client.print(isHumid);  // Send sensor value to receiver NodeMCU
      // client.stop();
    }
  } else {
    if (client.connect(receiver_ip, 80)) {
      client.print(isNotHumid);  // Send sensor value to receiver NodeMCU
      // delay(100);
      // client.stop();
    }
  }
  client.stop();
  // delay(1000);
}

BLYNK_WRITE(V5) {
  int temp = param.asInt();  // Read the value of temperature from the Blynk app
  if (temp >= 35) {
    Serial.println("Temprature is too hot. Cover the crops");
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(buzzerPin, LOW);
  }

  if (client.connect(receiver_ip, 80)) {
    client.print(hot);  // Send sensor value to receiver NodeMCU
    // delay(100);
    // client.stop();
  } else {
    if (client.connect(receiver_ip, 80)) {
      client.print(cold);  // Send sensor value to receiver NodeMCU
        // delay(100);
        // client.stop();
    }
  }
}
