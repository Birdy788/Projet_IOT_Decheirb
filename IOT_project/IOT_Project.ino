#include <DHT.h>
#include <DHT_U.h>

#include <stdio.h>
#include <SPI.h>
#include "LoRa.h"
#include "DHT.h"

#define echoPin1 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin1 3 //attach pin D3 Arduino to pin Trig of HC-SR04

#define echoPin2 5 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin2 4 //attach pin D3 Arduino to pin Trig of HC-SR04

#define echoPin3 8 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin3 7 //attach pin D3 Arduino to pin Trig of HC-SR04

/* Ids---------------------------------------------------------------------------------------------*/
const int number_of_sensors = 1;
uint16_t id_temp            = 574; // to change
uint16_t id_node            = 242; // to change
uint16_t id_sensor[number_of_sensors]={id_temp};
float value_f[number_of_sensors];


/* LoRa module */ 
unsigned int freq = 865500000;//HZ


unsigned int cpt=1;
/*network */ 
#define WITH_APPKEY
unsigned int idlePeriodInMin = 1; // in minute
unsigned int nCycle = idlePeriodInMin*60;
unsigned short id_frame = 0;
#ifdef WITH_APPKEY
uint8_t my_appKey[4]={5, 6, 7, 8};
#endif
char m[100];
/*----------------------------------------------------------------------------------------------------*/



long duration1; // variable for the duration of sound wave travel
int distance1; // variable for the distance measurement

long duration2; // variable for the duration of sound wave travel
int distance2; // variable for the distance measurement

long duration3;
int distance3;

float Vest=0;

int F1=0;
int F2=0;
int F3=0;

float Vtot;
int etat;
const char* dayy;
float Vseuil=400;   // la vraie valeur est 22500cm^3 mais pour faire le test rapidement sur grafana on choisit la valeur arbitraire 400
//float Vref[3]={2,4,6};

float Vref1[3]={145,43.2,5.4};
float Vref2[3]={217,64.8,8.1};

//float Vref1[2]={6,3};
//float Vref[3]={6,4,2};
//float Vref2[3]={3,2,1};
//float Vref2[3]={1,2,3};

//const int ledControle = 13;                   // déclaration de la led pour contrôler que le programme tourne (1 clignotement par seconde)
const char* journee[] = {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};
const char* month[] = {"janvier", "février", "mars", "avril", "mai", "juin", "juillet", "août", "septembre", "octobre", "novembre", "décembre"};
byte jour, mois, heure, minute, seconde, jj, mm, tabulation;
boolean affsec = false, dateinit = false, changeheure = false, minuit = true;
String date = "",message = "", deuxpoints = " ";
unsigned long temps = 0;
int annee, aa;


void setup() 
{
  /*----------------------------Lora----------------------------*/
  //Serial.println("Set LoRa modulation\r\n");
  
  if (!LoRa.begin(freq)) {
    Serial.println("Starting LoRa failed!");
    while (1); 
  }
  
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setPreambleLength(8);
  LoRa.enableCrc();
  LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);
  
  /*------------------------------------------------------------*/
  
  pinMode(1,OUTPUT);
  pinMode(6,OUTPUT);

  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin1, INPUT); // Sets the echoPin as an INPUT
  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin2, INPUT); // Sets the echoPin as an INPUT
  
  pinMode(trigPin3, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin3, INPUT); // Sets the echoPin as an INPUT
  
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  Serial.println("Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
 
  // initialize the pushbutton pin as an input:
//  pinMode(magnet_switch, INPUT_PULLUP);

  //Serial.begin(9600);
  jour = 17;
  mois = 5;
  annee = 1944;
  heure = 0;
  minute = 0;
  temps = 0;
  Serial.begin(9600);                         // initialisation du port série à 9600 bauds
  jour = saisir(F("le jour: "), 1, 31);
  mois = saisir(F("le mois: "), 1, 12);
  annee = saisir(F("l'annee: "), 2021, 2100); // en 2100, que sera t'on devenu ????
  heure = saisir(F("l'heure: "), 0, 23);
  minute = saisir(F("les minutes: "), 0, 59);
  // on ne saisi pas les secondes car elles démarreront toujours à zéro
  Serial.print(F("Voulez-vous afficher les secondes ?(O/N) "));
  while (!Serial.available());                // tant qu'on n'a pas validé une saisie avec la touche "entrée"
  message = Serial.readStringUntil('\n');
  Serial.println(message);
  if (message == "o" || message == "O") affsec = true;
  dateinit = true;
  Serial.println(F("dateinitok"));
}

int saisir(String libelle, int mini, int maxi)
  {
Affiche:
  message = F("Saisissez ");
  message.concat(libelle);
  Serial.print(message);
  while (!Serial.available());      // tant qu'on n'a pas validé une saisie avec la touche "entrée"
  String saisie = "";
  saisie = Serial.readStringUntil('\n');
  int temp = ' ';
  temp = saisie.toInt();            // converti les caractères saisis en type entier
  Serial.println(temp);
  if (temp < mini || temp > maxi)   // pour comparer avec les valeurs mini et maxi du type entier
    {
    Serial.print(F("Valeur incorrecte, saisir de "));
    Serial.print(mini);
    Serial.print(F(" à "));
    Serial.println(maxi);
    goto Affiche;
    }
  else                              // sinon, la valeur saisie est comprise de mini a maxi
    { // on ne teste pas la cohérence du jour avec le mois (limite à 28, 29 ou 30) car quel est l'intérêt de saisir une date incohérente !
//Serial.print(F("SRAM restant libre = ")); // affiche l'évolution de la RAM qui reste disponible
//Serial.println(freeRam());
    return temp;
    }
  }

void loop() {
 
  cpt++;
  // put your main code here, to run repeatedly:
  Debut:
  if (millis() - temps < 0)
    {
    temps = millis();
    if (temps == 0) goto Debut;    // si débordement (~50 jours) du compteur de millisecondes, on conserve la date et l'heure actuelles
    }
  else if (dateinit == true)       // pour démarrer les secondes à 0 après la mise sous tension ou un reset, donc une MAJ date et heure
    {
    dateinit = false;
    temps = millis();
    }
  else
    {
    seconde = int((millis() - temps) / 1000);
    }
  if (seconde > 59)
    {
    seconde = 0;
    minute++;
    temps = millis();
    if (minute > 59)
      {
      minute = 0;
      heure++;
      if (heure > 23)
        {
        heure = 0;                              // il est exactement minuit
        jour++;
        if (mois == 1 || mois == 3 || mois == 5 || mois == 7 || mois == 8 || mois == 10 || mois == 12)  // pour tous les mois de 31 jours
          {
          if (jour > 31)
            {
            jour = 1;
            mois++;
            if (mois > 12)
              {
              mois = 1;
              annee++;
              }
            }
          }
        else if (mois == 2)                     // si c'est le mois de février
          {
          if (annee % 400 == 0)                 // et une année non bissextile
            {
            if (jour > 29)
              {
              jour = 1;
              mois++;
              }
            }
          else if ((annee % 4 == 0) && (annee % 100 != 0))  // et une année non bissextile
            {
            if (jour > 29)
              {
              jour = 1;
              mois++;
              }
            }
          else                                  // sinon, c'est février dans une année bissextile donc de 28 jours
            {
            if (jour > 28)
              {
              jour = 1;
              mois++;
              }
            }
          }
        else                                    // sinon, c'est un mois de 30 jours
          {
          if (jour > 30)
            {
            jour = 1;
            mois++;
            }
          }
        minuit = true;                          // il est exactement minuit donc on vient de changer de jour (24 heures se sont écoulées)
        }
      }
    }
  message = "";
  if (minuit == true)
    {
    minuit = false;
    if (mois == 1 || mois == 2)                 // pour le calcul du jour dans la semaine selon la date
      {
      aa = annee - 1;                           // pour janvier et février
      mm = mois + 10;
      }
    else
      {
      aa = annee;                               // pour tous les autres mois
      mm = mois - 2;
      }
    jj = (jour + aa + aa / 4 - aa / 100 + aa / 400 + (31 * mm) / 12) % 7; // calcul de l'indice du jour dans la semaine
    dayy = journee[jj];
//     digitalWrite(ledControle, !digitalRead(ledControle)); // clignotement pour contrôler que le programme tourne
     delay(1000);  
    }
 
  while(Vest < Vseuil){
  F1=0;
  F2=0;
  F3=0;   
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration1 = pulseIn(echoPin1, HIGH);

  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration2 = pulseIn(echoPin2, HIGH);

  digitalWrite(trigPin3, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin3, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
     duration3 = pulseIn(echoPin3, HIGH);
     distance3 = duration3 * 0.034 / 2;
  
 
  // Calculating the distance
  distance1 = duration1 * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  distance2 = duration2 * 0.034 / 2;
  
  if ( distance1 < 23 ){
    F1=1;
  }
  else{
    F1=F1;
  }
  if ( distance2 < 23 ){
    F2=1;
  }
    else{
    F2=F2;
  }
  
 
    if (digitalRead(1) == HIGH)
      {
       // bleu 6
        //Serial.println("Switch Closed");
        //delay(1000);
     
      //while (digitalRead(magnet_switch) == LOW) {}
      }
      else if (digitalRead(6) == HIGH)  {
      //Serial.println("Switch Opened");
      // digitalWrite(4, HIGH);
      // digitalWrite(3, LOW);
      //delay(1000);
     // Serial.println("Switch Opened");
      if (dayy == "Samedi" || dayy == "Dimanche" ){
        if(11<=heure<=14 || 18<=heure<=21){
          if( F1==1 & F2==1 ){   // faisceaux
           Vest+= 217;//Vref2[0] ;  // estimer les déchets jetés
          }
          else if ( F1==1 & F2==0 ){
             Vest+=145;  //Vref1[0];
          }
          else if ( F1==0 & F2==1 ){
             Vest+=145;   //Vref1[0];
          }
          else{
            Vest=Vest;
          }
      }
       /* else{
          if( F1==1 & F2==1 ){   // faisceaux
           Vest+=Vref1[2];   // estimer les déchets jetés
          }
          else if (F1==1 & F2==0){
             Vest+=Vref2[2];
          }
          else if (F1==0 & F2==1 ){
             Vest+=Vref2[2];
          }
          else{
            Vest=Vest;
          }
        }*/
       }
       else {
        //Vest+=10; //a remplire ça depend de l'heure
        if(11<=heure<=14 || 18<=heure<=21){
          if( F1==1 & F2==1 ){   // faisceaux
           Vest+= 64.8;  //Vref1[1] ;  // estimer les déchets jetés
          }
          else if ( F1==1 & F2==0 ){
             Vest+= 43.2; //Vref2[1];
          }
          else if ( F1==0 & F2==1 ){
             Vest+= 43.2; //Vref2[1];
          }
          else{
            Vest=Vest;
          }
      }
        else{
          if( F1==1 & F2==1 ){   // faisceaux
           Vest+=8.1;   //Vref1[2];   // estimer les déchets jetés
          }
          else if (F1==1 & F2==0){
             Vest+=5.4;  //Vref2[2];
          }
          else if (F1==0 & F2==1 ){
             Vest+=5.4; // Vref2[2];
          }
          else{
            Vest=Vest;
          }
        } 
       }
       
      }
      else{
        Serial.println("Error");
      }
     
      //Serial.println(distance1);
      //Serial.println(distance2);
      Serial.println(Vest);
      delay(500);

    //  value_f[0] = Vest;
  /*Send Data via LoRa Module*/
    // send_data(value_f);

   /* wait for #idlePeriodInMin Minute */
     //for(int j=0;j<nCycle;j++){delay(1000);} 
}

if ( Vest>=Vseuil && distance3 < 5 ){
    Serial.println("Necessarily intervention"); 
    value_f[0] = 1; 
    send_data(value_f);

   /* wait for #idlePeriodInMin Minute */
     for(int j=0;j<nCycle;j++){delay(1000);} 
  }
else{
  Vest=300;  //la vraie valeur est 20000 cm^3 ms pour faire le test on choisit 200  
}
 
}

/*----------------------------------------------------send_data-----------------------*/
void send_data(float * value){
   
  uint8_t app_key_offset = 0;
  int e;
  #ifdef WITH_APPKEY
      app_key_offset = sizeof(my_appKey);
      memcpy(m,my_appKey,app_key_offset);
  #endif
      uint8_t r_size;
      char final_str[80] = "\\";
      char aux[10] = "";
      char id[1] = "";
      sprintf(final_str, "%s!%i!%hd", final_str,id_node, id_frame++);
      for (int i=0; i<number_of_sensors; i++) {
              sprintf(aux,"%4.2f", value[i]);
              sprintf(final_str, "%s#%d/%s", final_str, id_sensor[i], aux);
      }

      r_size=sprintf(m+app_key_offset,final_str);
    
      Serial.println(m);
      //Serial.println(r_size);
      
      LoRa.beginPacket();
      LoRa.print(m);
      LoRa.endPacket(); 
      
     Serial.println("DATA sent correctly !");
  
      for(uint8_t j=0;j<number_of_sensors;j++){
        value[j]=0.0;
      }
}
