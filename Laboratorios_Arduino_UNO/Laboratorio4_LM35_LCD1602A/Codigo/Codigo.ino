#include <LiquidCrystal.h>		

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);	
					
int SENSOR;		
float TEMPERATURA;	
float SUMA;		

void setup() {
  lcd.begin(16, 2);	
  
}

void loop() {
  SUMA = 0;					
  for (int i=0; i < 5; i++){			
  SENSOR = analogRead(A0);			
  TEMPERATURA = ((SENSOR * 5000.0) / 1023) / 10;
	
  SUMA = TEMPERATURA + SUMA;			
  delay(500);					
  }
  lcd.setCursor(0, 0);				
  lcd.print("Temp: ");				
  lcd.print(SUMA/5.0, 1);		

  lcd.print(" C");		
}