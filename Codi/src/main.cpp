/*
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <WiFi.h>
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

const char* ssid = "ReactionTrainer";
const char* password = "reaction123";

AsyncWebServer server(80);

enum Nivel {FACIL, MEDIO, DIFICIL};
Nivel nivel_dificultad;
unsigned long umbral_10, umbral_8, umbral_6;
void cargarRanking();
void guardarRanking();
void actualizarRanking(Nivel nivel, int nueva_nota);
String generarPaginaRanking();
void configurarServidorWeb();

void configurarServidorWeb() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", generarPaginaRanking());
  });

WiFi.mode(WIFI_AP);
WiFi.softAP(ssid, password);

Serial.println("🔌 Punto de acceso iniciado");
Serial.println(WiFi.softAPIP());

  server.begin();
}

// Configuración de pines
const int sensor_pins[] = {1, 5, 47, 45};
const int led_pins[] = {3, 15, 40, 41};
const int num_targets = 4;

const int buzzer_pin = 10;
const int boton_facil = 2;
const int boton_medio = 42;
const int boton_dificil = 38;

const char* archivo_ranking = "/ranking.txt";
const int max_ranking = 3;
std::vector<int> ranking_facil;
std::vector<int> ranking_medio;
std::vector<int> ranking_dificil;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int current_target = -1;
bool previous_states[4] = {HIGH, HIGH, HIGH, HIGH};

unsigned long start_time;
const unsigned long game_duration = 20000;
bool juego_activo = true;

unsigned long led_on_time;
int errores = 0;
int aciertos = 0;

const int max_intentos = 100;
unsigned long tiempos[max_intentos];
int tiempo_index = 0;


String generarPaginaRanking() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Ranking</title>";
  html += "<style>body{font-family:sans-serif;background:#f0f0f0;padding:20px;}table{border-collapse:collapse;width:100%;}th,td{border:1px solid #aaa;padding:8px;text-align:center;background:#fff;}h2{margin-top:30px;}</style></head><body>";

  html += "<h1>Ranking de Reflejos</h1>";

  auto tabla = [](String titulo, std::vector<int>& ranking) {
    String t = "<h2>" + titulo + "</h2><table><tr><th>Posición</th><th>Puntuación</th></tr>";
    for (int i = 0; i < ranking.size(); i++) {
      t += "<tr><td>" + String(i + 1) + "</td><td>" + String(ranking[i]) + "</td></tr>";
    }
    t += "</table>";
    return t;
  };

  html += tabla("Fácil", ranking_facil);
  html += tabla("Medio", ranking_medio);
  html += tabla("Difícil", ranking_dificil);

  html += "</body></html>";
  return html;
}

void mostrarCuentaAtras(int numero) {
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(55, 20);
  display.print(numero);
  display.display();
}

void mostrarMensajeInicio() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 25);
  display.print("Empieza");
  display.display();
}

void mostrarNotaFinal(int nota) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 0);
  display.print("Nota final");
  display.setTextSize(4);
  display.setCursor(50, 20);
  display.print(nota);
  display.setTextSize(1);
  display.setCursor(0, 55);
  display.display();
}

void configurarDificultad(Nivel nivel) {
  switch (nivel) {
    case FACIL:
      umbral_10 = 500;
      umbral_8 = 600;
      umbral_6 = 700;
      Serial.println("🎯 Modo: FÁCIL");
      break;
    case MEDIO:
      umbral_10 = 300;
      umbral_8 = 400;
      umbral_6 = 500;
      Serial.println("🎯 Modo: MEDIO");
      break;
    case DIFICIL:
      umbral_10 = 200;
      umbral_8 = 300;
      umbral_6 = 400;
      Serial.println("🎯 Modo: DIFÍCIL");
      break;
  }
}

void elegirDificultad() {
  Serial.println("Selecciona la dificultad con los botones:");
  Serial.println("GPIO 2: Fácil, GPIO 42: Medio, GPIO 38: Difícil");

  while (true) {
    if (digitalRead(boton_facil) == LOW) {
      nivel_dificultad = FACIL;
      Serial.println("Fácil seleccionado");
      break;
    }
    if (digitalRead(boton_medio) == LOW) {
      nivel_dificultad = MEDIO;
      Serial.println("Medio seleccionado");
      break;
    }
    if (digitalRead(boton_dificil) == LOW) {
      nivel_dificultad = DIFICIL;
      Serial.println("Difícil seleccionado");
      break;
    }
    delay(50);
  }
}

void nuevoTurno() {
  current_target = random(0, num_targets);
  led_on_time = millis();

  Serial.print("🔆 Nuevo objetivo: LED ");
  Serial.println(current_target + 1);

  for (int i = 0; i < num_targets; i++) {
    digitalWrite(led_pins[i], i == current_target ? HIGH : LOW);
  }
}

void evaluarTiempo(unsigned long tiempo) {
  if (tiempo < 200) Serial.println("🔥 ¡Excelente reflejo!");
  else if (tiempo < 300) Serial.println("💪 ¡Muy bien!");
  else if (tiempo < 500) Serial.println("👍 Bien, pero puedes mejorar.");
  else Serial.println("🐢 Reacción lenta, ¡intenta más rápido!");
}

void finalizarJuego() {
  Serial.println("⏱️ ¡Se acabó el tiempo! Fin del ejercicio.");
  for (int i = 0; i < num_targets; i++) {
    digitalWrite(led_pins[i], LOW);
  }
  juego_activo = false;

  Serial.println("📊 RESULTADOS FINALES:");
  Serial.print("✅ Aciertos: "); Serial.println(aciertos);
  Serial.print("❌ Errores: "); Serial.println(errores);

  if (tiempo_index > 0) {
    unsigned long suma = 0;
    for (int i = 0; i < tiempo_index; i++) suma += tiempos[i];
    unsigned long media = suma / tiempo_index;

    Serial.print("📥 Tiempo medio de reacción: "); Serial.print(media); Serial.println(" ms");

    int nota_base;
    if (media < umbral_10) nota_base = 10;
    else if (media < umbral_8) nota_base = 8;
    else if (media < umbral_6) nota_base = 6;
    else nota_base = 4;

    int penalizacion = errores / 3;
    int nota_final = max(0, nota_base - penalizacion);

    Serial.print("📉 Penalización por errores: -"); Serial.print(penalizacion); Serial.println(" puntos");
    Serial.print("🏁 Nota final: "); Serial.println(nota_final);

    Serial.print("📝 Comentario: ");
    if (nota_final == 10) Serial.println("¡Reflejos perfectos! Eres un rayo ⚡");
    else if (nota_final >= 8) Serial.println("Muy buen desempeño. ¡Sigue así! 👍");
    else if (nota_final >= 6) Serial.println("Nada mal, pero puedes mejorar.");
    else if (nota_final >= 1) Serial.println("Necesitas más práctica. ¡No te rindas!");
    else Serial.println("Sin datos suficientes o demasiados errores.");

    mostrarNotaFinal(nota_final);
    delay(10000);
    actualizarRanking(nivel_dificultad, nota_final);
  } else {
    Serial.println("⚠️ No se registraron tiempos de reacción. Nota: 0");
    mostrarNotaFinal(0);
    delay(10000);
    actualizarRanking(nivel_dificultad, 0);
  }
}

bool deseaRepetirJuego() {
  Serial.println("🔁 ¿Quieres volver a jugar?");
  Serial.println("Selecciona una nueva dificultad:");
  Serial.println("GPIO 2: Fácil, GPIO 42: Medio, GPIO 38: Difícil");

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Reiniciar juego:");
  display.setCursor(0, 20);
  display.print("Boton 1 = Facil");
  display.setCursor(0, 30);
  display.print("Boton 2 = Medio");
  display.setCursor(0, 40);
  display.print("Boton 3= Dificil");
  display.setCursor(0, 55);
  display.print("Tienes 1m para elegir");
  display.display();

  unsigned long tiempo_inicio = millis();
  while (millis() - tiempo_inicio < 60000) {// Espera 60 segundos para elegir
    if (digitalRead(boton_facil) == LOW) {
      nivel_dificultad = FACIL;
      Serial.println("🔄 Reiniciando en modo FÁCIL...");
      delay(500);
      return true;
    }
    if (digitalRead(boton_medio) == LOW) {
      nivel_dificultad = MEDIO;
      Serial.println("🔄 Reiniciando en modo MEDIO...");
      delay(500);
      return true;
    }
    if (digitalRead(boton_dificil) == LOW) {
      nivel_dificultad = DIFICIL;
      Serial.println("🔄 Reiniciando en modo DIFÍCIL...");
      delay(500);
      return true;
    }
    delay(100);
  }

  Serial.println("⏹️ Tiempo agotado. El juego no se reiniciará.");
  return false;
}


void jugar() {
  aciertos = 0;
  errores = 0;
  tiempo_index = 0;

  //elegirDificultad();
  //configurarDificultad(nivel_dificultad);

  Serial.println("Cuenta atrás...");
  for (int i = 5; i >= 0; i--) {
    mostrarCuentaAtras(i);
    digitalWrite(buzzer_pin, HIGH);
    delay(300);
    digitalWrite(buzzer_pin, LOW);
    delay(700);
  }

  mostrarMensajeInicio();
  delay(1000);
  display.clearDisplay();
  display.display();

  Serial.println("¡Comienza el juego de reflejos por 30 segundos!");

  juego_activo = true;
  start_time = millis();
  delay(500);
  nuevoTurno();

  while (juego_activo) {
    if (millis() - start_time >= game_duration) {
      finalizarJuego();
      break;
    }

    for (int i = 0; i < num_targets; i++) {
      bool current_state = digitalRead(sensor_pins[i]);

      if (previous_states[i] == HIGH && current_state == LOW) {
        if (i == current_target) {
          unsigned long reaction_time = millis() - led_on_time;
          Serial.print("✅ ¡Correcto! Sensor "); Serial.print(i + 1);
          Serial.print(" | Tiempo: "); Serial.print(reaction_time); Serial.println(" ms");

          if (tiempo_index < max_intentos) tiempos[tiempo_index++] = reaction_time;

          evaluarTiempo(reaction_time);
          aciertos++;

          digitalWrite(led_pins[i], LOW);
          tone(buzzer_pin, 1000, 150);
          delay(500);
          nuevoTurno();
        } else if (current_target != -1) {
          Serial.print("❌ Sensor incorrecto: "); Serial.println(i + 1);
          errores++;
          tone(buzzer_pin, 200, 500);
        } else {
          Serial.print("⚠️ Tocaste antes de tiempo en el sensor "); Serial.println(i + 1);
          errores++;
          tone(buzzer_pin, 200, 500);
        }
      }
      previous_states[i] = current_state;
    }
    delay(20);
  }
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(1));

  if (!LittleFS.begin(true)) {
  Serial.println("Error al montar LittleFS");
  while (true);
  }


  for (int i = 0; i < num_targets; i++) {
    pinMode(sensor_pins[i], INPUT);
    pinMode(led_pins[i], OUTPUT);
    digitalWrite(led_pins[i], LOW);
  }

  pinMode(buzzer_pin, OUTPUT);
  pinMode(boton_facil, INPUT_PULLUP);
  pinMode(boton_medio, INPUT_PULLUP);
  pinMode(boton_dificil, INPUT_PULLUP);

  Wire.begin(8, 9);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("No se pudo encontrar la pantalla OLED"));
    while (true);
  }
  display.clearDisplay();
  display.display();

  cargarRanking(); // Lee los datos desde el archivo
  configurarServidorWeb(); // Lanza el servidor

}

void cargarRanking() {
  File file = LittleFS.open(archivo_ranking, "r");
  if (!file) {
    Serial.println("📂 No existe ranking, se creará uno nuevo.");
    return;
  }

  Nivel nivel;
  String linea;
  while (file.available()) {
    linea = file.readStringUntil('\n');
    linea.trim();
    if (linea == "FACIL") nivel = FACIL;
    else if (linea == "MEDIO") nivel = MEDIO;
    else if (linea == "DIFICIL") nivel = DIFICIL;
    else {
      int puntuacion = linea.toInt();
      if (nivel == FACIL) ranking_facil.push_back(puntuacion);
      else if (nivel == MEDIO) ranking_medio.push_back(puntuacion);
      else if (nivel == DIFICIL) ranking_dificil.push_back(puntuacion);
    }
  }
  file.close();
}


void guardarRanking() {
  File file = LittleFS.open(archivo_ranking, "w");
  if (!file) {
    Serial.println("❌ No se pudo abrir ranking.txt para escribir.");
    return;
  }

  file.println("FACIL");
  for (int nota : ranking_facil) file.println(nota);

  file.println("MEDIO");
  for (int nota : ranking_medio) file.println(nota);

  file.println("DIFICIL");
  for (int nota : ranking_dificil) file.println(nota);

  file.close();
}

void actualizarRanking(Nivel nivel, int nueva_nota) {
  std::vector<int>* r = nullptr;

  if (nivel == FACIL) r = &ranking_facil;
  else if (nivel == MEDIO) r = &ranking_medio;
  else if (nivel == DIFICIL) r = &ranking_dificil;

  if (r) {
    r->push_back(nueva_nota);
    std::sort(r->begin(), r->end(), std::greater<int>());
    if (r->size() > max_ranking) r->resize(max_ranking);
    guardarRanking();
  }
}


void loop() {
  elegirDificultad();
  configurarDificultad(nivel_dificultad);
  jugar();

  if (!deseaRepetirJuego()) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(35, 25);
    display.print(" FIN ");
    display.display();
    while (true); // Se queda parado aquí
  }
}

