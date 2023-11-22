#include <AccelStepper.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <String.h>


// Define stepper motor connections and limit switch connections
#define motor1_step 13
#define motor1_dir 12
#define motor2_step 14
#define motor2_dir 27
#define motor3_step 26
#define motor3_dir 25
#define motor6_step 33
#define motor6_dir 32
#define motor5_step 19
#define motor5_dir 21
#define motor4_step 22
#define motor4_dir 23

//Limit Switches 
#define motor1_limit 15
#define motor2_limit 2
#define motor3_limit 4
#define motor4_limit 16
#define motor5_limit 17
#define motor6_limit 5

#define MAX_SPEED 900 
// #define ACCELERATION 50
// #define MOVE_TO 10000

#define SPEED 900

const char* ssid = "wifi_a";
const char* password = "aaaaaaaa";

bool limit1 = false;
bool limit2 = false;
bool limit3 = false;
bool limit4 = false;
bool limit5 = false;
bool limit6 = false;


int prevValue1 = 1;
int prevValue2 = 1;
int prevValue3 = 1;
int prevValue4 = 1;
int prevValue5 = 1;
int prevValue6 = 1;

int prevValues[6] = {0,0,0,0,0,0};
bool limits[6] = {false, false, false, false, false};
int limitState[6] = {0,0,0,0,0,0};
int limitPins[6] = {motor1_limit, motor2_limit, motor3_limit, motor4_limit, motor5_limit, motor6_limit};


bool go[6] = {0,0,0,0,0,0};

bool first1 = true; 
bool first2 = true;
bool first3 = true;
bool first4 = true;
bool first5 = true;
bool first6 = true;

AccelStepper stepper1(AccelStepper::DRIVER, motor1_step, motor1_dir);
AccelStepper stepper2(AccelStepper::DRIVER, motor2_step, motor2_dir);
AccelStepper stepper3(AccelStepper::DRIVER, motor3_step, motor3_dir);
AccelStepper stepper4(AccelStepper::DRIVER, motor4_step, motor4_dir);
AccelStepper stepper5(AccelStepper::DRIVER, motor5_step, motor5_dir);
AccelStepper stepper6(AccelStepper::DRIVER, motor6_step, motor6_dir);

AccelStepper steppers[] = {stepper1, stepper2, stepper3, stepper4, stepper5, stepper6};

AsyncWebServer server(80);

String receivedCommand;
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
  }
}
void moveSteppers(byte command, int value) {
  
  getStepperFromBits(command);
  getStepperFromBits(command);
  getStepperFromBits(command);
  getStepperFromBits(command);
  getStepperFromBits(command);
  getStepperFromBits(command);

  bool checkEnd = true;
  while(checkEnd){
    checkEnd = false;

    if(go[0] == 1)
      checkEnd = checkEnd || (abs(stepper1.currentPosition()) <= abs(value));
    if(go[1] == 1)
      checkEnd = checkEnd || (abs(stepper2.currentPosition()) <= abs(value));
    if(go[2] == 1)
      checkEnd = checkEnd || (abs(stepper3.currentPosition()) <= abs(value));
    if(go[3] == 1)
      checkEnd = checkEnd || (abs(stepper4.currentPosition()) <= abs(value));
    if(go[4] == 1)
      checkEnd = checkEnd || (abs(stepper5.currentPosition()) <= abs(value));
    if(go[5] == 1)
      checkEnd = checkEnd || (abs(stepper6.currentPosition()) <= abs(value));
    
    // Serial.print("Check End: ");
    // Serial.println(checkEnd);

    
    
    if((prevValue1 / abs(prevValue1)) != (value / abs(value))){
      limit1 = true;
    }
    else if((prevValue1 / abs(prevValue1)) == (value / abs(value)) && digitalRead(motor1_limit) != 1) {
      limit1 = false;
    }
    if(go[0] == 1){
      if(first1){
        prevValue1 = value;
        limit1 = false;
        first1 = false;
      }
      if((digitalRead(motor1_limit) != 1 || limit1)){
        stepper1.setSpeed(SPEED * (value/abs(value)));
        stepper1.runSpeed();   
        prevValue1 = value;         
      }
      else {
        stepper1.setCurrentPosition(value);
        Serial.println("Motor 1 Limit Triggered!");
      }      
    }

    if((prevValue2 / abs(prevValue2)) != (value / abs(value))){
      limit2 = true;
    }
    else if((prevValue2 / abs(prevValue2)) == (value / abs(value)) && digitalRead(motor2_limit) != 1) {
      limit2 = false;
    }
    if(go[1] == 1){
      if(first2){
        prevValue2 = value;
        limit2 = false;
        first2 = false;
      }
      if((digitalRead(motor2_limit) != 1 || limit2)){
        stepper2.setSpeed(SPEED * (value/abs(value)));
        stepper2.runSpeed();   
        prevValue2 = value; 
      }
      else{
        stepper2.setCurrentPosition(value);
        Serial.println("Motor 2 Limit Triggered!");
      }
      
    }
    
    if((prevValue3 / abs(prevValue3)) != (value / abs(value))){
      limit3 = true;
    }
    else if((prevValue3 / abs(prevValue3)) == (value / abs(value)) && digitalRead(motor3_limit) != 1) {
      limit3 = false;
    }
    if(go[2] == 1){
      if(first3){
        prevValue3 = value;
        limit3 = false;
        first3 = false;
      }
      if((digitalRead(motor3_limit) != 1 || limit3)){
        stepper3.setSpeed(SPEED * (value/abs(value)));
        stepper3.runSpeed();   
        prevValue3 = value;   
      }
      else {
        stepper3.setCurrentPosition(value);
        Serial.println("Motor 3 Limit Triggered!");
      }
    }

    if((prevValue4 / abs(prevValue4)) != (value / abs(value))){
      limit4 = true;
    }
    else if((prevValue4 / abs(prevValue4)) == (value / abs(value)) && digitalRead(motor4_limit) != 1) {
      limit4 = false;
    }
    if(go[3] == 1){
      if(first4){
        prevValue4 = value;
        limit4 = false;
        first4 = false;
      }
      if((digitalRead(motor4_limit) != 1 || limit4)){
        stepper4.setSpeed(SPEED * (value/abs(value)));
        stepper4.runSpeed();   
        prevValue4 = value; 
      }
      else{
        stepper4.setCurrentPosition(value);
        Serial.println("Motor 4 Limit Triggered!");
      }      
    }

    if((prevValue5 / abs(prevValue5)) != (value / abs(value))){
      limit5 = true;
    }
    else if((prevValue5 / abs(prevValue5)) == (value / abs(value)) && digitalRead(motor5_limit) != 1) {
      limit5 = false;
    }
    if(go[4] == 1){
      if(first5){
        prevValue5 = value;
        limit5 = false;
        first5 = false;
      }
      if((digitalRead(motor5_limit) != 1 || limit5)){
        stepper5.setSpeed(SPEED * (value/abs(value)));
        stepper5.runSpeed();   
        prevValue5 = value; 
      }
      else{
        stepper5.setCurrentPosition(value);
        Serial.println("Motor 5 Limit Triggered!");
      }      
    }

    if((prevValue6 / abs(prevValue6)) != (value / abs(value))){
      limit6 = true;
    }
    else if((prevValue6 / abs(prevValue6)) == (value / abs(value)) && digitalRead(motor6_limit) != 1) {
      limit6 = false;
    }
    if(go[5] == 1){
      if(first6){
        prevValue6 = value;
        limit6 = false;
        first6 = false;
      }
      if((digitalRead(motor6_limit) != 1 || limit6)){
        stepper6.setSpeed(SPEED * (value/abs(value)));
        stepper6.runSpeed();   
        prevValue6 = value;         
      }
      else{
        stepper6.setCurrentPosition(value);
        Serial.println("Motor 6 Limit Triggered!");
      }
      
    }
    if(((go[0] && (digitalRead(motor1_limit) == 1 && !limit1)) || go[0] == 0) && 
    ((go[1] && (digitalRead(motor2_limit) == 1 && !limit2)) || go[1] == 0) &&
    ((go[2] && (digitalRead(motor3_limit) == 1 && !limit3)) || go[2] == 0) &&
    ((go[3] && (digitalRead(motor4_limit) == 1 && !limit4)) || go[3] == 0) &&
    ((go[4] && (digitalRead(motor5_limit) == 1 && !limit5)) || go[4] == 0) &&
    ((go[5] && (digitalRead(motor6_limit) == 1 && !limit6)) || go[5] == 0)){
      break;
    }
  }
  limit1 = false;
  limit2 = false;
  limit3 = false;
  limit4 = false;
  limit5 = false;
  limit6 = false;
}
// void moveSteppers(byte command, int value) {
//   bool checkEnd = true;
//   int success = 0;
//   AccelStepper* stepper; 
//   while(checkEnd){
//     checkEnd= false;
//     stepper = getStepperFromBits(command, success);
//     checkEnd = checkEnd || (abs(stepper->currentPosition()) <= abs(value));
//     Serial.print("success x : ");
//     Serial.println(success);
//     Serial.print("command x : ");
//     Serial.println(command);
//     if(success == 1){
//       stepper->setSpeed(SPEED * (value/abs(value)));
//       stepper->runSpeed();
//       success = 0;
//     }

//     stepper = getStepperFromBits(command, success);
//     checkEnd = checkEnd || (abs(stepper->currentPosition()) <= abs(value));
//     if(success == 1){
//       stepper->setSpeed(SPEED * (value/abs(value)));
//       stepper->runSpeed();
//       success = 0;
//     }

//     stepper = getStepperFromBits(command, success);
//     checkEnd = checkEnd || (abs(stepper->currentPosition()) <= abs(value));
//     if(success == 1){
//       stepper->setSpeed(SPEED * (value/abs(value)));
//       stepper->runSpeed();
//       success = 0;
//     }

//     stepper = getStepperFromBits(command, success);
//     checkEnd = checkEnd || (abs(stepper->currentPosition()) <= abs(value));
//     if(success == 1){
//       stepper->setSpeed(SPEED * (value/abs(value)));
//       stepper->runSpeed();
//       success = 0;
//     }

//     stepper = getStepperFromBits(command, success);
//     checkEnd = checkEnd || (abs(stepper->currentPosition()) <= abs(value));
//     if(success == 1){
//       stepper->setSpeed(SPEED * (value/abs(value)));
//       stepper->runSpeed();
//       success = 0;
//     }

//     stepper = getStepperFromBits(command, success);
//     checkEnd = checkEnd || (abs(stepper->currentPosition()) <= abs(value));
//     if(success == 1){
//       stepper->setSpeed(SPEED * (value/abs(value)));
//       stepper->runSpeed();
//       success = 0;
//     }
    
//   }
  // bool checkEnd = true;
  // while(checkEnd){
  //   checkEnd = false;
  //   // for(int i = 0; i < sizeof(steppersFiltered); i++){
  //   for(int index = 0;  index < sizeof(indexes)/sizeof(indexes[0]); index++){
  //     if(indexes[index] != -1){
  //       checkEnd = checkEnd || (abs(steppers[index].currentPosition()) <= abs(value));
  //       if(abs(steppers[index].currentPosition()) >= abs(value)){
  //         steppers[index].setSpeed(0);
  //         steppers[index].stop();
  //         prevValues[index] = value;
  //       }
  //       else{
  //         if(!limits[index]){
  //           if(value/(abs(value)) != prevValues[index]/abs(prevValues[index])){
  //             limits[index] = false;
  //           }            
  //         }
  //         limitState[index] = digitalRead(limitPins[index]);
  //         if(limitState[index] != 1 || !limits[index]){
  //           steppers[index].setSpeed(SPEED * (value/abs(value)));
  //           steppers[index].runSpeed();
  //         }
  //       }
  //     }
  //   }
  // }
// }

// AccelStepper* getStepperFromBits(byte &bits, int &success){
//   if(bits & 0b00000001){
//     success = 1;
//     bits = 0b00111110 & bits;
//     return &stepper1;
//   }    
//   else if (bits &  0b00000010){
//     success = 1;
//     bits = 0b00111101 & bits;
//     return &stepper2;
//   }
//   else if (bits &  0b00000100){
//     success = 1;
//     bits = 0b00111011 & bits;
//     return &stepper3;
//   }
//   else if (bits &  0b00001000){
//     success = 1;
//     bits = 0b00110111 & bits;
//     return &stepper4;
//   }
//   else if (bits &  0b00010000){
//     success = 1;
//     bits = 0b00101111 & bits;
//     return &stepper5;
//   }
//   else if (bits &  0b00100000){
//     success = 1;
//     bits = 0b00011111 & bits;
//     return &stepper6;    
//   }
//   else{
//     success = 0;
//     return &stepper1;
//   }
//   return &stepper1;
// }
void getStepperFromBits(byte &bits){
  if(bits & 0b00000001){
    
    bits = 0b00111110 & bits;
    // return &stepper1;
    go[0] = 1;
  }    
  else if (bits &  0b00000010){
    
    bits = 0b00111101 & bits;
    go[1] = 1;
  }
  else if (bits &  0b00000100){
    
    bits = 0b00111011 & bits;
    go[2] = 1;
  }
  else if (bits &  0b00001000){
    
    bits = 0b00110111 & bits;
    go[3] = 1;
  }
  else if (bits &  0b00010000){
    
    bits = 0b00101111 & bits;
    go[4] = 1;
  }
  else if (bits &  0b00100000){
    
    bits = 0b00011111 & bits;
    go[5] = 1;
  }
}
AccelStepper* getStepper(int index) {
  switch (index) {
    case 0: return &stepper1;
    case 1: return &stepper2;
    case 2: return &stepper3;
    case 3: return &stepper4;
    case 4: return &stepper5;
    case 5: return &stepper6;
    default: return nullptr;
  }
}

void setup() {
  initWiFi();
  
  server.on("/command", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("cmd")) {
        
        receivedCommand = request->getParam("cmd")->value();
        Serial.println("Command received: " + receivedCommand);
        request->send(200, "text/plain", "Command received");
    } else {
        request->send(400, "text/plain", "No command received");
    }
  });

  // Start server
  server.begin();
  
  pinMode(motor1_limit, INPUT_PULLDOWN);
  pinMode(motor2_limit, INPUT_PULLDOWN);
  pinMode(motor3_limit, INPUT_PULLDOWN);
  pinMode(motor4_limit, INPUT_PULLDOWN);
  pinMode(motor5_limit, INPUT_PULLDOWN);
  pinMode(motor6_limit, INPUT_PULLDOWN);
  

  for (int i = 0; i < 6; i++) {
    AccelStepper* stepper = getStepper(i);
    stepper->setMaxSpeed(MAX_SPEED);
  } 

  Serial.begin(115200);

}  



void loop() {
  int pos = 0;
  int indexes[6] = {-1,-1,-1,-1,-1,-1};
  int value;
  byte command = 0b00000000;
  if(receivedCommand !=""){
    Serial.println("Proceessing command: " + receivedCommand);
    while((pos = receivedCommand.indexOf(' ')) >= 0){
      String axisSubStr = receivedCommand.substring(0, pos); 
      receivedCommand.remove(0, pos + 1);
      char axis = axisSubStr.charAt(0);
      value = axisSubStr.substring(1).toInt();
      
      // prevValue1 = value;
      // prevValue2 = value;
      // prevValue3 = value;
      // prevValue4 = value;
      // prevValue5 = value;   
      // prevValue6 = value;
      Serial.println("Axis: " + String(axis) + " Value : " + String(value));
      int limitState = 0;
      
      switch(axis) {
        int limitState; 
        case 'X':
          stepper1.setCurrentPosition(0);
          command |= 0b00000001;
          break;

        case 'Y':
          stepper2.setCurrentPosition(0);
          command |= 0b00000010;
          break;
        case 'Z':
          stepper3.setCurrentPosition(0);
          command |= 0b00000100;
            break;
        case 'A':
          stepper4.setCurrentPosition(0);
          command |= 0b00001000;
          break;
        case 'B':
          stepper5.setCurrentPosition(0);
          command |= 0b00010000;
          break;
        case 'C':
          stepper6.setCurrentPosition(0);
          command |= 0b00100000;
          break;
      }
    }
    if (receivedCommand.length() > 0) {
      char axis = receivedCommand.charAt(0);
      value = receivedCommand.substring(1).toInt();
      // Handle the command for this axis
      Serial.println("Axis: " + String(axis) + " Value: " + String(value));
      // prevValue1 = value;
      // prevValue2 = value;
      // prevValue3 = value;
      // prevValue4 = value;
      // prevValue5 = value;   
      // prevValue6 = value;
      switch(axis) {
        int limitState; 
        case 'X':
          stepper1.setCurrentPosition(0);
          command |= 0b00000001;
          break;

        case 'Y':
          stepper2.setCurrentPosition(0);
          command |= 0b00000010;
          break;
        case 'Z':
          stepper3.setCurrentPosition(0);
          command |= 0b00000100;
          break;
        case 'A':
          stepper4.setCurrentPosition(0);
          command |= 0b00001000;
          break;
        case 'B':
          stepper5.setCurrentPosition(0);
          command |= 0b00010000;
          break;
        case 'C':
          stepper6.setCurrentPosition(0);
          command |= 0b00100000;
          break;
      }
    }
    Serial.println(command);
    moveSteppers(command, value);    
    command = 0b00000000;
    // Reset the received command
    receivedCommand = "";
    for(int i = 0; i < sizeof(go)/sizeof(go[0]); i++){
      go[i] = 0;
    }
  }
  
  delay(10);
}



int getLimitPin(int index) {
  switch (index) {
    case 0: return motor1_limit;
    case 1: return motor2_limit;
    case 2: return motor3_limit;
    case 3: return motor4_limit;
    case 4: return motor5_limit;
    case 5: return motor6_limit;
    default: return -1;
  }
}