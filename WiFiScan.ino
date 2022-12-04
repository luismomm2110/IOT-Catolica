// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPLcfE2j3be"
#define BLYNK_DEVICE_NAME "PetFeeder"
#define BLYNK_AUTH_TOKEN "TOKEN"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Stepper.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

const int stepsPerRevolution = 2048; // change this to fit the number of steps per revolution

// ULN2003 Motor Driver Pins
#define IN1 26
#define IN2 25
#define IN3 33
#define IN4 32

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "wifi";
char pass[] = "SENHA";

BlynkTimer timer;
WidgetRTC rtc;
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);
bool isStepperRunning = false;
int countOpens = 0;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  isStepperRunning = !isStepperRunning;
  Serial.println(countOpens);

  // Update state
  Blynk.virtualWrite(V1, countOpens);
}

BLYNK_WRITE(V1) // the virtual Pin that stores and displays the number of openings
{
  countOpens = param.asInt();
  Serial.print(countOpens);
}

// display in the serial and write on the Virtual Pin V3
void clockDisplay()
{
  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentDate = String(day()) + "/" + month() + "/" + year();
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.print(" ");
  Serial.print(currentDate);
  Serial.println();
  String formatedTime = currentTime + " " + currentDate;

  Blynk.virtualWrite(V3, formatedTime);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  rtc.begin();
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{

  Blynk.virtualWrite(V2, millis() / 1000);
}

void setup()
{
  myStepper.setSpeed(5);

  // Debug console
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  Blynk.syncVirtual(V1); // just read V1 from server to MCU

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  myStepper.step(0);
  setSyncInterval(10 * 60); // Sync interval in seconds (10 minutes)

  if (isStepperRunning)
  {
    Serial.println("running");
    // run motor
    myStepper.step(stepsPerRevolution);
    // increase openings
    countOpens++;
    delay(1000);
    // stop running
    isStepperRunning = false;
    // display time of last opening
    clockDisplay();
  }
}
