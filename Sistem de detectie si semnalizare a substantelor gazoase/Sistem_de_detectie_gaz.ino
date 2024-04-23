#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

//initializarea elementelor necesare pentru modulul GSM

SoftwareSerial sim800L(10, 11);
int timeout;
String buffer;
String PHONE = "+40736957010";

// initializarea unor contoare pentru afisajul display-ului

int numarator = 0;
int count = 0;

int ref = 10;  // valoarea de referinta pentru activarea semnalizarii prezentei de gaz

//initializarea pinilor pentru alarma sonora, senzori si contactele ventilatoarelor

int alarm = 2;
int MQ1 = A2;
int MQ2 = A1;
int MQ3 = A0;
int FanState1 = 3;
int FanState2 = 4;
int FanState3 = 5;

// intializarea pinilor pentru simularea pornirii/opririi instalatiei de gaz (led-uri)

int On = 6;
int Off = 7;

// initializarea unor steaguri daca s-a detectat gaz si daca s-a trimis mesajul

bool gasDetected = false;
bool messageSent = false;

// initializarea display-ului conform marimilor sale

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

  // se configureaza pornirea si functionarea programului si al modulului GSM
  // se declara pinii de intrare si iesire carora s-au initializat cate o valoare

  Serial.begin(9600);
  buffer.reserve(50);
  sim800L.begin(9600);
  pinMode(MQ1, OUTPUT);
  pinMode(MQ2, OUTPUT);
  pinMode(MQ3, OUTPUT);
  pinMode(alarm, OUTPUT);
  pinMode(FanState1, OUTPUT);
  pinMode(FanState2, OUTPUT);
  pinMode(FanState3, OUTPUT);
  pinMode(On, OUTPUT);
  pinMode(Off, OUTPUT);
  delay(2000);
}

// se configureaza mecanismul de trimitere a mesajelor

void send_sms(String text, String phone) {
  Serial.println("sending sms....");
  sim800L.print("AT+CMGF=1\r");  // se configureaza modulul pentru a trimite mesaje
  delay(200);
  sim800L.println("AT+CMGS=\"+40736957010\"\r");  // declararea destinatarului conform documentatiei tehnice a modulului
  delay(200);
  sim800L.print(text);
  sim800L.print("\r");
  unsigned long startMillis = millis();
  while (!sim800L.available() && millis() - startMillis < 10000) {
    // se asteapta raspunsul
  }
  sim800L.print((char)26);  //cod ascii pentru ctrl-26
  Serial.println("Text Sent.");
  delay(200);
  buffer = readSerial();
}

// functie ce asteapta valabilitatea datelor pe portul serial

String readSerial() {
  timeout = 0;
  while (!sim800L.available() && timeout < 12000) {
    delay(13);
    timeout++;
  }
  if (sim800L.available()) {
    return sim800L.readString();
  }
}

void loop() {

  // definirea statusului initial al pinilor ventilatoarelor si cel al led-urilor
  digitalWrite(FanState1, HIGH);
  digitalWrite(FanState2, HIGH);
  digitalWrite(FanState3, HIGH);
  digitalWrite(On, HIGH);
  digitalWrite(Off, LOW);

  // citirea valorilor inregistrate de senzori
  int sensor1 = analogRead(MQ1);
  int sensor2 = analogRead(MQ2);
  int sensor3 = analogRead(MQ3);

  // proiectarea unui mecanism de intarziere a afisajului pe display
  // display-ul va mentine timp de 5 secunde informatii despre fiecare senzor pe rand
  numarator = numarator + 1;
  if (numarator == 3) {
    count = count + 1;
  }
  if (numarator > 4) {
    numarator = 0;
  }
  if (count > 2) {
    count = 0;
  }

  //pornirea display-ului

  lcd.begin();
  lcd.backlight();

  // daca valoarea inregistrata de senzori nu este mai mare decat referinta
  // sistemul se afla in stadiul normal de functionare
  // prin urmare display-ul va afisa informatii despre fiecare senzor

  if (sensor1 < ref && sensor2 < ref && sensor3 < ref) {
    gasDetected = false;
    messageSent = false;

    switch (count) {
      case 0:
        lcd.print("Senzor 1:");
        lcd.setCursor(0, 1);
        lcd.print("Valoare: ");
        lcd.print(sensor1);  // se afiseaza valoarea senzorului 1
        lcd.print(" PPM");   // unitatea de masura
        break;
      case 1:
        lcd.print("Senzor 2:");
        lcd.setCursor(0, 1);
        lcd.print("Valoare: ");
        lcd.print(sensor2);  // se afiseaza valoarea senzorului 2
        lcd.print(" PPM");   // unitatea de masura
        break;
      case 2:
        lcd.print("Senzor 3:");
        lcd.setCursor(0, 1);
        lcd.print("Valoare: ");
        lcd.print(sensor3);  // se afiseaza valoarea senzorului 3
        lcd.print(" PPM");   // unitatea de masura
        break;
    }

    noTone(alarm);  // alarma dezactivata cat timp nu se detecteaza gaz
  } else {

    // daca unul dintre senzori inregistreaza o valoare mai mare decat referinta
    // se verifica specific care senzor a declansat alarma

    if (sensor1 > ref || sensor2 > ref || sensor3 > ref) {
      // Gas detected
      if (!gasDetected) {
        gasDetected = true;

        lcd.print("ATENTIE GAZ!!!");
        lcd.setCursor(0, 1);

        if (sensor1 > ref) {

          // daca Senzor 1 a declansat alarma, se vor oferi utilizatorului
          // informatii despre sectorul in care s-a produs detectia, sector aferent Senzorului 1

          lcd.print("Senzor 1");

          // se trimite un mesaj cu informatii despre locul detectiei

          if (!messageSent) {
            send_sms("DETECTIE GAZ IN SECTORUL 1!!! Pornire sistem de ventilatie/desfumare pe raza sectorului 1. Se efectueaza oprirea totala a instalatiei de gaz.", PHONE);
            messageSent = true;
          }

          for (;;) {
            digitalWrite(FanState1, LOW);  // se porneste sistemul de ventilatie din sectorul 1
            tone(alarm, 2000);             // se activeaza alarma
            digitalWrite(On, LOW);         // se dezactiveaza instalatia de gaz
            digitalWrite(Off, HIGH);
          }

          // se repeta acelasi procedeu pentru senzorul 2

        } else if (sensor2 > ref) {
          lcd.print("Senzor 2");

          // se trimite un mesaj cu informatii despre locul detectiei

          if (!messageSent) {
            send_sms("DETECTIE GAZ IN SECTORUL 2!!! Pornire sistem de ventilatie/desfumare pe raza sectorului 2. Se efectueaza oprirea totala a instalatiei de gaz.", PHONE);
            messageSent = true;
          }

          for (;;) {
            digitalWrite(FanState2, LOW);  // se porneste sistemul de ventilatie din setorul 2
            tone(alarm, 2000);             // se activeaza alarma
            digitalWrite(On, LOW);         // se dezactiveaza instalatia de gaz
            digitalWrite(Off, HIGH);
          }

          // se repeta acelasi procedeu pentru senzorul 3

        } else if (sensor3 > ref) {

          lcd.print("Senzor 3");

          // se trimite un mesaj cu informatii despre locul detectiei

          if (!messageSent) {
            send_sms("DETECTIE GAZ IN SECTORUL 3!!! Pornire sistem de ventilatie/desfumare pe raza sectorului 3. Se efectueaza oprirea totala a instalatiei de gaz.", PHONE);
            messageSent = true;
          }

          for (;;) {
            digitalWrite(FanState3, LOW);  // se porneste sistemul de ventilatie din setorul 3
            tone(alarm, 2000);             // se activeaza alarma
            digitalWrite(On, LOW);         // se dezactiveaza instalatia de gaz
            digitalWrite(Off, HIGH);
          }
        }
      }
    } else {
      // daca nivelul scade in limitele normale, sistemul revine in regimul normal de functionare
      gasDetected = false;
      messageSent = false;
      noTone(alarm);  
    }
  }
}