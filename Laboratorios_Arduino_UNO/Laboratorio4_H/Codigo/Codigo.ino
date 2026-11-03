#include <DHT.h>
#include <LiquidCrystal.h>

// Definir el pin donde conectaste el DHT22 (A0)
#define DHTPIN A0     
#define DHTTYPE DHT22   // Definimos que es el modelo DHT22

// Inicializamos el sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Inicializamos la pantalla LCD con los pines conectados en tu circuito:
// RS = 7, Enable = 6, D4 = 5, D5 = 4, D6 = 3, D7 = 2
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

void setup() {
  // Inicializar la pantalla LCD indicando columnas (16) y filas (2)
  lcd.begin(16, 2);
  
  // Iniciar el sensor DHT
  dht.begin();
  
  // Mensaje de bienvenida opcional
  lcd.print("Iniciando DHT22");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Esperar 2 segundos entre mediciones (el DHT22 toma un tiempo en actualizar)
  delay(2000);

  // Leer la humedad
  float h = dht.readHumidity();
  // Leer la temperatura en grados Celsius
  float t = dht.readTemperature();

  // Comprobar si hubo un error en la lectura
  if (isnan(h) || isnan(isnan(t))) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error leyendo");
    lcd.setCursor(0, 1);
    lcd.print("sensor DHT22");
    return;
  }

  // Mostrar los datos en la pantalla LCD
  lcd.clear();
  
  // Fila 0: Temperatura
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print(" C");

  // Fila 1: Humedad
  lcd.setCursor(0, 1);
  lcd.print("Humedad: ");
  lcd.print(h);
  lcd.print(" %");
}