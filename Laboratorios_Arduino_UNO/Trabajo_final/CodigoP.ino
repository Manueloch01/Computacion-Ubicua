// PARQUEADERO: barrera + contador de cupos + LCD I2C
// 2 sensores HC-SR04, servo (barrera) y LCD 16x2 I2C (version FISICA)
// Los eventos tambien salen por el Serial Monitor.

#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---- Pines ----
const int TRIG_ENTRADA = 5;
const int ECHO_ENTRADA = 4;
const int TRIG_SALIDA  = 3;
const int ECHO_SALIDA  = 6;
const int PIN_SERVO    = 10;
// LCD I2C: SDA y SCL (pines dedicados del Arduino, junto a AREF)

// ---- Configuracion ----
const byte LCD_DIR = 0x27;                   // direccion I2C (si no muestra nada, prueba 0x3F)
const int CUPOS_TOTALES = 4;                 // capacidad del parqueadero
const int DIST_DETECCION = 15;               // cm: menos de esto = hay carro
const int ANGULO_CERRADA = 0;
const int ANGULO_ABIERTA = 90;
const unsigned long ESPERA_MAX_MS = 6000;    // tiempo maximo con la barrera arriba
const bool DEPURAR = true;                   // true = distancias en el Serial Monitor

LiquidCrystal_I2C lcd(LCD_DIR, 16, 2);
Servo barrera;
int cuposLibres = CUPOS_TOTALES;

// Mide la distancia en cm (999 si no hay eco)
long medirDistancia(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duracion = pulseIn(echo, HIGH, 30000);
  if (duracion == 0) return 999;
  return duracion * 0.0343 / 2;
}

bool hayCarro(int trig, int echo) {
  return medirDistancia(trig, echo) < DIST_DETECCION;
}

void mensaje(const char* linea1, const char* linea2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(linea1);
  lcd.setCursor(0, 1);
  lcd.print(linea2);
}

void mostrarCupos() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  PARQUEADERO");
  lcd.setCursor(0, 1);
  if (cuposLibres > 0) {
    lcd.print("Cupos libres: ");
    lcd.print(cuposLibres);
  } else {
    lcd.print("  * LLENO *");
  }
}

// Abre la barrera, espera a que el carro llegue al otro sensor y
// se aleje (o se acabe el tiempo), y la cierra
void dejarPasar(int trigOtro, int echoOtro) {
  barrera.write(ANGULO_ABIERTA);
  unsigned long inicio = millis();
  bool llegoAlOtro = false;
  while (millis() - inicio < ESPERA_MAX_MS) {
    if (hayCarro(trigOtro, echoOtro)) {
      llegoAlOtro = true;
    } else if (llegoAlOtro) {
      break;                                 // ya paso y el sensor quedo libre
    }
    delay(50);
  }
  delay(500);
  barrera.write(ANGULO_CERRADA);
}

// Espera a que ningun sensor tenga un objeto enfrente
void esperarViaLibre() {
  while (hayCarro(TRIG_ENTRADA, ECHO_ENTRADA) || hayCarro(TRIG_SALIDA, ECHO_SALIDA)) {
    delay(100);
  }
}

void setup() {
  Serial.begin(9600);

  lcd.init();                                // primero la pantalla
  lcd.backlight();
  mensaje("  BIENVENIDO", " Sistema listo");
  delay(1500);

  pinMode(TRIG_ENTRADA, OUTPUT);
  pinMode(ECHO_ENTRADA, INPUT);
  pinMode(TRIG_SALIDA, OUTPUT);
  pinMode(ECHO_SALIDA, INPUT);

  barrera.attach(PIN_SERVO);                 // despues el servo
  barrera.write(ANGULO_CERRADA);

  mostrarCupos();
  Serial.println("Sistema listo");
  Serial.print("Cupos libres: ");
  Serial.println(cuposLibres);
}

void loop() {
  long dEntrada = medirDistancia(TRIG_ENTRADA, ECHO_ENTRADA);
  delay(30);                                 // evita que un eco afecte al otro sensor
  long dSalida = medirDistancia(TRIG_SALIDA, ECHO_SALIDA);

  if (DEPURAR) {
    Serial.print("Entrada: ");
    Serial.print(dEntrada);
    Serial.print(" cm | Salida: ");
    Serial.print(dSalida);
    Serial.print(" cm | Libres: ");
    Serial.println(cuposLibres);
  }

  bool carroEntrada = dEntrada < DIST_DETECCION;
  bool carroSalida  = dSalida  < DIST_DETECCION;

  // Carro que quiere ENTRAR
  if (carroEntrada && !carroSalida) {
    if (cuposLibres > 0) {
      cuposLibres--;                         // se registra el carro
      Serial.println(">> Entra un carro");
      mensaje("   BIENVENIDO", "  Puede pasar");
      dejarPasar(TRIG_SALIDA, ECHO_SALIDA);
      esperarViaLibre();
      mostrarCupos();
      Serial.print("Cupos libres: ");
      Serial.println(cuposLibres);
    } else {
      Serial.println(">> LLENO: no hay cupos");
      mensaje("  * LLENO *", " No hay cupos");
      delay(1500);
      mostrarCupos();
    }
  }

  // Carro que quiere SALIR
  else if (carroSalida && !carroEntrada) {
    if (cuposLibres < CUPOS_TOTALES) {
      cuposLibres++;                         // el cupo queda libre
      Serial.println(">> Sale un carro");
      mensaje("  HASTA PRONTO", "  Puede salir");
      dejarPasar(TRIG_ENTRADA, ECHO_ENTRADA);
      esperarViaLibre();
      mostrarCupos();
      Serial.print("Cupos libres: ");
      Serial.println(cuposLibres);
    } else {
      Serial.println(">> No hay carros adentro");
      mensaje(" Sin carros", " adentro");
      delay(1500);
      mostrarCupos();
    }
  }

  delay(50);
}