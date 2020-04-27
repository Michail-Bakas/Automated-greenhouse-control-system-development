#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
SMSGSM sms;
int i;
int numdata;
char smsbuffer[160];
char n[20];
char sms_position;
char phone_number[20];
char sms_text[100];
char str[15];
String smsText ="";

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#include <DHT.h>;
#define DHTPIN 31     
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);
float igrasia;
float thermokrasia;

#include <Stepper.h>
#define motorSteps 200     
#define motorPin1 52
#define motorPin2 53
Stepper myStepper(motorSteps, motorPin1,motorPin2);

int photocellPin = 13;
int photocellReading;
 
int RaindropPin = 15;
int RaindropReading;

int StathmiPin = 14;
int StathmiReading;

#define DC_WATERPUMP_PIN 39
#define DC_FAN1_PIN 26
#define DC_FAN2_PIN 34

int ForesPotismatos = 0;
int ForesSMS = 0;
int ManualPotisma = 0;
int MetritisVradias = 0;



void setup()
{ 
  myStepper.setSpeed(1000);
  Serial.begin(9600);  
  
  dht.begin();
  
  lcd.begin(20,4); 
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);

  pinMode( DC_WATERPUMP_PIN, OUTPUT );
  pinMode( DC_FAN1_PIN, OUTPUT );
  pinMode( DC_FAN2_PIN, OUTPUT );    

  gsm.begin(9600);
  Serial.println("\nstatus=READY");

  if (gsm.begin(9600)){
    lcd.setCursor(0,2);
    lcd.print("GSM:");
    lcd.setCursor(4,2);
    lcd.print("CONNECTED");
  }
  else { 
    Serial.println("\nstatus=IDLE");
    lcd.setCursor(0,2);
    lcd.print("GSM:");
    lcd.setCursor(4,2);
    lcd.print("IDLE");
        
  }
        
  for(i=1;i<=5;i++)
       {
           sms.DeleteSMS(i);
       } 
  
}

void loop()
{
         delay(600);
         RaindropReading = analogRead(RaindropPin);
         StathmiReading = analogRead(StathmiPin);
         igrasia = dht.readHumidity();
         thermokrasia= dht.readTemperature();
         photocellReading = analogRead(photocellPin);


            sms_position=sms.IsSMSPresent(SMS_UNREAD);
            //Serial.print("SMS position:");
            //Serial.println(sms_position,DEC);
            sms.GetSMS(sms_position, phone_number, sms_text, 100);
            //Serial.println(phone_number);
            //Serial.println(sms_text);
            delay(1000);

            if (strcmp(sms_text,"STEILE")==0){

            lcd.setCursor(0,3);
            lcd.print("....SENDING DATA....");
            delay(1000);       
            smsText = String(igrasia)+"%/"+String(thermokrasia)+"C/"+String(StathmiReading)+"/"+String(RaindropReading)+"/"+String(ForesPotismatos);
            smsText.toCharArray(sms_text,160);
            sms.SendSMS("+306972344203",sms_text);
            }

            if (strcmp(sms_text,"POTISE")==0){
              if((ManualPotisma<1) && (ForesPotismatos<1)) {

            lcd.setCursor(0,3);
            lcd.print("AFTER DATA WATERING!");
            delay(1000);
            lcd.setCursor(0,3);
            lcd.print("...WATERING PLANS...");                
            digitalWrite( DC_WATERPUMP_PIN, HIGH );  
            delay(16000);
            digitalWrite( DC_WATERPUMP_PIN, LOW );
            delay(600);
            ManualPotisma = ManualPotisma + 1;       
            ForesPotismatos = ForesPotismatos +1;

            delay(1000);       
            smsText = String(igrasia)+"%/"+String(thermokrasia)+"C/"+String(StathmiReading)+"/"+String(RaindropReading)+"/"+String(ForesPotismatos);
            smsText.toCharArray(sms_text,160);
            sms.SendSMS("+306972344203",sms_text);
            
                  }


                  
 
      }

    if(ForesPotismatos<1) {
      if((photocellReading < 550) && (StathmiReading >300)){
              lcd.setCursor(0,3);
              lcd.print(".......NIGHT........");
              delay(500);
              ManualPotisma = 0;
              
              
                            
            if (igrasia<80){
                lcd.setCursor(0,3);
                lcd.print("...WATERING PLANS...");                
                digitalWrite( DC_WATERPUMP_PIN, HIGH );  
                delay(16000);
                digitalWrite( DC_WATERPUMP_PIN, LOW );
                ForesPotismatos = ForesPotismatos + 1;
                delay(600);
                lcd.setCursor(0,3);
                lcd.print("..FINISH WATERING...");
                                 
               }

           
           else if((igrasia>80) && (RaindropReading>800) ){          
              
            
                igrasia = dht.readHumidity();
                lcd.setCursor(0,3);
                lcd.print("FAN AND WINDOWS OPEN");
                digitalWrite( DC_FAN1_PIN, HIGH );
                digitalWrite( DC_FAN2_PIN, HIGH );
                myStepper.step(18000);
                delay(15000);
                digitalWrite( DC_FAN1_PIN, LOW );
                digitalWrite( DC_FAN2_PIN, LOW );
                myStepper.step(-18000);                
                lcd.setCursor(0,3);
                lcd.print(".......CLOSED.......");
                delay(2000);
                igrasia = dht.readHumidity();
                
               }
           
            
           else if((igrasia>80) && (RaindropReading<800) ){

            igrasia = dht.readHumidity();    
            while (igrasia > 80){
              
                
                igrasia = dht.readHumidity();
                lcd.setCursor(0,3);
                lcd.print("......FAN ONLY......");
                digitalWrite( DC_FAN1_PIN, HIGH );
                digitalWrite( DC_FAN2_PIN, HIGH );
                delay(2000); 
                igrasia = dht.readHumidity();
                
                break;
                 
                }
                
           while (igrasia < 80){
                delay(600);                 
                igrasia = dht.readHumidity();
                digitalWrite( DC_FAN1_PIN, LOW );
                digitalWrite( DC_FAN2_PIN, LOW );                
                lcd.setCursor(0,3);
                lcd.print(".......CLOSED.......");
                delay(2000);
                igrasia = dht.readHumidity();
                
                break;
                                
                }  
                
            }
        }      
   }


    
      


      if (photocellReading > 550){
          lcd.setCursor(0,3);
          lcd.print("........DAY.........");
          ForesPotismatos = 0;
             
         }


      if (photocellReading < 550){
          lcd.setCursor(0,3);
          lcd.print(".......NIGHT........");
          MetritisVradias = 0;
             
         }


      
    if (StathmiReading >300){

    lcd.setCursor(0,1);
    lcd.print("Water LvL:");
    lcd.setCursor(10,1);
    lcd.print("FULL");
    ForesSMS = 0;
  }

    
   if (StathmiReading <300) {
          
    lcd.setCursor(0,1);
    lcd.print("Water LvL:");
    lcd.setCursor(10,1);
    lcd.print("LOW!");
    lcd.setCursor(0,3);
    lcd.print("!!!!!!NO WATER!!!!!!");
    
    if (ForesSMS<1){
      
    lcd.setCursor(0,3);
    lcd.print("..SENDING LOW DATA..");
    delay(1000);       
    smsText = String(igrasia)+"%/"+String(thermokrasia)+"C/"+String(StathmiReading)+"/"+String(RaindropReading)+"/"+String(ForesPotismatos);
    smsText.toCharArray(sms_text,160);
    sms.SendSMS("+306972344203",sms_text);
    delay(10000);
    ForesSMS = ForesSMS + 1;    
    delay(1000);
    
                  }


   }            

    
    if ((photocellReading > 550) && (StathmiReading <300) && (MetritisVradias<1)){
      
    lcd.setCursor(0,3);
    lcd.print(".SEND MISS DATA DAY.");
    delay(1000);       
    smsText = String(igrasia)+"%/"+String(thermokrasia)+"C/"+String(StathmiReading)+"/"+String(RaindropReading)+"/"+String(ForesPotismatos);
    smsText.toCharArray(sms_text,160);
    sms.SendSMS("+306972344203",sms_text);
    delay(10000);
    MetritisVradias = MetritisVradias + 1;
    delay(1000);
    
                  }
  
if (RaindropReading > 800){
    lcd.setCursor(14,1);
    lcd.print("|");
    lcd.setCursor(15,1);
    lcd.print("CLEAR");
    
  }
  else if (RaindropReading <800){
    lcd.setCursor(14,1);
    lcd.print("|");
    lcd.setCursor(15,1);
    lcd.print("RAIN!");
    
  }
      
    lcd.setCursor(0,0);
    lcd.print("Hum|");    
    lcd.setCursor(4,0);
    lcd.print("Temp: ");
    lcd.setCursor(9,0);
    lcd.print(igrasia);
    lcd.setCursor(14,0);
    lcd.print("|");    
    lcd.setCursor(15,0);
    lcd.print(thermokrasia);
    lcd.setCursor(17,2);
    lcd.print("C:");
    lcd.setCursor(19,2);
    lcd.print(ForesPotismatos);
    


    
    
     //Serial.print("photocellReading: ");
     //Serial.println(photocellReading);     
     //Serial.print("RaindropReading: ");
     //Serial.println(RaindropReading);     
     //Serial.print("igrasia: ");
     //Serial.println(igrasia);
     //Serial.print("thermokrasia: ");
     //Serial.println(thermokrasia);     
     Serial.print("StathmiReading: ");
     Serial.println(StathmiReading);
     //Serial.print("ManualPotisma: ");
     //Serial.println(ManualPotisma);
     Serial.print("ForesSMS: ");
     Serial.println(ForesSMS);     
     Serial.print("MetritisVradias: ");
     Serial.println(MetritisVradias);

    


 }
