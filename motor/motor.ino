#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <WiFiClient.h>
Servo myservo;
int pos = 0;

//Blynk details
char auth[] = "mKZc8vmjzqLRypf21cUN0sV1DeuYluEu";
char ssid[] = "Gryffindor's Tower";
char pass[] = "qazwsx1234";

//Object for Creating a Wifi - Server
WiFiServer server(80);

int incomingData;
int ledPin = 2;                                                                            // the pin that the LED is attached to
bool ledStatus = false;                                                                    // variable to store the status of the LED
int commands[6] = { 1,2,3,4,5,6 };  // Array of possible commands
int commandCount = 6;                                                                      // Number of elements in the commands array


int tempPin = 5;
int flag = 1;
int humidFlag = 1;


void setup() {
  Serial.begin(9600);  //For Serial Connection to serial Monitor
  pinMode(ledPin, OUTPUT);
  pinMode(tempPin, OUTPUT);

  Serial.print("Connecting to ");  //Connecting to Wifi
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");  //Showing IP Address of Connection
  Serial.println(WiFi.localIP());

  server.begin();  //Starting the Server

  Blynk.begin(auth, ssid, pass);  //Starting the Blynk App Connection
  // Blynk.syncVirtual(V3, V4);            //Syncing Virtual Pins
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Blynk.run();                  //Running the Blynk App Client
    while (client.connected()) {  //Checking for client Connection
      if (client.available()) {
        incomingData = client.read() - '0';
        Serial.println(incomingData);
        for (int i = 0; i < commandCount; i++) {  // Loop through the possible commands
          if (incomingData == commands[i]) {      // If the incoming data matches the current command
            executeCommand(i);                    // Call the function to execute the command
            break;                                // Exit the loop
          }
        }  //Reading the Data Sent by Client
        incomingData = -1;
      }
    }
  }
 
  // while (Serial.available() > 0) {              // Check if there is any data available on the serial port
  //   char incomingChar = Serial.read();          // Read the incoming data
  //   incomingData += incomingChar;               // Append the incoming character to the incomingData string
  //   if (incomingChar == '\n') {                 // If a newline character is received
  //     for (int i = 0; i < commandCount; i++) {  // Loop through the possible commands
  //       if (incomingData == commands[i]) {      // If the incoming data matches the current command
  //         executeCommand(i);                    // Call the function to execute the command
  //         break;                                // Exit the loop
  //       }
  //     }
  //     incomingData = "";  // Reset the incomingData string
  //   }
  // }
  client.stop();
}

void executeCommand(int commandIndex) {
  switch (commandIndex) {
    case 0:  // If the command is "on"
      if (flag) {
        if (humidFlag) {
          digitalWrite(ledPin, HIGH);
          myservo.attach(D2);
        }
      }
      // Turn on the LED
      break;
    case 1:  // If the command is "off"
      // digitalWrite(ledPin, LOW);    // Turn off the LED
      digitalWrite(ledPin, LOW);
      myservo.detach();
      break;
    case 2:  // If the command is "hot"
      Serial.println("Atmospheric Temprature is very hot cover the crops with shed");
      break;
    case 3:  // if command is "cold"
      Serial.println("Atmospheric Temprature is appropriate rest assured!!");
      break;
    case 4:  // if command is "isHumid"
      humidFlag = 0;
      myservo.detach();
            digitalWrite(ledPin, LOW);
      break;
    case 5:  // if command is "isNotHumid"
      humidFlag = 1;
      break;
  }
}

BLYNK_WRITE(V1)  // this function gets called whenever the button widget is turned on or off
{
  int buttonState = param.asInt();  // get the state of the button

  if (buttonState == 1)  // if the button is turned on
  {
    digitalWrite(ledPin, HIGH);  // turn on the LED
    ledStatus = true;            // update the LED status variable
    myservo.attach(D2);
    flag = 1;
  } else  // if the button is turned off
  {
    digitalWrite(ledPin, LOW);  // turn off the LED
    ledStatus = false;          // update the LED status variable
    myservo.detach();
    flag = 0;
  }
}


BLYNK_WRITE(V5) {
  int temp = param.asInt();  // Read the value of temperature from the Blynk app
  if (temp >= 35) {
    Serial.println("Temprature is too hot. Cover the crops");
    digitalWrite(tempPin, HIGH);
  } else {
    digitalWrite(tempPin, LOW);
  }
}
