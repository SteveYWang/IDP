#include <Servo.h>
#include "Adafruit_TCS34725.h"
#include <SoftwareSerial.h>
#define TXPin 14
#define redpin 45
#define greenpin 46
#define bluepin 44

const int pingPin = 50;
float distThres = 9; 
int stopHash;
int code = 0;
int lefts =  47;
int mid = 51;
int rights = 52;
int hash = -1;
int greenLED = 5;
int yellowLED = 10;
int redLED = 2;
char letters[5] = {'A', 'B', 'C', 'D', 'E'};
char order[4] = {'1', '2', '3', '4'};
int hashColor[5][3] = {{0, 255, 255}, {150, 250, 255}, {255, 0, 255},
                      {255, 255, 0}, {150, 255, 150}};
                      
Servo servoLeft;
Servo servoRight;
SoftwareSerial LCD = SoftwareSerial(255, TXPin);

void setup() {
  Serial.begin(9600);  
  Serial2.begin(9600);
  LCD.begin(9600);

  servoRight.attach(12);
  servoLeft.attach(11);

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  LCD.write(12);
  delay(10);
  LCD.write(22);
  delay(10);
  LCD.write(17);
}

void loop() {
  int state = 4 * color(lefts) + 2 * color(mid) + color(rights);
  //Serial.println(state);

  switch(state){
    case 0:
        forward();
        break;

    case 3:
        rightLittle();
        break;
        
    case 1:
        right();
        break;
        
    case 4:
        left();
        break;
          
    case 6:
        leftLittle();
        break;

    case 2:
        forward();
        break;
    
    case 7:
        //Serial.println(hash);
        hash += 1;

        if (hash <= 4){
          stop();
          float distance = dist();
          //Serial.println(distance);
          set_RGB(hashColor[hash][0], hashColor[hash][1], hashColor[hash][2]);
          digitalWrite(redLED, HIGH);        
          if (distance < distThres){
            code += 1;
          }

          LCD.write(12);
          delay(10);
          LCD.print(code);
          delay(1000);
          set_RGB(255, 255, 255);
          digitalWrite(redLED, LOW);

          if (hash == 4){
            digitalWrite(greenLED, HIGH);
            //Serial.println("code: " + code);
            Serial2.write(char(code + 64));
            delay(1000);
            digitalWrite(greenLED, LOW);
            
            bool noRec = true;
            char recCode;
            char recData;
            while (noRec){
              if (Serial2.available()){
                //Serial.println("Here");
                recData = Serial2.read();
                pinMode(yellowLED, HIGH);
                delay(1000);
                pinMode(yellowLED, LOW);
                //Serial.println("Recieved a: " + recData);
              }

              if (isIn(recData, 5, letters)){
                //Serial.println("Here!!");
                recCode = recData;

                if (code == 1){
                  noRec = false;
                }
              }

              else if (isIn(recData, 4, order)){
                //Serial.println("recieved a percent");
                if (recData == code + '0' - 1){
                  //Serial.println("false now");
                  noRec = false;
                }              
              }

              //Serial.println("noRec: " + noRec);              
              delay(100);
            }

            stopHash = 12 - code;
            //Serial.println(stopHash);
          }
          forward();
          delay(300);
          break;
        }

        else if (hash == 6){
          stop();
          rotate();
          forward();
          delay(300);
        }

        else if (hash == stopHash){
          done();
        }

        else{
          forward();
          delay(300);
        }

        break;    

    default:
    stop();
    break;        
  }

  delay(20);
}

void done() {
  stop();
  Serial2.write(char(code + 48));
  Serial.println("Done!");
  play_song();
  while(true){}
}

long color(int pin)
{
  pinMode(pin, OUTPUT);    
  digitalWrite(pin, HIGH); 
  delay(1);                
  pinMode(pin, INPUT);     
  digitalWrite(pin, LOW);  
  long time = micros();    
  while(digitalRead(pin)); 
  time = micros() - time;
  //Serial.println(time);  
  return (time > 150);            
}

void forward(){
  servoLeft.writeMicroseconds(1600);
  servoRight.writeMicroseconds(1400);
  //Serial.print("f");
}

void stop(){
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);      
}

void leftLittle(){
  servoRight.writeMicroseconds(1400);
  servoLeft.writeMicroseconds(1500);
}

void left(){
  servoRight.writeMicroseconds(1300);
  servoLeft.writeMicroseconds(1500);
}

void rightLittle(){
  servoRight.writeMicroseconds(1500);
  servoLeft.writeMicroseconds(1650);
}

void right(){
  servoRight.writeMicroseconds(1500);
  servoLeft.writeMicroseconds(1700);
}

void set_RGB(int r, int g, int b){
  analogWrite(redpin, r);
  analogWrite(greenpin, g);
  analogWrite(bluepin, b);
}

float dist(){
  long duration;
  float inches, cm;

  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);

  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);

  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  // convert the time into a distance
  cm = duration / 29.155 / 2.0;

  return cm;
}

bool isIn(char let, int len, char arr[]){
  for (int i = 0; i < len; i++){
    if (*(arr + i) == let){
      return true;
    } 
  }
  return false;
}

void rotate(){
  servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1700);
  delay(500);
  stop();
}

// MUSIC STUFF - part: 5

int notes[] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 6, 8, 6, 1, 1, 8, 6, 8, 1, 1, 1, 6, 1, 8, 11, 8, 8, 8, 1, 1, 1, 1, 1, 1, 1, 8, 8, 8, 8, 8, 8, 8, 1, 1, 1, 8, 8, 8, 8, 8, 8, 1, 1, 1, 11, 10, 11, 8, 8, 8, 8, 8, 8, 1, 0, 0, 0, 1, 1, 0, 10, 10, 6, 6, 5, 3, 1, 0, 10, 8, 10, 0, 1, 0, 10, 8, 1, 8, 8, 8, 8, 6, 6, 8, 8, 10, 8, 8, 8, 8, 6, 1, 10, 8, 8, 10, 0, 6, 6, 8, 8, 10, 8, 8, 1, 11, 9, 8, 6, 4, 3, 1, 8, 8, 8, 8, 8, 8, 8, 8, 6, 8, 6, 8, 8
};
int octaves[] = {
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 4, 4, 4, 4, 4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 4, 4, 4, 4, 4, 4, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 4, 4, 4, 4, 4, 4, 3, 3, 3, 4, 4, 4, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4
};
double start_times[] = {
        0, 0.8333339999999998, 0.9027784999999998, 0.9722229999999998, 1.0416675, 1.111112, 1.9444460000000001, 2.0138905, 2.083335, 2.1527795, 2.2222239999999998, 2.4074093333333333, 2.592594666666667, 2.7777800000000004, 2.962965333333334, 3.1481506666666674, 3.333336000000001, 3.5185213333333345, 3.703706666666668, 3.8888920000000016, 4.166670000000002, 4.259262666666668, 4.351855333333335, 4.444448000000001, 4.629633333333334, 4.8148186666666675, 5.000004000000001, 6.111116, 6.296301333333333, 6.481486666666666, 6.998847924999999, 7.082181258333333, 7.165514591666667, 7.915514591666666, 7.998847925, 8.082181258333332, 8.165514591666664, 8.415514591666664, 8.915514591666664, 9.415514591666664, 9.665514591666664, 9.83218125833333, 9.915514591666662, 9.998847924999994, 10.16551459166666, 10.332181258333327, 10.498847924999993, 10.915514591666659, 11.415514591666659, 11.665514591666659, 11.832181258333325, 11.915514591666657, 11.998847924999989, 12.165514591666655, 12.332181258333321, 12.498847924999987, 12.665514591666653, 12.83218125833332, 12.998847924999986, 13.165514591666652, 13.915514591666652, 13.998847924999984, 14.082181258333316, 14.165514591666648, 14.415514591666648, 14.665514591666648, 15.165514591666648, 15.665514591666648, 16.040514591666646, 16.165514591666646, 16.332181258333314, 16.498847924999982, 16.66551459166665, 17.16551459166665, 17.498847924999982, 17.66551459166665, 17.832181258333318, 17.998847924999986, 18.165514591666653, 18.33218125833332, 18.49884792499999, 18.665514591666657, 19.165514591666657, 19.415514591666657, 19.665514591666657, 19.832181258333325, 19.998847924999993, 20.16551459166666, 20.41551459166666, 20.66551459166666, 22.16551459166666, 22.33218125833333, 22.498847924999996, 22.665514591666664, 23.165514591666664, 24.165514591666664, 24.665514591666664, 25.415514591666664, 25.665514591666664, 26.415514591666664, 26.665514591666664, 27.165514591666664, 27.665514591666664, 28.165514591666664, 28.415514591666664, 28.665514591666664, 29.165514591666664, 29.665514591666664, 30.165514591666664, 30.665514591666664, 31.165514591666664, 32.165514591666664, 32.665514591666664, 33.415514591666664, 33.665514591666664, 34.415514591666664, 34.665514591666664, 34.998847925, 35.165514591666664, 35.498847925, 35.665514591666664, 35.998847925, 36.165514591666664, 36.498847925, 36.665514591666664, 37.165514591666664, 37.33218125833333, 37.49884792499999, 37.66551459166666, 38.16551459166666, 38.33218125833332, 38.498847924999986, 38.66551459166665, 39.41551459166665, 39.66551459166665, 40.16551459166665, 40.66551459166665
};
double durations[] = {
        0.8321765916666665, 0.06828709166666666, 0.06828709166666666, 0.06828709166666666, 0.06828709166666666, 0.8321765916666665, 0.06828709166666666, 0.06828709166666666, 0.06828709166666666, 0.06828709166666666, 0.18402792499999995, 0.18402792499999995, 0.18402792499999995, 0.18402792499999995, 0.18402792499999995, 0.18402792499999995, 0.18402792499999995, 0.18402792499999995, 0.18402792499999995, 0.27662059166666664, 0.09143525833333331, 0.09143525833333331, 0.09143525833333331, 0.18402792499999995, 0.18402792499999995, 0.18402792499999995, 0.8321765916666665, 0.18402792499999995, 0.18402792499999995, 0.18402792499999995, 0.0828125, 0.0828125, 0.49947916666666664, 0.0828125, 0.0828125, 0.0828125, 0.24947916666666667, 0.24947916666666667, 0.24947916666666667, 0.24947916666666667, 0.16614583333333333, 0.0828125, 0.0828125, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.24947916666666667, 0.24947916666666667, 0.16614583333333333, 0.0828125, 0.0828125, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.7494791666666667, 0.0828125, 0.0828125, 0.0828125, 0.24947916666666667, 0.24947916666666667, 0.49947916666666664, 0.49947916666666664, 0.37447916666666664, 0.12447916666666667, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.49947916666666664, 0.3328125, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.49947916666666664, 0.24947916666666667, 0.24947916666666667, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.24947916666666667, 0.24947916666666667, 1.4994791666666667, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.49947916666666664, 0.9994791666666667, 0.49947916666666664, 0.7494791666666667, 0.24947916666666667, 0.7494791666666667, 0.24947916666666667, 0.37447916666666664, 0.49947916666666664, 0.49947916666666664, 0.24947916666666667, 0.24947916666666667, 0.49947916666666664, 0.49947916666666664, 0.49947916666666664, 0.49947916666666664, 0.49947916666666664, 0.9994791666666667, 0.49947916666666664, 0.7494791666666667, 0.24947916666666667, 0.7494791666666667, 0.24947916666666667, 0.3328125, 0.16614583333333333, 0.3328125, 0.16614583333333333, 0.3328125, 0.16614583333333333, 0.3328125, 0.16614583333333333, 0.49947916666666664, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.49947916666666664, 0.16614583333333333, 0.16614583333333333, 0.16614583333333333, 0.7494791666666667, 0.24947916666666667, 0.49947916666666664, 0.49947916666666664, 4.5
};
int note_count = 137;


void play_song() {
  servoLeft.detach();
  servoRight.detach();
  Serial.println("waiting for = to play song...");
  while (!Serial2.available() || Serial2.read() != '=') {
    delay(10);
  }
  Serial.println("PLAYING!");

  unsigned long start_time = millis();
  LCD.write(209);
  for (int i = 0; i < note_count; i++) {
    // Wait for note start time
    unsigned long current_time = millis() - start_time;
    long desired_time = start_times[i] * 1000;

    if (desired_time > current_time) {
      delay(desired_time - current_time);
    }
    // Calculate duration note
    LCD.write(get_duration_value(durations[i]));
    // Play note
    LCD.write(212 + octaves[i]);
    LCD.write(220 + notes[i]);
  }
}

int get_duration_value(double duration) {
  int duration_64ths = duration * 32;
  if (duration_64ths <= 1) {
    duration_64ths = 1;
  }

  int result = 214;
  int current_dur_64ths = 64;
  while (current_dur_64ths > duration_64ths) {
    current_dur_64ths /= 2;
    result -= 1;
  }
  return result;
}


