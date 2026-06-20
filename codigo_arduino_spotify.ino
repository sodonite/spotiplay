#include <SPI.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <SpotifyEsp32.h>
#include <Button2.h>

// --- DATOS DE CONEXIÓN ---
char* ssid = "Lemmor54";
char* password = "3114610624lemmor";

const char* clientId = "59fcb2cb9c33426c81ee0573bc5d90d5";
const char* clientSecret = "20a0d50c512647c18bb31fb691df3cdc";

Spotify sp(clientId, clientSecret);

#define PIN_NEXT   32
#define PIN_PREV   33
#define PIN_PAUSE  25
#define PIN_DJ     26
#define PIN_LIKE   27
#define PIN_REPEAT 14
#define PIN_POT    34 

// --- PINES DE LA PANTALLA ST7735 ---
#define TFT_CS     5   // Chip Select
#define TFT_DC     2   // Data/Command
#define TFT_RST    4   // Reset
// Nota: SDI(MOSI) va al pin 23 y SCK al pin 18 del ESP32 por defecto

// --- DECLARACIÓN DE OBJETOS ---
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Button2 btnNext, btnPrev, btnPause, btnDJ, btnLike, btnRepeat;
int ultimoVol = -1;

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi Conectado");

  sp.begin();

  // --- CONFIGURACIÓN DE BOTONES ---
  
  btnNext.begin(PIN_NEXT);
  btnNext.setClickHandler([](Button2& b) { sp.skip_to_next(); });

  btnPrev.begin(PIN_PREV);
  btnPrev.setClickHandler([](Button2& b) { sp.skip_to_previous(); });

  // 1.
  btnPause.begin(PIN_PAUSE);
  btnPause.setClickHandler([](Button2& b) { sp.pause_playback(); }); 

  // 2.
  btnDJ.begin(PIN_DJ);
  btnDJ.setClickHandler([](Button2& b) { sp.toggle_shuffle(spotify_types::SHUFFLE_ON); });

  // 3. El Like se llama save_track
  btnLike.begin(PIN_LIKE);
  btnLike.setClickHandler([](Button2& b) { 
    String id = sp.current_track_id();
    if (id != "" && id != "null") {
      sp.get_users_saved_tracks();
    }
  });

  // 4. El repetir se llama set_repeat_mode
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

  int lectura = analogRead(PIN_POT);
  int volActual = map(lectura, 0, 4095, 0, 100);

  if (abs(volActual - ultimoVol) > 2) {
    ultimoVol = volActual;
    sp.set_volume(volActual);
    Serial.printf("Volumen: %d%%\n", volActual);
  }
}