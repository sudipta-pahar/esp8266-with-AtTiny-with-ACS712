/*  since we were not told about the method to take input in the esp8266, by default I took as android app
 *  Submitted by Sudipta Pahar on 31.03.2020 @ 3:00 pm
 *  Comments are welcome
 */
#include<SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <Filters.h>                      //This library does a massive work check it's .cpp file

WiFiClient client;
WiFiServer server(80);
const char* ssid = "ssid"; //WiFi name
const char* password = "password"; //WiFi Password
String command = "";
int R1 = 0;// AtTiny85 pins 0,1,2
int R2 = 1;
int ACS_Pin = 2;// 2 relays
int state;

float ACS_Value;                              //Here we keep the raw data valuess
float testFrequency = 50;                    // test signal frequency (Hz) Current frequency in India
float windowLength = 40.0/testFrequency;     // how long to average the signal, for statistist



float intercept = 0; // to be adjusted based on calibration testing
float slope = 0.0752; // to be adjusted based on calibration testing
                      //Please check the ACS712 Tutorial video by SurtrTech to see how to get them because it depends on your sensor, or look below


float Amps_TRMS; // estimated actual current in amps

unsigned long printPeriod = 1000; 
unsigned long previousMillis = 0;
void setup(void)
{ 

  server.begin();
  Serial.begin(115200);// Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
     Serial.print("*");
  }
  
  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP());// Print the IP address
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(ACS_Pin, INPUT);// we will check the TRMS value of current passing through R1
}

void loop() 
{
  RunningStatistics inputStats;                 
  inputStats.setWindowSecs( windowLength ); 
  if(Serial.available()>0)
  {
    state = Serial.read();
  }
  client = server.available();
  if(!client) return;
  command = checkClient ();
  Serial.println(command);
  if(command == "R1 on")
  {
    digitalWrite(R1, HIGH);

    while( true ) //start the ACS712 function
    {   
    ACS_Value = analogRead(ACS_Pin);  
    inputStats.input(ACS_Value);  // log to Stats function
        
    if((unsigned long)(millis() - previousMillis) >= printPeriod) 
    { //every second we do the calculation
      previousMillis = millis();   // update time
      
      Amps_TRMS = intercept + slope * inputStats.sigma();

      Serial.print( "\t Amps: " ); 
      Serial.print( Amps_TRMS );// printing the TRMS to Serial Monitor, can update to any other display module in future

    }
  }
  }
  else if(command == "R1 off")
  {
    digitalWrite(R1, LOW);
  }
  else if(command == "R2 on")
  {
    digitalWrite(R1, HIGH);
  }
  else if(command == "R2 off")
  {
    digitalWrite(R2, LOW);
  }
}
String checkClient (void)
{
  while(!client.available()) delay(1);
    String request = client.readStringUntil('\r');
    request.remove(0, 5);
    request.remove(request.length() - 9,9);
    return request;  
}

void sendBackEcho(String echo)
{
  /* Since it is a dummy code, I am not writing this part
   *  if required would add afterwards!
   */
}
