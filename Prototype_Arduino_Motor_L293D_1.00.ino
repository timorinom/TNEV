#include "Ultrasonic.h"

Ultrasonic ultrasonic(10, 8); // Trig et Echo //10 cables pwm pour le 10 et analog pour le 8.



//Moteur 1
const int controlPin1 = 2; // Connecté à la borne 2 de l'arduino qui est branché à la broche 7 du pont en H 
const int controlPin2 = 3; // Connecté à la borne 3 de l'arduino qui est branché à la broche 2 du pont en H  
const int enablePin = 9; // Connecté à la broche 1 du pont en H sert à allumer/étendre car brancher ensuite au bouton on/off

//Moteur 2
const int controlPin2_1 = 11; //Connecté à la borne 11 de l'arduino qui est branché à la broche 15 du pont en H
const int controlPin2_2 = 12; // Connecté à la borne 12 de l'arduino qui est branché à la broche 10 du pont en H
const int enablePin2 = 6; // Connecté à la broche 9 du pont en H sert à allumer/étendre car brancher ensuite au bouton on/off

///A retirer///
const int directionSwitchPin = 4; // Connecté à l'interrupteur de direction

const int onOffSwitchStateSwitchPin = 5; // Connecté à l'interrupteur de mise en marche/arrêt du moteur

//const int potPin = A0; // Connecté à la sortie du potentiomètre pour si on veut modifier la vitesse manuellement

//vitesse
//int vitesseMax = 250;
int vitesseTurn = 80;

bool Turn = false; //pour savoir si le robot doit tourner ou non.

// Création de variables pour stocker les valeurs des entrées 
int onOffSwitchState = 0; // État actuel de l'interrupteur marche/arrêt 
int previousOnOffSwitchState = 0; // État précédent de l'interrupteur marche/arrêt 
int directionSwitchState = 0; // État actuel de l'interrupteur de direction 
int previousDirectionSwitchState = 0; // État précédent de l'interrupteur de direction

int motorEnabled = 0; // Moteur éteint = 0 ; moteur allumé = 1 ; Moteur allumé + toure à droite = 2 ; 
int motorSpeed = 0; // Vitesse du moteur 
int motorDirection = 1; // Direction actuelle du moteur

void setup() { 
// Initialisation des entrées et sorties 

  ///A retirer///
  pinMode(directionSwitchPin, INPUT); 

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

  Serial.begin(9600);
}

void loop() { 
  // Lire l'état de l'interrupteur marche/arrêt 
  onOffSwitchState = digitalRead(onOffSwitchStateSwitchPin); 
  delay(1);

  //capteur ultrason
  int dist = ultrasonic.Ranging(CM);
  Serial.print(dist);
  Serial.println(" cm");

  delay(100);
  //fin capteur ultrason
  
  ///A retirer///
  //directionSwitchState = digitalRead(directionSwitchPin);
  
  // Lire la valeur du potentiomètre et la diviser par 4 
  // pour obtenir une valeur utilisable en PWM 
  //motorSpeed = analogRead(potPin) / 4; //a mettre ou enlever en fonction de l'utilisation ou non du potentiomètre, pour modifier manuellement la vitesse
  //motorSpeed = (motorSpeed + 10)%250;
  motorSpeed = 250;

  // Si le bouton marche/arrêt a changé d'état depuis la dernière boucle() 
  if (onOffSwitchState != previousOnOffSwitchState) { 
  // Inverser la valeur de motorEnabled si le bouton est pressé 
  if (onOffSwitchState == HIGH) { 
    motorEnabled = !motorEnabled; 
    } 
  }


  //Appelle fonction capteur ultra son pour connaitre distance avec l'obstacle, passé en mode recule si trop près
  //Script non changé depuis le script de base 13/03
  // Si le bouton de direction a changé d'état depuis la dernière boucle() 
  /*
  if (directionSwitchState != previousDirectionSwitchState) { 
  // Inverser la valeur de motorDirection si le bouton est pressé 
  if (directionSwitchState == HIGH) { 
    motorDirection = !motorDirection; 
    motorSpeed = 0;
    } 
  }
*/
//Action à effecteur en fonction de la proximité de l'objet calculer
  if (dist <= 10) { 
    //Serial.println("Danger objet trop près !");
    //Turn = false;
    motorDirection = 0;
    //moteur recule
    Turn = false;
  } 
  else{
    motorDirection = 1;
    //Turn = false;
    //moteur avance tout droit
    if(dist <= 35){
      //Serial.println("Tourné");
      Turn = true;
    }
    else{
      Turn = false;
    }
  }
  

  // Changer la direction du moteur en agissant sur les broches de contrôle 
  // du pont en H 
  if (motorDirection == 1) { 
    //moteur 1
    digitalWrite(controlPin1, HIGH); 
    digitalWrite(controlPin2, LOW);

    //moteur 2
    digitalWrite(controlPin2_1, HIGH); 
    digitalWrite(controlPin2_2, LOW);
  }
  else { 
    //moteur 1
    digitalWrite(controlPin1, LOW); 
    digitalWrite(controlPin2, HIGH); 

    //moteur 2
    digitalWrite(controlPin2_1, LOW); 
    digitalWrite(controlPin2_2, HIGH);
  }

  // Si le moteur doit être activé 
  if (motorEnabled == 1) { 
    //Serial.println("on avance plein puissance");
  // Appliquer un signal PWM sur la broche enable pour ajuster la vitesse 
    //Si tout droit pleine vitesse : 
    if(Turn == false){
      analogWrite(enablePin, motorSpeed); 
      analogWrite(enablePin2, motorSpeed);
    }
    //Sinon si il faut tourné
    //virage en amont de l'objet détécté par ralentissement d'une roue et l'autre toujours à pleine puissance
    else{
      Serial.println("ON TOURNE");
      analogWrite(enablePin, vitesseTurn); 

      analogWrite(enablePin2, motorSpeed);
    }
  } 
  else { // Si le moteur doit être éteint 
  // Arrêter le moteur 
    Serial.println("ON s'arrête");
    analogWrite(enablePin, 0); 

    analogWrite(enablePin2, 0);
  }

  // Sauvegarder l'état actuel de l'interrupteur de direction comme précédent 
  previousDirectionSwitchState = directionSwitchState; 
  // Sauvegarder l'état actuel de l'interrupteur marche/arrêt comme précédent 
  previousOnOffSwitchState = onOffSwitchState; 
}