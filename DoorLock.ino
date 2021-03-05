//#include <Keyboard.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Servo.h>
//#include <ArduinoJson.h>

#define HOSTIFTTT "maker.ifttt.com"
#define EVENTO "DoorLock"
#define IFTTTKEY "n_mXDY1OpwkDlt-0FNCNRfjHlig2gC4ctmQ1-n-MQcJ"
WiFiClient client;

const char* ssid = "IOT9";
const char* password = "12345678";


#include <Keypad.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

char keys[ROWS][COLS] = {
  {'1', '4', '7', '*'},
  {'2', '5', '8', '0'},
  {'3', '6', '9', '#'},
  {'A', 'B', 'C', 'D'}

};


// For ESP8266 Microcontroller
byte rowPins[ROWS] = {D1, D2, D3, D4};

byte colPins[COLS] = {D5, D6, D7, D8};
// For ESP32 Microcontroller
//byte rowPins[ROWS] = {23, 22, 3, 21};
//byte colPins[COLS] = {19, 18, 5, 17};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int len_key = 5;
char master_key[len_key] = {'1', '2', '3', '4', '1'};
char attempt_key[len_key];
int z = 0;
int incorrect = 0;
bool check = true;


//for servo
Servo servo_1;



void setup() {
  Serial.begin(115200);


//servo attach
servo_1.attach(16);//d0

  
  // Connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Start the server
  //  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  Serial.print("Enter Code: ");
  Serial.println("");

  //Send Webook to IFTTT
  //  send_webhook(IFTTTEVENT,IFTTTKEY)
}
void loop() {
  char key = keypad.getKey();
  if (key) {
    switch (key) {
      case 'A':
        z = 0;
        break;
      case '#':
        delay(100); // added debounce
        checkKEY();
        break;
      default:
        attempt_key[z] = key;
        z++;
    }
  }
}

void checkKEY()
{
  int correct = 0;
  int i;
  for (i = 0; i < len_key; i++) {
    if (attempt_key[i] == master_key[i]) {
      correct++;
    }
  }
  if (correct == len_key && z == len_key) {
    Serial.println("Correct Key");
    incorrect = 0;
   // delay(3000);
    z = 0;

//for motor
    servo_1.write (0);// servo will move to 90 degree angle.
    delay(10000);

    servo_1.write (180);// Servo will move to 45 degree angle.
    delay(10000);

    
  }
  else if (incorrect == 1) {
    //    Serial.println("Wrong password");
    if (client.connected())
    {
      client.stop();
    }

    client.flush();

    if (client.connect(HOSTIFTTT, 80)) {
      // build the HTTP request
      String toSend = "GET /trigger/";
      toSend += EVENTO;
      toSend += "/with/key/";
      toSend += IFTTTKEY;
      toSend += "?value1=";
      toSend += "Softwarica";
      toSend += " HTTP/1.1\r\n";
      toSend += "Host: ";
      toSend += HOSTIFTTT;
      toSend += "\r\n";
      toSend += "Connection: close\r\n\r\n";
      client.print(toSend);
      delay(5000);
    }

  }
  else
  {
    Serial.println("Incorrect Key");
    delay(1000);
    z = 0;
    incorrect++;

    Serial.println("Insert Password");
  }
  for (int zz = 0; zz < len_key; zz++) {
    attempt_key[zz] = 0;
  }

}
