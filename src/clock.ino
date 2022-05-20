#include <Arduino.h>
#include <U8g2lib.h>
#include <RTClib.h>
#include <Button.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

/*

Function: Volt-meter
Author: https://www.luisllamas.es/analogicas-mas-precisas/

*/

long readVcc() {
  long result;
  // Situamos la referencia de voltaje en 1.1V
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Esperamos que se estabilice el voltaje
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Devolvemos el voltaje en mV
  return result;
}

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
RTC_DS3231 rtc;
const long InternalReferenceVoltage = 1080L;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const unsigned long intervalo = 5000;
unsigned long previousTime = 0;
unsigned long previousMillis;
int menu = 0;
bool apagado;
bool aviso;
Button button1(2);

void setup(void) {
  u8g2.begin(/* menu_select_pin= */ 2, /* menu_next_pin= */ 4, /* menu_prev_pin= */ 7, /* menu_up_pin= */ 6, /* menu_down_pin= */ 5, /* menu_home_pin= */ 3);
  button1.begin();
  ADMUX = (0 << REFS1) | (0 << REFS0) | (0 << ADLAR) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0);
  delay(50);
}

void loop(void) {
  
  if (aviso == true) {
    
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setFontRefHeightAll(); 
    u8g2.userInterfaceMessage("Bateria baja", "Recomendado su carga", "La hora no se perderá", " Ok \n Cancelar ");
  }

  if (button1.pressed()) {
    if (apagado == true) {
      menu + 2;
      u8g2.setPowerSave(0);
      apagado = false;
      aviso = false;
    }
    previousMillis = millis() / 1000;
    if (apagado == false) {
      menu++;
    }
  }
  if (menu >= 2) {
    menu = 0;
  }
  switch (menu) {
    case 0: {
        DateTime now = rtc.now();
        int hora = now.hour();
        int minutos = now.minute();
        int load = 0;
        uint8_t par;
        u8g2.clearBuffer();
        u8g2.setCursor(27, 30);
        u8g2.setFont(u8g2_font_inr24_mn );
        u8g2.print(hora);
        par = now.second() & 0b1;
        if (par)
        {
          //numero impar
          u8g2.print(" ");
        }
        else
        {
          //numero par
          u8g2.print(":");
        }
        if (minutos < 10)
        {
          u8g2.print("0");
        }
        else
        {
          // Posición del texto
          u8g2.print("");
        }
        u8g2.print(minutos);
        u8g2.drawHLine(20, 35, 100);
          if ((hora >= 0) && (hora <= 8)) {
            u8g2.setFont(u8g2_font_crox5h_tf );
            u8g2.setCursor(30, 60);
            u8g2.print("Valle");

          }
          else if ((hora >= 8) && (hora <= 10)) {
            u8g2.setFont(u8g2_font_crox5h_tf );
            u8g2.setCursor(30, 60);
            u8g2.print("Llana");

          }
          else if ((hora >= 14) && (hora <= 18)) {
            u8g2.setFont(u8g2_font_crox5h_tf );
            u8g2.setCursor(30, 60);
            u8g2.print("Llana");

          }
          else if ((hora >= 22) && (hora <= 0)) {
            u8g2.setFont(u8g2_font_crox5h_tf );
            u8g2.setCursor(30, 60);
            u8g2.print("Llana");

          }
          else if ((hora >= 10) && (hora <= 14)) {
            u8g2.setFont(u8g2_font_crox5h_tf );
            u8g2.setCursor(30, 60);
            u8g2.print("Punta");

          }
          else if ((hora >= 18) && (hora <= 22)) {
            u8g2.setFont(u8g2_font_crox5h_tf );
            u8g2.setCursor(30, 60);
            u8g2.print("Punta");

          }
        }
        if (millis() / 1000 - previousMillis == 10) {
          u8g2.setPowerSave(1);
          apagado = true;
        }
        u8g2.sendBuffer();
        break;
      

    case 1: {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_osr18_tr );
        u8g2.setCursor(27, 40);
        u8g2.print( (float) (readVcc() + 770) / 1000, 2 );
        if ((float) (readVcc() + 770) / 1000 < 3.2){
          aviso = true;
        }
        u8g2.print("");
        u8g2.print(" V");
        u8g2.sendBuffer();
        break;
      }
  }

  u8g2.sendBuffer();
}
