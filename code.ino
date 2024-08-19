#include "U8glib.h"
#include <DHT.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

int posicao = 0;
int temp_max = 45;
bool relayActivated = false;
bool showCoolingMessage = false;

#define pino_aumenta 5
#define pino_diminui 4
#define pino_rele 7
#define pino_DHT A0
#define DHTTYPE DHT11

DHT dht(pino_DHT, DHTTYPE);

int temperatura;
int aumenta, diminui;
int prev_temp_max = temp_max;

unsigned long previousMillis = 0;
const long interval = 10000;
const long welcomeDuration = 5000; // 5 seconds
const long coolingMessageDuration = 5000; // 5 seconds

void draw()
{
    // Retangulo temperatura atual
    u8g.drawRFrame(0, 17, 84, 46, 2);
    u8g.setFont(u8g_font_profont29 );// u8g_font_fub20
    // Atualiza a temperatura no display
    u8g.setPrintPos(9, 50);
    u8g.print(temperatura);
    // Circulo grau
    u8g.drawCircle(51, 33, 3);
    u8g.drawStr( 56, 50, "C");

    // Box superior amarelo
    u8g.setFont(u8g_font_tpss);
    u8g.drawRBox(0, 0, 128, 18, 0);
    u8g.setColorIndex(0);
    u8g.setPrintPos(posicao, 11);
    u8g.print("Ev Cooling System by ECE Department...!");
    u8g.setColorIndex(1);

    // Title "Battery Temperature"
    u8g.setFont(u8g_font_tpss);
    u8g.setPrintPos(10, 30);
    u8g.print("Battery Temp");

    // Box temperatura maxima
    u8g.drawRFrame(86, 17, 42, 46, 2);
    if (temp_max <= temperatura)
    {
        // Temperature greater than temp. SET
        u8g.drawRBox(86, 17, 42, 22, 2);
        u8g.setColorIndex(0);
        u8g.drawStr(96, 33, "SET");
        u8g.setColorIndex(1);
        // Aciona saida do rele
        digitalWrite(pino_rele, LOW);
        relayActivated = true;
    }
    else
    {
        // Temperature lower than temp. SET
        u8g.drawRFrame(86, 17, 42, 22, 2);
        u8g.drawStr(96, 33, "SET");
        // Desliga saida do rele
        digitalWrite(pino_rele, HIGH);
        relayActivated = false;
    }
    // Atualiza na tela o valor da temp. maxima
    u8g.setPrintPos(100, 55);
    u8g.print(temp_max);
    u8g.drawCircle(120, 47, 2);
    u8g.setColorIndex(1);
}

void welcomeMessage()
{
    u8g.setRot180(); // Rotate the display by 180 degrees for welcome message
    u8g.firstPage();
    do
    {
        u8g.setFont(u8g_font_profont29 );
        u8g.drawStr(10, 30, "Welcome to");
        u8g.setFont(u8g_font_profont15 );
        u8g.drawStr(10, 50, "Vaagdevi College");
    }
    while(u8g.nextPage());

    delay(welcomeDuration);
}

void coolingActivatedMessage()
{
    u8g.firstPage();
    do
    {
        u8g.setFont(u8g_font_profont15 );
        u8g.drawStr(10, 30, "Cooling System");
        u8g.drawStr(10, 50, "Activated");
        // Add fan symbol here if available
    }
    while(u8g.nextPage());
}

void coolingDeactivatedMessage()
{
    u8g.firstPage();
    do
    {
        u8g.setFont(u8g_font_profont15 );
        u8g.drawStr(10, 30, "Cooling System");
        u8g.drawStr(10, 50, "Deactivated");
        // Add fan symbol here if available
    }
    while(u8g.nextPage());
}

void setup(void)
{
    Serial.begin(9600);
    pinMode(pino_rele, OUTPUT);
    pinMode(pino_aumenta, INPUT);
    pinMode(pino_diminui, INPUT);
    dht.begin();

    // Display welcome message
    welcomeMessage();
}

void loop(void)
{
    unsigned long currentMillis = millis();
    // Timer para ler o valor da temperatura
    if (currentMillis - previousMillis >= interval)
    {
        temperatura = dht.readTemperature();
        previousMillis = currentMillis;
    }

    // Testa botao aumenta temperatura
    aumenta = digitalRead(pino_aumenta);
    if (aumenta == 1)
    {
        temp_max++;
        if (temp_max != prev_temp_max) {
            showCoolingMessage = false;
        }
        prev_temp_max = temp_max;
    }
    while (digitalRead(pino_aumenta) == 1)
    {
        delay(100);
    }

    // Testa botao diminui temperatura
    diminui = digitalRead(pino_diminui);
    if (diminui == 1)
    {
        temp_max--;
        if (temp_max != prev_temp_max) {
            showCoolingMessage = false;
        }
        prev_temp_max = temp_max;
    }
    while (digitalRead(pino_diminui) == 1)
    {
        delay(100);
    }

    // Scroll the text
    posicao--;
    if (posicao < -300)
    {
        posicao = 128; // Reset position to start
    }

    if (relayActivated && !showCoolingMessage)
    {
        coolingActivatedMessage();
        showCoolingMessage = true;
        delay(coolingMessageDuration);
    }
    else if (!relayActivated && showCoolingMessage)
    {
        coolingDeactivatedMessage();
        showCoolingMessage = false;
        delay(coolingMessageDuration);
    }
    else
    {
        // Rotate the display by 180 degrees for main GUI layout
        u8g.setRot180();
        u8g.firstPage();
        // Call the drawing routine on the screen
        do
        {
            draw();
        }
        while ( u8g.nextPage() );
    }

    delay(50);
}

