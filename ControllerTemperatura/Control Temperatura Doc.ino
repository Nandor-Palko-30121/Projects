int pwmPin = 9;   // Pin for PWM signal
int dutyCycle = 0;  // Duty cycle initialization at 0%
int ThermistorPin = A0;
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

void setup() {
  pinMode(pwmPin, OUTPUT);  // PWM pin set as output
  pinMode(ThermistorPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  Tc = T - 273.15;
  
  delay(100);

  dutyCycle = map(Vo, 550, 1000, 100, 255);
  
  if(Vo > 1000)
  dutyCycle = 255;

  if(Vo<550)
  dutyCycle = 0;

  analogWrite(pwmPin, dutyCycle);

  Serial.print (300);
  Serial.print (", ");
  Serial.print (0);
  Serial.print (", ");
  Serial.println(Tc);
}