#include "Ultrasonic.h"

//Capteur ultra son 1
Ultrasonic ultrasonicRight(A2, A3); // Trig et Echo //pwm pour le 10 et analog pour le 8.
int distRight;

//Capteur ultra son 2
Ultrasonic ultrasonicLeft(A0, A1);  // Trig et Echo // Trig = A0 et Echo = A1.
int distLeft;

//Capteur ultra son 3
Ultrasonic ultrasonicFront(A4, A5);
int distFront;

//Moteur 1
const int controlPin1 = 2; // Connecté à la borne 2 de l'arduino qui est branché à la broche 7 du pont en H 
const int controlPin2 = 3; // Connecté à la borne 3 de l'arduino qui est branché à la broche 2 du pont en H  
const int enablePin = 9; // Connecté à la broche 1 du pont en H sert à allumer/étendre car brancher ensuite au bouton on/off

//Moteur 2
const int controlPin2_1 = 11; //Connecté à la borne 11 de l'arduino qui est branché à la broche 15 du pont en H
const int controlPin2_2 = 12; // Connecté à la borne 12 de l'arduino qui est branché à la broche 10 du pont en H
const int enablePin2 = 6; // Connecté à la broche 9 du pont en H sert à allumer/étendre car brancher ensuite au bouton on/off


const int onOffSwitchStateSwitchPin = 5; // Connecté à l'interrupteur de mise en marche/arrêt du moteur


// Création de variables pour stocker les valeurs des entrées 
int onOffSwitchState = 0; // État actuel de l'interrupteur marche/arrêt 
int previousOnOffSwitchState = 0; // État précédent de l'interrupteur marche/arrêt 



int motorEnabled = 0; // Moteur éteint = 0 ; moteur allumé = 1 ; Moteur allumé + toure à droite = 2 ; 
int motorSpeed = 0; // Vitesse du moteur 
int motorDirection = 1; // Direction actuelle du moteur

//Tourner à 90° :
int etape = 0; //etape 0 = tout droit à l'infini, Etape 1 = tout droit pdt tempsToutDroit, Etape 2 = on tourne à droite pdt tempsTourne, Etape 3 = on tourne à gauche pdt tempsTourne

int tempsTourne = 200;

int seuilReached = 0;




int tempsToutDroit = 800;


int tempsRecule = 1000;


int droiteGauche = 0;

int seuil = 110;


int dernierCoteDetecter = 0; //0 pour la droite, 1 pour la gauche.
//calcul de tempsTourne pour savoir au bout de combien de temps tourné : 
unsigned long debut = 0;

//pour les capteurs ultra sons, alternance entre les 3.
unsigned long lastPing = 0;
int capteurActuel = 0;

void setup() { 
// Initialisation des entrées et sorties 

  pinMode(onOffSwitchStateSwitchPin, INPUT); 
  
  //moteur 1
  pinMode(controlPin1, OUTPUT); 
  pinMode(controlPin2, OUTPUT); 
  pinMode(enablePin, OUTPUT);
  // Mettre la broche enable à LOW au démarrage comme ça le moteur ne s'allume pas d'un coup
  digitalWrite(enablePin, LOW);

  //moteur 2
  pinMode(controlPin2_1, OUTPUT); 
  pinMode(controlPin2_2, OUTPUT); 
  pinMode(enablePin2, OUTPUT); 
  // Mettre la broche enable à LOW au démarrage comme ça le moteur ne s'allume pas d'un coup
  digitalWrite(enablePin2, LOW);

  motorSpeed = 255;

  debut = millis(); // démarre le chrono
  Serial.begin(9600);  
}

void loop() { 
  // Lire l'état de l'interrupteur marche/arrêt 
  onOffSwitchState = digitalRead(onOffSwitchStateSwitchPin); 
  

  // alternance entre les 3 capteurs ultrasons.
  if (millis() - lastPing > 30) { // 30 ms entre chaque tir
    lastPing = millis();

    if (capteurActuel == 0) {
      distRight = ultrasonicRight.Ranging(CM);
    }
    else if (capteurActuel == 1) {
      distLeft = ultrasonicLeft.Ranging(CM);
    }
    else if (capteurActuel == 2) {
      distFront = ultrasonicFront.Ranging(CM);
    }

    capteurActuel = (capteurActuel + 1) % 3;
  }


  // Si le bouton marche/arrêt a changé d'état depuis la dernière boucle() 
  if (onOffSwitchState != previousOnOffSwitchState) { 
    // Inverser la valeur de motorEnabled si le bouton est pressé 
    if (onOffSwitchState == HIGH) { 
      motorEnabled = !motorEnabled; 
    } 
  }

//Action à effecteur en fonction de la proximité de l'objet calculer
  if(distFront < 20 && motorDirection == 1 && distFront > 0)
  {
    motorDirection = 0; //on recule
    debut = millis();
  }

  if(distRight > 0 && distRight <= 40 && dernierCoteDetecter == 1 && etape == 0 && motorDirection == 1){
    dernierCoteDetecter = 0;
    etape = 1;
    debut = millis();
  }

  if(distLeft > 0 && distLeft <= 40 && dernierCoteDetecter == 0 && etape == 0 && motorDirection == 1){
    dernierCoteDetecter = 1;
    etape = 1;    
    debut = millis(); 
  }

  
  // Changer la direction du moteur en agissant sur les broches de contrôle du pont en H 
  //on avance
  if (motorDirection == 1) { 
    if(etape == 0){
    //les 2 roues avancent pleines puissances
      //moteur 1
      digitalWrite(controlPin1, HIGH); 
      digitalWrite(controlPin2, LOW);

      //moteur 2
      digitalWrite(controlPin2_1, HIGH); 
      digitalWrite(controlPin2_2, LOW);
      seuilReached = 0;
    }
    else if(etape == 1){
      //moteur 1
      digitalWrite(controlPin1, HIGH); 
      digitalWrite(controlPin2, LOW);

      //moteur 2
      digitalWrite(controlPin2_1, HIGH); 
      digitalWrite(controlPin2_2, LOW);
      seuilReached = 0;
      
      //au bout de 1,350 secondes le robot arrete de tourner et va tout droit
      if (millis() - debut >= tempsToutDroit){
        if(dernierCoteDetecter == 0){
          //tourne vers la droite
          //Serial.println("On passe à l'étape 2 : on tourne à DROITE après avoir été tout droit pendant tempsToutDroit");
          etape = 2;
        }
        else{
          //tourne vers la gauche
          //Serial.println("On passe à l'étape 2 : on tourne à GAUCHE après avoir été tout droit pendant tempsToutDroit");
          etape = 3;
        }
        debut = millis();
      }
    }
    else if(etape == 2){
      //etape 1 : on tourne de 90° vers la droite (roue droite en marche arrière, roue gauche en marche avant).
      //moteur 1
      digitalWrite(controlPin1, HIGH); 
      digitalWrite(controlPin2, LOW);

      //moteur 2
      digitalWrite(controlPin2_1, LOW); 
      digitalWrite(controlPin2_2, HIGH);
      
      //Serial.println("On tourne etape 2");
      if(distFront <= seuil && seuilReached == 0 && distFront > 0)
      {
        debut = millis();
        seuilReached = 1;
        Serial.println("Seuil atteint quand tourne à droite");
      }
      //au bout de 1,350 secondes le robot arrete de tourner et va tout droit
      if (millis() - debut >= tempsTourne && seuilReached == 1){
        //Serial.println("On passe à l'étape 0 : tout droit à l'infini après avoir tourné à DROITE");
        etape = 0;
        debut = millis();
      }
    }
    else if(etape == 3){
      //etape 1 : on tourne de 90° vers la gauche (roue droite en marche avant, roue gauche en marche arrière).
      //moteur 1
      digitalWrite(controlPin1, LOW); 
      digitalWrite(controlPin2, HIGH);

      //moteur 2
      digitalWrite(controlPin2_1, HIGH); 
      digitalWrite(controlPin2_2, LOW);

      //Serial.println("On tourne etape 3");
      if(distFront <= seuil && seuilReached == 0 && distFront > 0)
      {
        seuilReached = 1;
        debut = millis();
        Serial.println("Seuil atteint quand tourne à gauche");
      }
      //au bout de 1,350 secondes le robot arrete de tourner et va tout droit
      if (millis() - debut >= tempsTourne && seuilReached == 1){
        //Serial.println("On passe à l'étape 0 : tout droit à l'infini après avoir tourné à GAUCHE");
        etape = 0;
        debut = millis();
      }
    }
  }
  //on recule
  else if(motorDirection == 0){ 

    //moteur 1
    digitalWrite(controlPin1, LOW); 
    digitalWrite(controlPin2, HIGH); 

    //moteur 2
    digitalWrite(controlPin2_1, LOW); 
    digitalWrite(controlPin2_2, HIGH);
    if (millis() - debut >= tempsRecule){
      motorDirection = 1;
      seuilReached = 1;
      if(dernierCoteDetecter == 0)
      {
        //Serial.println("Recule puis étape 2");
        etape = 2;
      }
      else{
        //Serial.println("Recule puis étape 2");
        etape = 3;
      }
      debut = millis();
    }
  }


  // Si le moteur doit être activé 
  if (motorEnabled == 1) { 
    analogWrite(enablePin, motorSpeed); 
    analogWrite(enablePin2, motorSpeed);
  } 
  else { // Si le moteur doit être éteint 
  // Arrêter le moteur 
    //Serial.println("ON s'arrête");
    analogWrite(enablePin, 0); 

    analogWrite(enablePin2, 0);
    etape = 0;
  }
  // Sauvegarder l'état actuel de l'interrupteur marche/arrêt comme précédent 
  previousOnOffSwitchState = onOffSwitchState; 
}