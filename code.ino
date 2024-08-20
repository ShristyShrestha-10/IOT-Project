#include <LiquidCrystal_I2C.h>
#include <DHT_U.h>

#define DHTPIN 32
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2); 

//vairables of ultrasonic sensor
#define trigger_pin  26
#define echo_pin  27

// variables for buzzer
#define buzzer_pin 13

// variables for moisture sensor
int m,sensor_analog;
const int sensor_pin = 35;  /* Soil moisture sensor O/P pin */


#ifdef __cplusplus
extern "C" {
#endif
  uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();
#include <WiFi.h>

String apiKey = "J6R4M5R6VKITS2ZC";  //  Enter your Write API key from ThingSpeak
const char *ssid = "Islington-College";         // replace with your wifi ssid and wpa2 key
const char *pass = "I$LiNGT0N2023";
const char *server = "api.thingspeak.com";
WiFiClient client;

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));

  dht.begin();

  //setting up the lcd display
  lcd.init();
  lcd.backlight();

  //setting up the ultrasonic sensor
  pinMode (trigger_pin, OUTPUT); 
  pinMode (echo_pin, INPUT);

  // setting up the buzzer
  pinMode (buzzer_pin, OUTPUT);

  //things speak code
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
}

void loop() {
  // Wait a few seconds between measurements.
  delay(1000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  int h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // red moisture sensor 
  sensor_analog = analogRead(sensor_pin);
  m = ( 100 - ( (sensor_analog/4095.00) * 100 ) );

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }


  //printing in serial monitor
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print("  Moisture: ");
  Serial.print(m);
  Serial.println("%");


  //printing in lcd display
  lcd.print("Hd:");
  lcd.print(h);
  lcd.print("%");
  lcd.print(" Tp:");
  lcd.print(t);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print(" Moisture: ");
  lcd.print(m);
  lcd.print("%");
  lcd.setCursor(0, 0);

  //ultrasonic sensor code
  digitalWrite (trigger_pin, HIGH);
  delayMicroseconds (10);
  digitalWrite (trigger_pin, LOW);
  int time = pulseIn (echo_pin, HIGH);
  int distance = (time * 0.034) / 2;
    
  // buzzer code
  if (distance <= 25) 
        {
        digitalWrite (buzzer_pin, HIGH);   // make this HIGH------------------------------------    
        }

  else {  
        digitalWrite (buzzer_pin, LOW);             
  }  

  
  if (client.connect(server, 80))             //   "184.106.153.149" or api.thingspeak.com
  {

    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(h);
    postStr += "&field2=";
    postStr += String(t);
    postStr += "&field3=";
    postStr += String(m);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.println("%. Send to Thingspeak.");
    // client.stop(); -------------------------------------------this has been edited 
    Serial.println("Waiting...");
  } 
}