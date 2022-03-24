
#include <SPI.h>  
#include <Pixy.h>

Pixy pixy;

#define X_CENTER        ((PIXY_MAX_X-PIXY_MIN_X)/2)       
#define Y_CENTER        ((PIXY_MAX_Y-PIXY_MIN_Y)/2)
//Motor A
const int motorPin1  = 5;  //
const int motorPin2  = 6;  // Pin 10 of L293
//Motor B
const int motorPin3  = 10; // Pin  7 of L293
const int motorPin4  = 9;  // Pin  2 of L293

class ServoLoop
{
public:
  ServoLoop(int32_t pgain, int32_t dgain);

  void panUpdate(int32_t error ,int a ,int b);
  void tiltUpdate(int32_t error);
  int32_t m_pos;
  int32_t m_prevError;
  int32_t m_pgain;
  int32_t m_dgain;
  
};


ServoLoop panLoop(300, 500);
ServoLoop tiltLoop(500, 700);

ServoLoop::ServoLoop(int32_t pgain, int32_t dgain)
{
  m_pos = PIXY_RCS_CENTER_POS;
  m_pgain = pgain;
  m_dgain = dgain;
  m_prevError = 0x80000000L;

}

void ServoLoop::panUpdate(int32_t error, int a , int b)
{
  long int vel;
  char buf[32];
  
  
  if (m_prevError!=0x80000000)
  {	
    vel = (error*m_pgain + (error - m_prevError)*m_dgain)>>10;
    //sprintf(buf, "%ld\n", vel);
    //Serial.print(buf);2
    m_pos += vel;
    if (m_pos>(PIXY_RCS_MAX_POS-25)) 
      {
        m_pos = (PIXY_RCS_MAX_POS-25); 
       digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
   
    }
    else if (m_pos<(PIXY_RCS_MIN_POS+25)) 
      {m_pos = (PIXY_RCS_MIN_POS+25);
       digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
   
    }
    else {
          digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    
    }
  }
  
  m_prevError = error;
   

 if ( a>39 && b>28){
     digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
 }
 else
{
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
}
}

void ServoLoop::tiltUpdate(int32_t error)
{
  long int vel;
  char buf[32];
  
  
  if (m_prevError!=0x80000000)
  {	
    vel = (error*m_pgain + (error - m_prevError)*m_dgain)>>10;
    //sprintf(buf, "%ld\n", vel);
    //Serial.print(buf);
    m_pos += vel;
    if (m_pos>(PIXY_RCS_MAX_POS)) 
      {
        m_pos = (PIXY_RCS_MAX_POS); 
   
    }
    else if (m_pos<(PIXY_RCS_MIN_POS)) 
      {m_pos = (PIXY_RCS_MIN_POS);
   
    }
   
    
  }
  m_prevError = error;
}


void setup()
{
  Serial.begin(9600);
  Serial.print("Starting...\n");
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pixy.init();
}

void loop()
{ 
  static int i = 0;
  int j;
  uint16_t blocks;
  char buf[32]; 
  int32_t panError, tiltError;
  
  blocks = pixy.getBlocks();
  
  if (blocks)
  {
    int w = pixy.blocks[0].width;
    int h = pixy.blocks[0].height;
    panError = X_CENTER-pixy.blocks[0].x;
    tiltError = pixy.blocks[0].y-Y_CENTER;
    
    panLoop.panUpdate(panError,w,h);
    tiltLoop.tiltUpdate(tiltError);
    
    pixy.setServos(panLoop.m_pos, tiltLoop.m_pos);
   
   
 
    
    i++;
    
    // do this (print) every 50 frames because printing every
    // frame would bog down the Arduino
    if (i%50==0) 
    {
      sprintf(buf, "Detected %d:\n", blocks);
      Serial.print(buf);
      for (j=0; j<blocks; j++)
      {
        sprintf(buf, "  block %d: ", j);
        Serial.print(buf); 
        pixy.blocks[j].print();
        Serial.print(PIXY_RCS_MAX_POS);
   
      }
    }
  }  
}

