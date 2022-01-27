#include <SoftwareSerial.h>
#include <Adafruit_AS7341.h>
#include <Servo.h>

// Variables for bluetooth
SoftwareSerial Bluetooth(3,2);

// Configuration and variables for LIDAR
SoftwareSerial Lidar(4, 5); //define software serial port name as Serial1 and define pin2 as RX and pin3 as TX
int lidarDist; //actual distance measurements of LiDAR
int lidarStrength; //signal strength of LiDAR
float lidarTemprature;
int check; //save check value
int i;
int uart[9]; //save data measured by LiDAR
const int HEADER = 0x59; //frame header of data package
String lidarQueryResponse;
String LidarQueryResponse;
String LightQueryResponse;

Adafruit_AS7341 as7341;

Servo servo;
int pos = 0; 

void setup() {
  
  // Begin Serial and SoftwareSerial ports
  Serial.begin(9600);
  Lidar.begin(115200);
  Bluetooth.begin(9600);
  
  servo.attach(9);
  // Listen on Bluetooth port
  Bluetooth.listen();
  // Begin Light sensor
  if (!as7341.begin()){
    Serial.println("Could not find AS7341");
    while (1) { delay(10); }
  }
  // Light sensor config
  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_256X);
  as7341.enableLED(false);
  Serial.println("Initialized all sensors, waiting for connections...");
}

void loop() {
  // Whilst reciving bluetooth messages
  if(Bluetooth.available()>0)
  {
    Serial.println("made it");
    char data = Bluetooth.read();
    String sData = String(data);
    Serial.println(sData);
    // If query is for lidar (letter 'A'), return lidar stats
    if (sData == "a")
    {
      fetchLidar();
      
      LidarQueryResponse = "Distance: " + String(lidarDist) + ":" + String(lidarStrength);
      Bluetooth.print(LidarQueryResponse);
    }
    // Elif query is for light wavelength sensor (letter 'B'), return light stats
    else if(sData == "b")
    {
//      Serial.println("Else if B");
       fetchLight();


      
       LightQueryResponse = String(as7341.getChannel(AS7341_CHANNEL_630nm_F7)) + "," + String(as7341.getChannel(AS7341_CHANNEL_515nm_F4)) + "," +String(as7341.getChannel(AS7341_CHANNEL_415nm_F1));
       Bluetooth.print("#00FF00");
//       Bluetooth.print(LightQueryResponse);
    }
    else if(sData == "c")
    {
      Serial.println("Servo");
      Bluetooth.print("ok");
      for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
    servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
        
    }
  }
  
}

void fetchLidar(){
  Lidar.listen();
  delay(50);
  Serial.println("Fetching lidar stats");
  if (Lidar.available()) { //check if serial port has data input
    Serial.println("Lidar available");
    if (Lidar.read() == HEADER) { //assess data package frame header 0x59
      uart[0] = HEADER;
      if (Lidar.read() == HEADER) { //assess data package frame header 0x59
        uart[1] = HEADER;
        for (i = 2; i < 9; i++) { //save data in array
          uart[i] = Lidar.read();
        }
        check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
        //if (uart[8] == (check & 0xff)) { //verify the received data as per protocol
          Serial.println(String(uart[2]));
          Serial.println(String(uart[3]));
          lidarDist = uart[2] + uart[3] * 256; //calculate distance value
          lidarStrength = uart[4] + uart[5] * 256; //calculate signal strength value
          lidarTemprature = uart[6] + uart[7] * 256; //calculate chip temprature
          lidarTemprature = lidarTemprature / 8 - 256;
        //}
      }
    }
    Serial.println("Distance: "+String(lidarDist));
    Bluetooth.listen();
  }  
}


void fetchLight(){
  if (!as7341.readAllChannels()){
      Serial.println("ERROR reading channels in light sensor!");
      return;
  }
  Serial.print("F1 415nm : ");
  Serial.println(as7341.getChannel(AS7341_CHANNEL_415nm_F1));
  Serial.print("F2 445nm : ");
  Serial.println(as7341.getChannel(AS7341_CHANNEL_445nm_F2));
  Serial.print("F3 480nm : ");
  Serial.println(as7341.getChannel(AS7341_CHANNEL_480nm_F3));
  Serial.print("F4 515nm : ");
  Serial.println(as7341.getChannel(AS7341_CHANNEL_515nm_F4));
  Serial.print("F5 555nm : ");
  Serial.println(as7341.getChannel(AS7341_CHANNEL_555nm_F5));
  Serial.print("F6 590nm : ");
  Serial.println(as7341.getChannel(AS7341_CHANNEL_590nm_F6));
  Serial.print("F7 630nm : ");
  Serial.println(as7341.getChannel(AS7341_CHANNEL_630nm_F7));
  Serial.print("F8 680nm : ");
  Serial.println(as7341.getChannel(AS7341_CHANNEL_680nm_F8));

  Serial.print("Clear    : ");
  Serial.println(as7341.getChannel(AS7341_CHANNEL_CLEAR));

  Serial.print("Near IR  : ");
  Serial.println(as7341.getChannel(AS7341_CHANNEL_NIR));
  return;
}