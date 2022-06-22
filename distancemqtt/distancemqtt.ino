const int trigPin = 12;
const int echoPin = 14;


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define TOKEN "N2wom6NUmXJurzPRtrCL"     //  Enter your Write API key from ThingSpeak
 
const char *ssid =  "meiexien";     // replace with your wifi ssid and wpa2 key
const char *password =  "Guangdong1982";
//const char* server = "api.thingspeak.com";
char ThingsboardHost[] = "iot.eie.ucr.ac.cr";



//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;


//WiFiClient client;
WiFiClient wifiClient;
PubSubClient client(wifiClient);
int status = WL_IDLE_STATUS;


void setup() {
    Serial.begin(115200); // Starts the serial communication
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input


 
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(200);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("connected to");
    Serial.println(ssid);
    client.setServer( ThingsboardHost, 1883 );
}

void loop() 
{
    if ( !client.connected() ) 
      reconnect();



  
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

    unsigned long timeBegin = micros();
  
    // Prepare a JSON payload string
    String payload = "{";
    payload += "\"distanceCm\":";payload += distanceCm; 
    payload += "}";

    char attributes[1000];
    payload.toCharArray( attributes, 1000 );
    client.publish( "v1/devices/me/telemetry",attributes);
    Serial.println( attributes );
   
    // stop timer.
    unsigned long timeEnd = micros();
    unsigned long duration = timeEnd - timeBegin;
    double averageDuration = (double)duration / 1000.0;
    Serial.print("data meassured: ");
    Serial.println(distanceCm);
    Serial.print("Time taken to send packet: "); 
    Serial.print(averageDuration);    
    Serial.println(" s");
    Serial.print("Bit rate: ");
    Serial.print(sizeof(payload)*8/averageDuration);
    Serial.println(" bps");
    //Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
    Serial.println(" ");
    
    Serial.println("Waiting...");
    delay(1000);

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("Esp8266", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.println( " : retrying in 5 seconds]" );
      delay( 500 );
    }
  }
}
