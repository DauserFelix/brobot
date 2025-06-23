#include <stdio.h>
int STEP;
int DIR;
int ENA;

int BAUDRATE =9600;



class JOINT{
  public:
    JOINT(){MotorID=0;};

    void setMotorID(int y){MotorID=y;};
    void Configurate_Pins();
    int getID(){return MotorID;};
    int getPinConfiguration(){return ENA;};
    String moveToPosition(int stepps);


  private:
    int MotorID;
};

void JOINT::Configurate_Pins()
{
  switch(MotorID)
  {
    case 4: 
      STEP=2;
      DIR=3;
      ENA=4;
      break;

    case 3: 
      STEP=5;
      DIR=6;
      ENA=7;
      break;
    
    case 1: 
      STEP=8;
      DIR=9;
      ENA=10;
      break;

    case 2: 
      STEP=11;
      DIR=12;
      ENA=13;
      break;
  }

  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENA, OUTPUT);

}


String JOINT::moveToPosition(int stepps)
{
  digitalWrite(ENA, LOW);

  if(stepps<0)
  {
    digitalWrite(DIR, LOW);
    for(int i=0;i>stepps;i--)
    {
      digitalWrite(STEP, HIGH);
      delay(1);
      digitalWrite(STEP, LOW);
    }
  }
  else if(stepps >0)
  {
    digitalWrite(DIR, HIGH);
    for(int i=0;i<stepps;i++)
    {
      digitalWrite(STEP, HIGH);
      delay(1);
      digitalWrite(STEP, LOW);
    }
  }
  else
  {
    //stepps==0
  }

  return "x";
}

String read_Data_from_python()
{
  String DataFromPython;
  while(Serial.available()==0)
  {
    //Warten bis Daten verfügbar sind
  }
  DataFromPython= Serial.readString();
  //DataFromPython.trim();  //remove \r and \n
  return DataFromPython;
}


JOINT Joint;

void setup()
{
  Serial.begin(BAUDRATE);
  while (!Serial)
    {
      ; /* wait for serial port to connect. Needed for native USB port only */
    }

  Serial.println("Prepare Arduino");

  

  Serial.println("Preparations for Arduino done!");

}

void loop() {
  // put your main code here, to run repeatedly:
  String Joint_ID_from_python;
  String direction_from_python;
  String relative_stepps_from_python;
  

  while(true)
  {
    //einlesen des zu steuernden Joints
    Joint_ID_from_python=read_Data_from_python();

    if(Joint_ID_from_python=="end"){
      break;
    }
    else
    {
      Joint.setMotorID(Joint_ID_from_python.toInt());
      Joint.Configurate_Pins();

      //ansteuern des zu steuernden Joints
      Serial.println("moveJoint");
      relative_stepps_from_python=read_Data_from_python();
      Joint.moveToPosition(relative_stepps_from_python.toInt());
      Serial.println("end_of_motion");
    }
  }

  Serial.println("end./");
}


