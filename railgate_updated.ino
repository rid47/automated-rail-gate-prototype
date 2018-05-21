

//------------------------------Defining required pins----------------------------------//
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MUX_A D2
#define MUX_B D3
#define ANALOG_INPUT A0


//-----------------------------Defining required variables------------------------------//

volatile int sensorValue;
int closingldrValue=0;
int openingldrValue=0;
int obstacleldrValue1=0;
int obstacleldrValue2=0;
int openGate=0,closeGate=80,haltGate=40;
Servo servo1,servo2,servo3,servo4; 

//----------------wifi & Mqtt----------------------------------//
const char* ssid = "DataSoft_WiFi";
const char* password = "support123";
const char* mqtt_server = "182.163.112.207";
WiFiClient espClient;
PubSubClient client(espClient);

// Reading only closingldr until it senses train (1:read; 0:no need to read) 

int closingldrRead=1;
int obstacleldrRead=0;
int openingldrRead=0;
int gateStatus=0;

//----------------------------Setup function--------------------------------------------//

void setup() {
  
  Serial.begin(115200);
  //Deifne output pins for Mux
  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);   
  pinMode(D1,OUTPUT);
   pinMode(D4,OUTPUT);

   digitalWrite(D4, 1);
   
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.connect("lens_WLZak2eSNKqaIWqJhvHruTAtlvf");
//  client.setCallback(callback);

  
  servo1.attach(D5);
  servo2.attach(D6);
  servo3.attach(D7);
  servo4.attach(D8);
  
  servo1.write(160);//open
  servo4.write(160);//open 
  
  servo2.write(10);//open
  servo3.write(10);//open
        
}

//-----------------------------wifi_Setup---------------------------------//
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//---------------------------Main Loop---------------------------------------------------//

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (closingldrRead==1){
          
          
          
          closingldrValue=gateClosingLDR();
          Serial.print("closingldrValue:");
          Serial.println(closingldrValue);

          

        

          
          if (closingldrValue<900)//Approaching train detected
          
          {
                Serial.println("Train approaching railgate");
                client.publish("ds/railgate", "1");
                digitalWrite(D1, 1);
                digitalWrite(D4, 0);
                closingldrRead=0;
                obstacleldrRead=1;
                
                
                
          }
  }


//--------------------------Reading obstacle sensor while train is approaching-----------------------------//


  if (obstacleldrRead==1){
  
          obstacleldrValue1=obstaclSensigLDR1();
          obstacleldrValue2=obstaclSensigLDR2();
          Serial.print("obstacleldrValue1:");
          Serial.println(obstacleldrValue1);
           Serial.print("obstacleldrValue2:");
          Serial.println(obstacleldrValue2);
          
          if (obstacleldrValue1<600 && obstacleldrValue2<600)// Obstacle @ both lane
        
          {
            digitalWrite(D1, 0);
            Serial.println("Half close gate");
            Serial.println(" Obstacle ahead");
            client.publish("ds/railgate", "Obstacle ahead");
             servo1.write(120);//half
             servo4.write(120);//half
             servo2.write(45);//half
             servo3.write(45);//half
             
             digitalWrite(D1, 1);
          }
          

if (obstacleldrValue1>600 && obstacleldrValue2<600)// Obstacle @ lane 1
        
          {
            digitalWrite(D1, 0);
            Serial.println("Half close gate");
            Serial.println(" Obstacle ahead");
             client.publish("ds/railgate", "Obstacle ahead");
            //servo3.write(45);//half
            servo1.write(120);//half
            servo4.write(120);//half
            delay(450);
            //servo1.write(80);//close
            servo2.write(77); //close
            servo3.write(84);//close
            digitalWrite(D1, 1);
          }
          
if (obstacleldrValue1<600 && obstacleldrValue2>600)// Obstacle @ lane 2
        
          {
            digitalWrite(D1, 0);
            Serial.println("Half close gate");
            Serial.println(" Obstacle ahead");
             client.publish("ds/railgate", "Obstacle ahead");
             servo1.write(120);//half
             //servo2.write(45);//half
             servo4.write(120);//half
             delay(450);
             
             //servo4.write(80);//close
             
             servo2.write(77);
             servo3.write(84);//close
             
             digitalWrite(D1, 1);
            
          }
 
 if (obstacleldrValue1>600 && obstacleldrValue2>600)// No obstacle 
          {
        
           //Serial.println("No obstacle on the Rail line");
          // Serial.println("Completely close the gate");
           Serial.println(" Clear to go");
            client.publish("ds/railgate", "Clear to go");
            delay(500);
           servo1.write(75);//close
           servo4.write(70);//close

           delay(450);// delay for cross effect in closing rail gates
           
           servo2.write(77);//close
           servo3.write(84);//close
           
        
           obstacleldrRead=0;
           openingldrRead=1;
          
          }
  }
  
 

  if (openingldrRead==1){
       openingldrValue=gateOpeningLDR();
       Serial.print("openingldrValue:");
       Serial.println(openingldrValue);
       if(openingldrValue<900)//Departing train detected
        { 
          Serial.println("Departing train detected");
          client.publish("ds/railgate", "Departed");
             client.publish("ds/railgate", "2");
          //gateStatus=1;


       servo1.write(160);//open
       servo4.write(160);//open

       delay(450); //Delay for cross effect while opening the gate
       
       servo2.write(10);//open
       servo3.write(10);//open
//       delay(1500);
       closingldrRead=1;
       openingldrRead=0;
       gateStatus=0;
       digitalWrite(D4, 1);
       digitalWrite(D1, 0);
        
        }
  }

//  for (int i=0;i<3;i++){

//  openingldrValue=gateOpeningLDR();
//  
//  if (gateStatus==1 && openingldrValue>900)
//  {
//
//       Serial.println("Train departed;open the gate");
//       
//       servo1.write(160);//open
//       servo4.write(160);//open
//
//       delay(450); //Delay for cross effect while opening the gate
//       
//       servo2.write(10);//open
//       servo3.write(10);//open
////       delay(1500);
//       closingldrRead=1;
//       openingldrRead=0;
//       gateStatus=0;
//       digitalWrite(D4, 1);
//       digitalWrite(D1, 0);
//      
//}  
//  
//  
}


//--------------------------Reading approaching train detector sensor----------------------------------//


int gateClosingLDR(){

//for(int j=0;j<50;j++){
  changeMux(1,0);
  sensorValue = analogRead(ANALOG_INPUT); 
  //Serial.print("Obstacle sensing LDR value:");
  //Serial.println(sensorValue);
  delay(300);
  return sensorValue;
  
  //}
}


//------------------------Reading obstacle on both lane---------------------------------------------//

int obstaclSensigLDR1(){
//for(int k=0;k<50;k++){
  changeMux(1,1);
  sensorValue = analogRead(ANALOG_INPUT); 
  //Serial.print("Gate closing LDR value:");
  //Serial.println(sensorValue);
  delay(300);
  
  return sensorValue;
  
}
 int obstaclSensigLDR2(){

  changeMux(0,1);
  sensorValue = analogRead(ANALOG_INPUT); 

  delay(300);
  
  return sensorValue;
 

}

//-----------------------------------------Reading departing train sensor-----------------------//

int gateOpeningLDR()
{ 

//for(int i=0;i<50;i++){
  changeMux(0,0);
  sensorValue = analogRead(ANALOG_INPUT);
  //Serial.print("Gate opening LDR value:");
  //Serial.println(sensorValue);
  delay(300);
  return sensorValue;
  
  
  //}
}

//------------------------------------------MUX pin selection to choose what sensor to read----//

void changeMux(int b, int a) {
  digitalWrite(MUX_B, b);
  digitalWrite(MUX_A, a);
  
}

//----------------------------------------Reconnect For Wifi-----------------------//
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
//    if (client.connect("lens_1mJ2TB5UT8FvNmgubTcd1u7rQvm")) {
      if (client.connect("lens_WLZak2eSNKqaIWqJhvHruTAtlvf")) {
      Serial.println("connected");
    
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
