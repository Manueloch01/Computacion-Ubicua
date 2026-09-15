int LED = 3;
int BRILLO_ACTUAL = 0;
int BRILLO_OBJETIVO = 0;
int POT = 0;

unsigned long ultimoCambio = 0;
int velocidad = 8; // ms entre cada paso de brillo

void setup(){
  pinMode(LED, OUTPUT);
}

void loop(){
  BRILLO_OBJETIVO = analogRead(POT) / 4;

  if (millis() - ultimoCambio >= velocidad) {
    ultimoCambio = millis();

    if (BRILLO_ACTUAL < BRILLO_OBJETIVO) {
      BRILLO_ACTUAL++;
    } else if (BRILLO_ACTUAL > BRILLO_OBJETIVO) {
      BRILLO_ACTUAL--;
    }

    analogWrite(LED, BRILLO_ACTUAL);
  }
}