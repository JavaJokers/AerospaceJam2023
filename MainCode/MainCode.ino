#include <SoftwareSerial.h>

// Bluetooth SoftwareSerial
SoftwareSerial Bluetooth(3, 2);

// IR Stuff
#define IR_SEND_PIN 9
#define DECODE_SAMSUNG
#define DISABLE_CODE_FOR_RECEIVER
#define NO_LED_FEEDBACK_CODE
#define ir_open_command 0x60
#define ir_close_command 0x61
#define ir_offset 0x707
#define ir_repeats 5
#include <IRremote.hpp>

// Ultrasonic Sensor
#define ultrasonic_trig_pin 5
#define ultrasonic_echo_pin 4


void setup()
{
	// Begin Serial and SoftwareSerial ports
	Serial.begin(9600);
	Bluetooth.begin(9600); // Since we initialize it last, it'll be the SoftwareSerial port that's listened to by default

  // Begin IR
  IrSender.begin(DISABLE_LED_FEEDBACK); // We don't need multiple things fighting over control of the internal LED (pin 13)

  // Setup pins
  pinMode(ultrasonic_trig_pin, OUTPUT);
  pinMode(ultrasonic_echo_pin, INPUT);

	Serial.println("Initialized everything, waiting for connections...");
  Serial.println(F("Running " __FILE__ " from " __DATE__ ));
  Serial.print("Sending IR signals on pin ");
  Serial.println(IR_SEND_PIN);
}

void loop()
{
  Bluetooth.listen();
	// While receiving bluetooth messages
	if (Bluetooth.available())
	{
		Serial.print("Incoming Bluetooth command: ");
		char charData = Bluetooth.read();
		String data = String(charData);
		Serial.println(data);

    if (data == "e") { // Echo status
      Bluetooth.print("Bluetooth is connected properly.");
    } else if (data == "b") { // Open box
      IrSender.sendSamsung(ir_offset, ir_open_command, ir_repeats);
      Bluetooth.print("Opened box");
    } else if (data == "p") {
      IrSender.sendSamsung(ir_offset, ir_close_command, ir_repeats);
      Bluetooth.print("Closed box");
    } else if (data == "u") {
      digitalWrite(ultrasonic_trig_pin, HIGH);
      delayMicroseconds(10); // Ensure pulse length is 10 microseconds
      digitalWrite(ultrasonic_trig_pin, LOW);
      float duration_us, distance_cm, distance_in;
      duration_us = pulseIn(ultrasonic_echo_pin, HIGH); // Duration of echo pulse in µs
      distance_cm = 0.017 * duration_us;
      distance_in = 0.39 * distance_cm;
      Bluetooth.print(String(distance_cm) + " centimeteres, " + String(distance_in) + " inches");
    }
    else {
      Bluetooth.print("Unsupported command");
    }
	}
}
