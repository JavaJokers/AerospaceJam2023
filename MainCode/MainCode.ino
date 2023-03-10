#include <SoftwareSerial.h>
#include <Servo.h>

// Bluetooth
#define BT_RX_PIN 3
#define BT_TX_PIN 2
#define BT_echo_cmd "e"

// IR Stuff
#define IR_SEND_PIN 9
#define DECODE_SAMSUNG
#define DISABLE_CODE_FOR_RECEIVER
#define NO_LED_FEEDBACK_CODE
#define ir_open_cmd 0x60
#define ir_close_cmd 0x61
#define ir_offset 0x707
#define ir_repeats 5
#include <IRremote.hpp>

// Ultrasonic Sensor
#define ultrasonic_trig_pin 5
#define ultrasonic_echo_pin 4
#define ultrasonic_pulse_length 10 // microseconds

// Claw Servo
#define claw_servo_pin 10
#define claw_servo_initial_position 130
#define claw_servo_open 110
#define claw_servo_closed 175

// Pump
#define pump_pin 11
#define pump_timer_length 6000 // milliseconds

// Commands (note BT_echo_cmd above)
// Box commands
#define open_box_cmd "b"
#define close_box_cmd "p"
// Ultrasonic sensor commands
#define ultrasonic_sensor_cmd "u"
// Claw commands
#define claw_open_cmd "v"
#define claw_close_cmd "c"
#define reset_claw_cmd "r"
// Pump commands
#define pump_on_cmd "a"
#define pump_off_cmd "s"
#define pump_timer_cmd "t"

// Initialize objects
SoftwareSerial Bluetooth(BT_RX_PIN, BT_TX_PIN); // SoftwareSerial objects are initialized with RX, TX instead of the usual TX, RX
Servo claw_servo;

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
  pinMode(pump_pin, OUTPUT);

  // Attach claw servo
  claw_servo.write(claw_servo_initial_position); // HACK: Set position before attaching to prevent it from moving to 0
  claw_servo.attach(claw_servo_pin);

  Serial.println(" ---------- JavaJokers Aerospace Jam 2023 ---------- ");
  Serial.println("Initialized everything, waiting for connections...");
  Serial.println(F("Running " __FILE__ " from " __DATE__));
  Serial.println(" ---------------------------------------------------");
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

    if (data == BT_echo_cmd)
    { // Echo status
      sendMsg("Bluetooth is connected properly.");
    }
    else if (data == open_box_cmd)
    { // Open box
      IrSender.sendSamsung(ir_offset, ir_open_cmd, ir_repeats);
      sendMsg("Opened box");
    }
    else if (data == close_box_cmd)
    { // Close box
      IrSender.sendSamsung(ir_offset, ir_close_cmd, ir_repeats);
      sendMsg("Closed box");
    }
    else if (data == ultrasonic_sensor_cmd)
    { // Ultrasonic sensor - measure distance
      digitalWrite(ultrasonic_trig_pin, HIGH);
      delayMicroseconds(ultrasonic_pulse_length); // Ensure pulse length is 10 microseconds
      digitalWrite(ultrasonic_trig_pin, LOW);
      float duration_us, distance_cm, distance_in;
      duration_us = pulseIn(ultrasonic_echo_pin, HIGH); // Duration of echo pulse in µs
      distance_cm = 0.017 * duration_us;
      distance_in = 0.39 * distance_cm;
      sendMsg(String(distance_cm) + " centimeters, " + String(distance_in) + " inches");
    }
    else if (data == claw_open_cmd)
    {
      claw_servo.write(claw_servo_open);
      sendMsg("Opened claw");
    }
    else if (data == claw_close_cmd)
    {
      claw_servo.write(claw_servo_closed);
      sendMsg("Closed claw");
    }
    else if (data == reset_claw_cmd)
    {
      claw_servo.write(claw_servo_initial_position);
      sendMsg("Reset claw position");
    }
    else if (data == pump_on_cmd)
    {
      digitalWrite(pump_pin, HIGH);
      sendMsg("Pump on");
    }
    else if (data == pump_off_cmd)
    {
      digitalWrite(pump_pin, LOW);
      sendMsg("Pump off");
    }
    else if (data == pump_timer_cmd)
    {
      digitalWrite(pump_pin, HIGH);
      sendMsg("Pump on for " + String(pump_timer_length/1000) + " seconds");
      delay(pump_timer_length);
      digitalWrite(pump_pin, LOW);
      sendMsg("Pump off");
    }
    else
    {
      sendMsg("Err: Unsupported or malformed command");
    }
  }
}

void sendMsg(String msg)
{
  Bluetooth.print(msg);
  Serial.print("[" + String(millis()) + "] ");
  Serial.println(msg);
}