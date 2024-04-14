// Load Wi-Fi library
#include <WiFi.h>
#include <string>

// Network credentials Here
const char* ssid     = "ESP32-Network";
const char* password = "Esp32-Password";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds
const long timeoutTime = 2000;

#define RXp2 16
#define TXp2 17

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);

  WiFi.softAP(ssid,password);
  
  // Print IP address and start web server
  Serial.println("");
  Serial.println("IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();
 
}

void loop(){

  string two_num = Serial2.readString();
  // separate two numbers received from Arduino
  string s;
  stringstream ss(two_num);
  vector<string> v;
  while (getline(ss, s, ' ')) {
        v.push_back(s);
  }

  string max_pp = v[0]
  string current_pp = v[1];

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    // Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // check if max capcaity changes or not
            if (header.indexOf("GET /1/inc") >= 0) {
              Serial.println("+")               // +1
            } else if (header.indexOf("GET /1/dec") >= 0) {
              Serial.println("-");               //-1
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html lang=\"en\">");
            client.println("<head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
            client.println("<title>Room Occupancy Monitor</title>");
            client.println("<style>body {font-family: Arial, sans-serif; background-color: #4ec33e; margin: 0;padding: 0;}");
            client.println("h1 {color: #5c260b;text-align: center;}.container {width: 80%;margin: 0 auto;padding: 20px;background-color: #fff;border-radius: 10px;box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);}");
            client.println("p {line-height: 1.6; color: #666;}");
            client.println(".gap{width: 100%;margin: 0 auto;padding: 20 px;background-color: #8cd6e3;border-radius: 10px;box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);}");
            client.println("button{width: 30%;margin: auto;padding: 20 px;background-color: #ed1111;display: block;border-radius: 20px;box-shadow: 0 0 10px rgba(94, 92, 92, 0.025);}</style> </head>");
            client.println("<body><h1> Room Occupancy Monitor </h1> <div class=\"container\"> <p>Current People in the Room: ");
            client.println(current_pp);
            client.println("</p></div>");
            client.println("<div class=\"gap\"> <p>.</p> </div><div class=\"container\"> <p>Maximum People in the Room: ");
            client.println(max_pp);
            client.println("</p></div>");
            client.println("<div class=\"gap\"><p> . </p></div><p>Changing Maximum Capacity</p><a href=\"/1/inc\"><button type=\"button\">+1</button></a><p></p><a href=\"/1/dec\"><button type=\"button\">-1</button></a></body>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    //Serial.println("Client disconnected.");
    //Serial.println("");
  }
}