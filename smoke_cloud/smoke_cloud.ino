#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//#include <SoftwareSerial.h>

//SoftwareSerial ESPserial(3, 1); // RX, TX

const char* ssid =  "NETGEAR47";     // change according to your Network - cannot be longer than 32 characters!
const char* pass =  "caiser@1234"; // change according to your Network
const char* host = "192.168.0.112";

// Update these with values suitable for your network.
IPAddress ip(192,168,1,24);  //Node static IP
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
int g;
int ledpin = D2;
int ledpin1 = D1;
String gu="ppm";
String n;
String data;
int gasAnalogPin = A0;//MQ7 sensor
float m = -0.49; //Slope 
float b = 2.88; //Y-Intercept 
 float sensor_volt;
 float RS_air;
 float R0; 
 double ppm;
 float sensorValue = 0;
//float R0 = 0.15; //Sensor Resistance in fresh air from previous code
ESP8266WebServer server(7999);
void setup() {
  Serial.begin(9600);
pinMode(D1, OUTPUT);
pinMode(D2, OUTPUT);
  pinMode(gasAnalogPin, INPUT);
WiFi.begin(ssid, pass);
  WiFi.config(ip, gateway, subnet);
  
  while ((WiFi.status() != WL_CONNECTED)){
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected"));
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); 
}


void POST()
  { 
   Serial.print("connecting to ");
    Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 8001;
  if (!client.connect(host, httpPort))
    {
     Serial.println("connection Failed");
     return;
    }
    Serial.print("Requesting POST: ");
   
   data = "smoke=" + String(ppm);
 
client.println("POST /smoke/rest/smoke HTTP/1.1"); //API CALL
     client.println("Host:192.168.0.112"); // SERVER ADDRESS HERE TOO
     client.println("Content-Type: application/x-www-form-urlencoded"); 
     client.print("Content-Length: "); 
     client.print(data.length());
     client.print("\n\n");
     client.print(data);
     Serial.println(data);
     
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 7000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println();
  Serial.println("closing connection"); 
  }

void co(){



  // A) preparation
    // turn the heater fully on
    analogWrite(gasAnalogPin, 1023.0); 
    // heat for 1 min
//    Serial.println("preheating for 60sec");
    delay(60000);
    

    // now reducing the heating power: turn the heater to approx 1.4V
    analogWrite(gasAnalogPin, 71.4); // 1023x1400/5000
//     Serial.println("Threshold Value 90sec");// heat for 90 sec
    delay(90000);
    

  // B) reading    
    // CO via MQ7
    analogWrite(gasAnalogPin, HIGH); 
    delay(50); // Getting an analog read apparently takes 100uSec

  for(int i = 0; i <= 100; i++){
    sensorValue = sensorValue + analogRead(gasAnalogPin);
     
   }
     
 sensorValue = sensorValue/100.0; //get the avarage value
 sensor_volt = sensorValue*5.0/1023.0;
 RS_air = ((5.0)-sensor_volt)/sensor_volt;
 R0 = RS_air/(25.132); // Not sure how they came up with this ?
//   Serial.print("sensorValue1:");
//     Serial.println(sensorValue);
//    Serial.print("sensor_volt:");
//     Serial.println(sensor_volt);
//    Serial.print("RS_air:");
//     Serial.println(RS_air);
//     Serial.print("R0:");
//     Serial.println(R0);
//Serial.println("--From LoadResistance to ResistanceSensor---");
 float RS_gas = 0;
 float ratio = 0;

  sensor_volt = 0;
  sensorValue = 0;
  sensor_volt = 0;
//  sensorValue= sensorValue/50.0;
  sensorValue = analogRead(gasAnalogPin);
  sensor_volt = sensorValue*(5.0/1023.0);
  RS_gas = ((5.0*10)-sensor_volt)/sensor_volt-10.0;
  ratio = RS_gas/R0; //Replace R0 with the value found using the sketch above
double ppm_log = (log10(ratio)-b)/m; //Get ppm value in linear scale according to the the ratio value  
   ppm = pow(10, ppm_log); //Convert ppm value to log scale 
  double percentage = ppm/10000; //Convert to percentage 
//     Serial.print("sensorValue:");
//     Serial.println(sensorValue);
//     Serial.print("sensor_volt:");
//     Serial.println(sensor_volt);
//     Serial.print("RS_gas:");
//     Serial.println(RS_gas);
//     Serial.print("RS:");
//     Serial.println(ratio);
//     Serial.println("::::::::Ratio of sensor to CO ppm concentration::::::"); 
//     Serial.print("PPMlog:"); 
//    Serial.println(ppm_log);
//    Serial.print("PPM:"); 
//    Serial.println(ppm);
//    Serial.print("percentage:"); 
//    Serial.print(percentage);
//    Serial.println("%");
Serial.println(":::::::preheating sensor for 60sec::::::::::::");
//    if(ppm>=50){ //Check if ppm value is greater than 2000 
//    digitalWrite(D2, LOW); //Turn LED on 
//    digitalWrite(D1, HIGH); //Turn buzzer on 
//  }
//  else{ //Case ppm is not greater than 2000
//    digitalWrite(D1, LOW); //Turn LED off
//    digitalWrite(D2, HIGH); //Turn buzzer off
//  } 
}
//void gas(){
//  float sensorVoltage; 
//  float sensorValue;
//  sensorValue = adc.readADC(0);
//  sensorVoltage = sensorValue*3.3/1023.0;
//  g = 26.572*exp(1.2894*(sensorValue*3.3/1023.0)); //Multiply raw analog value by 3.3/1023.0 to convert to a voltage
////  Serial.print("RawValue = ");
////  Serial.print(sensorValue);Serial.println("Analog");
////  Serial.print("Sensor Voltage = ");
////  Serial.print(sensorVoltage);Serial.println("V");
////  Serial.print("MQ9 Sensor Value = ");
////  Serial.print(g);
//// Serial.println("ppm");Serial.println("");
//if (g>=500){
//  n="Hazardous";
//  digitalWrite(ledpin, HIGH);  digitalWrite(ledpin1, LOW);
//}else{
//  n="Normal";
//  digitalWrite(ledpin1, HIGH);
//    digitalWrite(ledpin, LOW);
//}
//}
void loop() {
//gas();
co();
   POST();

}
