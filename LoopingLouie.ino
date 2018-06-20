#include <AFMotor.h>
#define MAXSPEED 100
#define MINSPEED 40

#define SPEEDCHANGE 3000

AF_DCMotor motorL(1);

unsigned long targetTimeMode = 0;
unsigned long nextSpeedChange = 0;
unsigned int targetSpeed = MINSPEED;

unsigned int rndfor = 0;

typedef struct{
    int minspeed;
    int maxspeed;
    int mintime;
    int maxtime;
    bool forward;
    bool randomspeed;
} mode;

//minspeed, maxspeed, mintime, maxtime, forward/backward (1/-1), randomspeed 1/0
const mode modeForward = {MINSPEED, MAXSPEED, 2000, 30000, true, true};
const mode modeBackward = {MINSPEED, MAXSPEED, 500, 1000, false, true};
const mode modeDrop = {0, 0, 1000, 1000, true, false};
const mode modeEscalate = {MAXSPEED, MAXSPEED, 5000, 5000, true, false};

mode modes[] = {modeForward, modeBackward, modeDrop, modeEscalate};
mode *currentMode = NULL;

void nextMode() {
  int index = random(0, sizeof(modes) / sizeof(mode));
  currentMode = &modes[index];
  targetTimeMode = millis() + random(currentMode->mintime, currentMode->maxtime);
  targetSpeed = random(currentMode->minspeed, currentMode->maxspeed);

  Serial.print("Mode changed: ");
  Serial.print(index);
  Serial.print("\n");
}

uint8_t speed = 0;
uint8_t generateSpeed() {
  if (currentMode->randomspeed) {
      if(millis() > nextSpeedChange){
        targetSpeed = random(currentMode->minspeed, currentMode->maxspeed);        
      }
  } else {
    nextSpeedChange = millis() + SPEEDCHANGE;
  }

  if(speed < targetSpeed) {
     speed = (speed+2);
  } else {
     speed = (speed-2);
  }

  speed = min(speed, MAXSPEED);
  
  return speed;
}

void setup() {
  // put your setup code here, to run once:
  motorL.run(RELEASE);
  targetTimeMode = millis();
  nextMode();
  
  Serial.begin(9600);
}

void loop() {
  if(millis() > targetTimeMode){
    nextMode();
  }
  motorL.setSpeed(generateSpeed());
  
  if(currentMode->forward) {
    motorL.run(FORWARD);
  } else {
    motorL.run(BACKWARD);
  }
  
  delay(100);
}

