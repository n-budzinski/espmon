#include <OneWire.h>
#include <DallasTemperature.h>
#include <IRac.h>
#include <cmath>

const int oneWireBus = 5; //D1     
const int IRLED = 4;      //D2
const int buzzer = 2;     //D4

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
IRBosch144AC ac(IRLED);

int temperature = 0;
int newTemperature = 0;

///////////////////////////////////////////////////////////
// POCZĄTEK USTAWIEŃ ALGORYTMU TEMPERATURY
const int kroki = 3;
const int maksymalna_ujemna_temperatura = 22; //NA MINUSIE
const int nastaw_przy_najnizszej_temperaturze = 29;
const int maksymalna_dodatnia_temperatura = 16;
const int nastaw_przy_najwyzszej_temperaturze = 16;
const int odstep_pomiedzy_pomiarami = 1; //W MINUTACH
// KONIEC USTAWIEŃ ALGORYTMU TEMPERATURY               
///////////////////////////////////////////////////////////

void setup() {
  ac.begin();
  ac.calibrate();
  ac.setFan(25);
  ac.setMode(6); 
  Serial.begin(115200);
  sensors.begin();
}

void loop() {
  sensors.requestTemperatures();
  newTemperature = round(sensors.getTempCByIndex(0));
  if(temperature==-127){
    buzz(3);
  }
  else if(temperature!=newTemperature){
    Serial.println("Temperatura zmieniła się z ");
    Serial.print(temperature);
    Serial.print("ºC na ");
    Serial.print(newTemperature);
    Serial.print("ºC.");
    temperature = newTemperature;
    ac.setTemp(tempConversion(temperature));
    Serial.print(ac.toString());
    ac.send();
    buzz(1);
  }
  else{
    Serial.println("Temperatura nie zmieniła się od ostatniego odczytu (");
    Serial.print(temperature);
    Serial.print("ºC).");
    buzz(2);
  }
  delay(odstep_pomiedzy_pomiarami*60*1000);
}

void buzz(int code){
  delay(3000);
  for(int i=code; i<code; i++){
    tone(buzzer, 6000, 250);
    delay(1500);
  }
}

int tempConversion(int temp) {
  if(temp<-maksymalna_ujemna_temperatura){
    return nastaw_przy_najnizszej_temperaturze+1;
  }
  else if(temp>maksymalna_dodatnia_temperatura){
    return nastaw_przy_najwyzszej_temperaturze;
  }
  else{
    return (int)nastaw_przy_najnizszej_temperaturze-(maksymalna_ujemna_temperatura+temp)/kroki;
  }
}