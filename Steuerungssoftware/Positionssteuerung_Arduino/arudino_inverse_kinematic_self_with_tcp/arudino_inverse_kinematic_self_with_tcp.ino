#include <stdio.h>

int BAUDRATE =9600;



class JOINT{
  public:
    JOINT(){MotorID=0;};

    void setMotorID(int y){MotorID=y;};
    void Configurate_Pins();
    int getID(){return MotorID;};
    //int getPinConfiguration(){return ENA;};
    String moveToPosition(int stepps, int ENA, int DIR, int STEP);
    
    const int STEP_1=8;
    const int DIR_1=9;
    const int ENA_1=10;

    const int STEP_2=11;
    const int DIR_2=12;
    const int ENA_2=13; 

    const int STEP_3=5;
    const int DIR_3=6;
    const int ENA_3=7;

    const int STEP_4=2;
    const int DIR_4=3;
    const int ENA_4=4;


  private:
    int MotorID;
    int ENA;
    int DIR;
    int STEP;

    
};

void JOINT::Configurate_Pins()
{
  switch(MotorID)
  {
    case 4:
      pinMode(STEP_4, OUTPUT);
      pinMode(DIR_4, OUTPUT);
      pinMode(ENA_4, OUTPUT);
      break;

    case 3: 
      pinMode(STEP_3, OUTPUT);
      pinMode(DIR_3, OUTPUT);
      pinMode(ENA_3, OUTPUT);
      break;
    
    case 1: 
      pinMode(STEP_1, OUTPUT);
      pinMode(DIR_1, OUTPUT);
      pinMode(ENA_1, OUTPUT);
      break;

    case 2:
      pinMode(STEP_2, OUTPUT);
      pinMode(DIR_2, OUTPUT);
      pinMode(ENA_2, OUTPUT);
      break;
  }


}


String JOINT::moveToPosition(int stepps, int ENA_, int DIR_, int STEP_)
{
  ENA=ENA_;
  DIR=DIR_;
  STEP=STEP_;

  digitalWrite(ENA, LOW);

  if(stepps<0)
  {
    digitalWrite(DIR, HIGH);
    for(int i=0;i>stepps;i--)
    {
      digitalWrite(STEP, HIGH);
      delay(1);
      digitalWrite(STEP, LOW);
    }
  }
  else if(stepps >0)
  {
    digitalWrite(DIR, LOW);
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
  DataFromPython.trim();  //remove \r and \n
  return DataFromPython;
}

void move_all()
{
      digitalWrite(11, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(2, HIGH);
      delay(1);
      digitalWrite(11, LOW);
      digitalWrite(5, LOW);
      digitalWrite(2, LOW);
}


void move_similar_to_position(int stepps_joint_2, int stepps_joint_3, int stepps_joint_4)
{

  digitalWrite(13, LOW);
  digitalWrite(7, LOW);
  digitalWrite(4, LOW);

  //rotation ausschalten dass nicht überhitzt
  digitalWrite(10, HIGH);

  while(true)
  {
    if((stepps_joint_2<0)&&(stepps_joint_3<0)&&(stepps_joint_4<0))
    {
      digitalWrite(12, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(3, HIGH);

      move_all();

      stepps_joint_2++;
      stepps_joint_3++;
      stepps_joint_4++;
    }
    else if((stepps_joint_2>0)&&(stepps_joint_3>0)&&(stepps_joint_4>0))
    {
      digitalWrite(12, LOW);
      digitalWrite(6, LOW);
      digitalWrite(3, LOW);

     move_all();

      stepps_joint_2--;
      stepps_joint_3--;
      stepps_joint_4--;
    }
    else if((stepps_joint_2>0)&&(stepps_joint_3<0)&&(stepps_joint_4<0))
    {
      digitalWrite(12, LOW);
      digitalWrite(6, HIGH);
      digitalWrite(3, HIGH);

      move_all();

      stepps_joint_2--;
      stepps_joint_3++;
      stepps_joint_4++;
    }
    else if((stepps_joint_2<0)&&(stepps_joint_3>0)&&(stepps_joint_4<0))
    {
      digitalWrite(12, HIGH);
      digitalWrite(6, LOW);
      digitalWrite(3, HIGH);

      move_all();

      stepps_joint_2++;
      stepps_joint_3--;
      stepps_joint_4++;
    }
    else if((stepps_joint_2<0)&&(stepps_joint_3<0)&&(stepps_joint_4>0))
    {
      digitalWrite(12, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(3, LOW);

      move_all();

      stepps_joint_2++;
      stepps_joint_3++;
      stepps_joint_4--;
    }
    else if((stepps_joint_2<0)&&(stepps_joint_3>0)&&(stepps_joint_4>0))
    {
      digitalWrite(12, HIGH);
      digitalWrite(6, LOW);
      digitalWrite(3, LOW);

      move_all();

      stepps_joint_2++;
      stepps_joint_3--;
      stepps_joint_4--;
    }
    else if((stepps_joint_2>0)&&(stepps_joint_3<0)&&(stepps_joint_4>0))
    {
      digitalWrite(12, LOW);
      digitalWrite(6, HIGH);
      digitalWrite(3, LOW);

      move_all();

      stepps_joint_2--;
      stepps_joint_3++;
      stepps_joint_4--;
    }
    else if((stepps_joint_2>0)&&(stepps_joint_3>0)&&(stepps_joint_4<0))
    {
      digitalWrite(12, LOW);
      digitalWrite(6, LOW);
      digitalWrite(3, HIGH);

      move_all();

      stepps_joint_2--;
      stepps_joint_3--;
      stepps_joint_4++;
    }

    //_________________________________________________________________________1=______________________________________________
    else if((stepps_joint_2>0)&&(stepps_joint_3==0)&&(stepps_joint_4==0))
    {
      digitalWrite(12, LOW);

      digitalWrite(11, HIGH);
      delay(1);
      digitalWrite(11, LOW);

      stepps_joint_2--;
    }
    else if((stepps_joint_2>0)&&(stepps_joint_3>0)&&(stepps_joint_4==0))
    {
      digitalWrite(12, LOW);
      digitalWrite(6, LOW);

      digitalWrite(11, HIGH);
      digitalWrite(5, HIGH);
      delay(1);
      digitalWrite(11, LOW);
      digitalWrite(5, LOW);

      stepps_joint_2--;
      stepps_joint_3--;
    }
    else if((stepps_joint_2==0)&&(stepps_joint_3>0)&&(stepps_joint_4==0))
    {
      digitalWrite(6, LOW);

      digitalWrite(5, HIGH);
      delay(1);
      digitalWrite(5, LOW);

      stepps_joint_3--;
    }

    else if((stepps_joint_2==0)&&(stepps_joint_3==0)&&(stepps_joint_4>0))
    {
      digitalWrite(3, LOW);

      digitalWrite(2, HIGH);
      delay(1);
      digitalWrite(2, LOW);

      stepps_joint_4--;
    }

    else if((stepps_joint_2==0)&&(stepps_joint_3>0)&&(stepps_joint_4>0))
    {
      digitalWrite(6, LOW);
      digitalWrite(3, LOW);

      digitalWrite(5, HIGH);
      digitalWrite(2, HIGH);
      delay(1);
      digitalWrite(5, LOW);
      digitalWrite(2, LOW);

      stepps_joint_3--;
      stepps_joint_4--;
    }
    else if((stepps_joint_2>0)&&(stepps_joint_3==0)&&(stepps_joint_4>0))
    {
      digitalWrite(12, LOW);
      digitalWrite(3, LOW);

      digitalWrite(11, HIGH);
      digitalWrite(2, HIGH);
      delay(1);
      digitalWrite(11, LOW);
      digitalWrite(2, LOW);

      stepps_joint_2--;
      stepps_joint_4--;
    }

    //______________________________________________________0=________________________________________________
    else if((stepps_joint_2<0)&&(stepps_joint_3==0)&&(stepps_joint_4==0))
    {
      digitalWrite(12, HIGH);

      digitalWrite(11, HIGH);
      delay(1);
      digitalWrite(11, LOW);

      stepps_joint_2++;
    }
    else if((stepps_joint_2<0)&&(stepps_joint_3<0)&&(stepps_joint_4==0))
    {
      digitalWrite(12, HIGH);
      digitalWrite(6, HIGH);

      digitalWrite(11, HIGH);
      digitalWrite(5, HIGH);
      delay(1);
      digitalWrite(11, LOW);
      digitalWrite(5, LOW);

      stepps_joint_2++;
      stepps_joint_3++;
    }
    else if((stepps_joint_2==0)&&(stepps_joint_3<0)&&(stepps_joint_4==0))
    {
      digitalWrite(6, HIGH);

      digitalWrite(5, HIGH);
      delay(1);
      digitalWrite(5, LOW);

      stepps_joint_3++;
    }

    else if((stepps_joint_2==0)&&(stepps_joint_3==0)&&(stepps_joint_4<0))
    {
      digitalWrite(3, HIGH);

      digitalWrite(2, HIGH);
      delay(1);
      digitalWrite(2, LOW);

      stepps_joint_4++;
    }

    else if((stepps_joint_2==0)&&(stepps_joint_3<0)&&(stepps_joint_4<0))
    {
      digitalWrite(6, HIGH);
      digitalWrite(3, HIGH);

      digitalWrite(5, HIGH);
      digitalWrite(2, HIGH);
      delay(1);
      digitalWrite(5, LOW);
      digitalWrite(2, LOW);

      stepps_joint_3++;
      stepps_joint_4++;
    }
    else if((stepps_joint_2<0)&&(stepps_joint_3==0)&&(stepps_joint_4<0))
    {
      digitalWrite(12, HIGH);
      digitalWrite(3, HIGH);

      digitalWrite(11, HIGH);
      digitalWrite(2, HIGH);
      delay(1);
      digitalWrite(11, LOW);
      digitalWrite(2, LOW);

      stepps_joint_2++;
      stepps_joint_4++;
    }
    //__________________________________________________________-01=__________________________________________________________
    else if((stepps_joint_2>0)&&(stepps_joint_3<0)&&(stepps_joint_4==0))
    {
      digitalWrite(12, LOW);
      digitalWrite(6, HIGH);

      digitalWrite(5, HIGH);
      digitalWrite(11, HIGH);
      delay(1);
      digitalWrite(5, LOW);
      digitalWrite(11, LOW);

      stepps_joint_2--;
      stepps_joint_3++;
    }
    else if((stepps_joint_2<0)&&(stepps_joint_3>0)&&(stepps_joint_4==0))
    {
      digitalWrite(12, HIGH);
      digitalWrite(6, LOW);

      digitalWrite(11, HIGH);
      digitalWrite(5, HIGH);
      delay(1);
      digitalWrite(11, LOW);
      digitalWrite(5, LOW);

      stepps_joint_2++;
      stepps_joint_3--;
    }
    else if((stepps_joint_2==0)&&(stepps_joint_3>0)&&(stepps_joint_4<0))
    {
      digitalWrite(6, LOW);
      digitalWrite(3, HIGH);

      digitalWrite(2, HIGH);
      digitalWrite(5, HIGH);
      delay(1);
      digitalWrite(5, LOW);
      digitalWrite(2, LOW);

      stepps_joint_3--;
      stepps_joint_4++;
    }

    else if((stepps_joint_2==0)&&(stepps_joint_3<0)&&(stepps_joint_4>0))
    {
      digitalWrite(6, HIGH);
      digitalWrite(3, LOW);

      digitalWrite(5, HIGH);
      digitalWrite(2, HIGH);
      delay(1);
      digitalWrite(5, LOW);
      digitalWrite(2, LOW);

      stepps_joint_3++;
      stepps_joint_4--;
    }

    else if((stepps_joint_2>0)&&(stepps_joint_3==0)&&(stepps_joint_4<0))
    {
      digitalWrite(12, LOW);
      digitalWrite(3, HIGH);

      digitalWrite(11, HIGH);
      digitalWrite(2, HIGH);
      delay(1);
      digitalWrite(11, LOW);
      digitalWrite(2, LOW);

      stepps_joint_2--;
      stepps_joint_4++;

    }
    else if((stepps_joint_2<0)&&(stepps_joint_3==0)&&(stepps_joint_4>0))
    {
      digitalWrite(12, HIGH);
      digitalWrite(3, LOW);

      digitalWrite(11, HIGH);
      digitalWrite(2, HIGH);
      delay(1);
      digitalWrite(11, LOW);
      digitalWrite(2, LOW);

      stepps_joint_2++;
      stepps_joint_4--;
    }
    
    
    else if((stepps_joint_2==0)&&(stepps_joint_3==0)&&(stepps_joint_4==0))
    {
      break;
    }
  }
}

JOINT Joint;

void setup()
{
  Serial.begin(BAUDRATE);
  while (!Serial)
    {
      ; /* wait for serial port to connect. Needed for native USB port only */
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  String Mode_from_python;
  String Joint_ID_from_python;
  String direction_from_python;
  String relative_stepps_from_python;
  String relative_position_from_python;

  while(true)
  {
    //einlesen des zu steuernden Joints
    Mode_from_python=read_Data_from_python();

    if(Mode_from_python=="end"){
      Serial.print("end./");
      break;
    }
    else if(Mode_from_python=="m")
    {
      Serial.println("manuell_mode");
      while(true)
      {
        Joint_ID_from_python=read_Data_from_python(); //Einlesen des Joints
        relative_stepps_from_python=read_Data_from_python();  //Einlesen der Stepps


        if(Joint_ID_from_python=="end")
        {
          Serial.println("Choose_Mode");
          break;
        }
        else if((Joint_ID_from_python=="1")or(Joint_ID_from_python=="2")or(Joint_ID_from_python=="3")or(Joint_ID_from_python=="4"))
        { 
          Joint.setMotorID(Joint_ID_from_python.toInt());
          Joint.Configurate_Pins();

          if(Joint_ID_from_python=="1")
          {Joint.moveToPosition(relative_stepps_from_python.toInt(), Joint.ENA_1, Joint.DIR_1, Joint.STEP_1);}
          else if(Joint_ID_from_python=="2")
          {Joint.moveToPosition(relative_stepps_from_python.toInt(), Joint.ENA_2, Joint.DIR_2, Joint.STEP_2);}
          else if(Joint_ID_from_python=="3")
          {Joint.moveToPosition(relative_stepps_from_python.toInt(), Joint.ENA_3, Joint.DIR_3, Joint.STEP_3);}
          else if(Joint_ID_from_python=="4")
          {Joint.moveToPosition(relative_stepps_from_python.toInt(), Joint.ENA_4, Joint.DIR_4, Joint.STEP_4);}  
          
          Serial.println("end_of_manuell_motion");
        }
        else
        {
          break;
        }
        
      }
    }
    else if(Mode_from_python=="a")
    {
      while(true)
      {
        Serial.println("automatic_mode");

        String stepps_alpha_joint_2=read_Data_from_python();
        delay(10);
        String stepps_beta_joint_3=read_Data_from_python();
        delay(10);
        String stepps_thetta_joint_4=read_Data_from_python();

        //Serial.print(stepps_alpha_joint_2);
        //delay(1000);
        //Serial.print(stepps_beta_joint_3);

        JOINT Joint_1;
        JOINT Joint_2;
        JOINT Joint_3;
        JOINT Joint_4;

        Joint_1.setMotorID(1);
        Joint_2.setMotorID(2);
        Joint_3.setMotorID(3);
        Joint_4.setMotorID(4);

        Joint_1.Configurate_Pins();
        Joint_2.Configurate_Pins();
        Joint_3.Configurate_Pins();
        Joint_4.Configurate_Pins();

        move_similar_to_position(stepps_alpha_joint_2.toInt(), stepps_beta_joint_3.toInt(), stepps_thetta_joint_4.toInt());

        Serial.println("end_of_automatic_motion"); 
      }
      
      
    }
    else
    {
      Serial.print("Fehlerhafte_Eingabe!");
      break;
    }
  }

  Serial.println("end./");
}


