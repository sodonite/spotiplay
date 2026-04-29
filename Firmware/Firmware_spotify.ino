#include <SPI.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <SpotifyEsp32.h>
#include <Button2.h>

// --- DATOS DE CONEXIÓN ---
char* ssid = "SSID_USER";
char* password = "PASSWORD_USER";

const char* clientId = "CLIENTID_USER";
const char* clientSecret = "CLIENTSECRET_USER";

Spotify sp(clientId, clientSecret);

// --- PINES 
#define PIN_NEXT   0   
#define PIN_PREV   1   
#define PIN_PAUSE  4   
#define PIN_DJ     5   
#define PIN_LIKE   6   
#define PIN_REPEAT 7   
#define PIN_POT    3   // GPIO 3 para lectura analógica del volumen

// --- PINES DE LA PANTALLA ST7735 ---
#define TFT_CS     10  
#define TFT_DC     2   
#define TFT_RST    8   

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Button2 btnNext, btnPrev, btnPause, btnDJ, btnLike, btnRepeat;
int ultimoVol = -1;

void setup() {
  Serial.begin(115200);
  
  // Inicialización de la pantalla
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); 
  }
  Serial.println("\nWiFi Conectado");

  sp.begin();

  // --- CONFIGURACIÓN DE BOTONES ---
  btnNext.begin(PIN_NEXT);
  btnNext.setClickHandler([](Button2& b) { sp.skip_to_next(); });

  btnPrev.begin(PIN_PREV);
  btnPrev.setClickHandler([](Button2& b) { sp.skip_to_previous(); });

  btnPause.begin(PIN_PAUSE);
  btnPause.setClickHandler([](Button2& b) { sp.pause_playback(); }); 

  btnDJ.begin(PIN_DJ);
  btnDJ.setClickHandler([](Button2& b) { sp.toggle_shuffle(spotify_types::SHUFFLE_ON); });

  btnLike.begin(PIN_LIKE);
  btnLike.setClickHandler([](Button2& b) { 
    String id = sp.current_track_id();
    if (id != "" && id != "null") {
      sp.save_track(id); // Guarda la canción en tu biblioteca
    }
  });

  btnRepeat.begin(PIN_REPEAT);
  btnRepeat.setClickHandler([](Button2& b) { sp.set_repeat_mode(spotify_types::REPEAT_TRACK); });

  pinMode(PIN_POT, INPUT);
}

void loop() {
  btnNext.loop();
  btnPrev.loop();
  btnPause.loop();
  btnDJ.loop();
  btnLike.loop();
  btnRepeat.loop();

  // Lectura del volumen (Potenciómetro)
  int lectura = analogRead(PIN_POT);
  int volActual = map(lectura, 0, 4095, 0, 100);

  if (abs(volActual - ultimoVol) > 2) {
    ultimoVol = volActual;
    sp.set_volume(volActual);
    Serial.printf("Volumen Spotify: %d%%\n", volActual);
  }
}
