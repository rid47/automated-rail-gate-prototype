//----------------------Including Required libraries-----------------------------//

#include <SoftwareSerial.h>
SoftwareSerial BTserial(2,3); // RX | TX



//----------------------Defining variables-------------------------------------//

char rc;
const int Train_switch;


//---------------------Setup--------------------------------------------------//

void setup() {

       Serial.begin(9600);
       BTserial.begin(9600);
       pinMode(13, OUTPUT);
       digitalWrite(13,LOW);

  // put your setup code here, to run once:

}

//---------------------------Main Loop----------------------------------------//

void loop() 
{ 
if (BTserial.available() > 0) {

      rc = BTserial.read();
      
      if (rc=='0') {

                digitalWrite(13,LOW);
                Serial.write("Train off");

                }

        if (rc=='1' ){

              
                digitalWrite(13,HIGH);
                Serial.write("Train on");
                }
  
}
}



