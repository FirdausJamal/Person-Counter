#ifndef UNIT_TEST
#include "ThingSpeak.h"
#include "secrets.h"
#include <Arduino.h>
#endif
#include <ESP8266WiFi.h>
#include <Servo.h>
WiFiServer server(301);//the port
Servo myservo; // create servo object to control a servo
IPAddress ip(192, 168, 43, 67);
IPAddress gateway(192, 168, 43, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 43, 1);

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

void setup(void) {
  Serial.begin(115200);
  delay(10);
  // attaches the servo on D2 or GPIO4 of the NodeMCU devkit v1.0
  myservo.attach(D2);
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //Static IP Setup Info Here...
  WiFi.config(ip, dns, gateway, subnet);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Start the server
  server.begin();
  Serial.println("Server started");
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}
void loop()
{
   if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempti; ng to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  
  int pos;
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    // delay(1);
    client.setNoDelay(1);
  }
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  // Match the request for the angle movement of the servo motor
  if (request.indexOf("/sweep left") != -1)
  {
    // goes from 0 degrees to 180 degrees
    for (pos = 0; pos <= 180; pos += 1)
    { // in steps of 1 degree
      // tell servo to go to position in variable 'pos'
      myservo.write(pos);
      // waits 15ms for the servo to reach the position
      delay(15);
    }
  }
  if (request.indexOf("/sweep right") != -1)
  {
    // goes from 0 degrees to 180 degrees
    for (pos = 0; pos <= 180; pos += 1)
      // in steps of 1 degree
    {
      // tell servo to go to position in variable 'pos'
      myservo.write(pos);
      // waits 15ms for the servo to reach the position 
      delay(15);
    }
  }
   int x = ThingSpeak.writeField(myChannelNumber, 1, pos, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); // do not forget this one
  client.println("");
  client.println("<html>");
  client.println("<title>Control the gate</title>");
  client.println("<center><H1>WEB CONTROLLED SERVO MOTOR");
  client.println("<H2>USING NODEMC U 1.0 BOARD");
  client.print("<body style = 'background-color:lightgreen;'>");
  client.print("<br>");
  client.print("<br>");
  client.println("<a href=\"/sweep left\"\"><button style='background-color:blue;width:200px;height:60px'><h2>SWEEP LEFT</h2></button></a><p>");
  client.println("<a href=\"/sweep right\"\"><button style='background-color:red;width:200px;height:60px'><h2>SWEEP RIGHT</h2></button></a><br/></center>");
  client.println("</body>");
  client.println("</html>");
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}
