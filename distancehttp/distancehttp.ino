//pin connected to distance sensor
const int trigPin = 12;
const int echoPin = 14;


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "meiexien";
const char* password = "Guangdong1982";

// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";
// Service API Key
String apiKey = "CJ9OX6AZ01C2BY4Y";



//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
int sequenceNumber;
float distanceCm;
float distanceInch;



// THE DEFAULT TIMER IS SET TO 1 SECONDS FOR TESTING PURPOSES
unsigned long lastTime = 0;
// Set timer to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Timer set to 10 seconds (10000)
unsigned long timerDelay = 10000;

void setup() {
  //serial Baud rate set up
  Serial.begin(115200);
  sequenceNumber=0;
  //distance sensor set up
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  //wifi configuration
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() 
{

  // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    
    // Calculate the distance
    distanceCm = duration * SOUND_VELOCITY/2;
    
    // Convert to inches
   // distanceInch = distanceCm * CM_TO_INCH;
   
   
  
/*******************************************************/  
  //Send an HTTP POST request every 10 seconds
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED)
    {
      WiFiClient client;
      HTTPClient http;
      unsigned long timeBegin = micros();
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      sequenceNumber++;
      String httpRequestData = "api_key=" + apiKey + "&field1=" + distanceCm + "&field2=" + sequenceNumber;           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      /*
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      // JSON data to send with HTTP POST
      String httpRequestData = "{\"api_key\":\"" + apiKey + "\",\"field1\":\"" + String(random(40)) + "\"}";           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);*/
       http.end();
      // stop timer.
      unsigned long timeEnd = micros();
      unsigned long duration = timeEnd - timeBegin;
      double averageDuration = (double)duration / 1000.0;
      double bitrate = (sizeof(httpRequestData)*8)/averageDuration;
      Serial.print("data meassured: ");
      Serial.println(distanceCm);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Time taken to send packet: "); 
      Serial.print(averageDuration);    
      Serial.println(" s");
      Serial.print("Bit rate: ");
      Serial.print(bitrate);
      Serial.println(" bps");
      Serial.println(" ");
    }
    else 
      Serial.println("WiFi Disconnected");
    lastTime = millis();
  }
}
