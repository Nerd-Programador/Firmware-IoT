/*************************************************
 *  OTA Automático por Versão
 *  ESP8266 + ESP32 (Código Único)
 *  Verificação: 1x no boot
 *************************************************/

// ========= CONFIGURAÇÕES =========
#define WIFI_SSID     "SEU_WIFI"
#define WIFI_PASSWORD "SUA_SENHA"

#define AMBIENTE   "cozinha"
#define FW_VERSION "1.0.3"

// =================================

// --------- PRÉ-PROCESSAMENTO ---------
#if defined(ESP8266)
  #define PLACA "esp8266"
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <ESP8266httpUpdate.h>
#elif defined(ESP32)
  #define PLACA "esp32"
  #include <WiFi.h>
  #include <HTTPClient.h>
  #include <HTTPUpdate.h>
#else
  #error "Plataforma não suportada"
#endif

#include <WiFiClientSecure.h>

// --------- URL BASE OTA ---------
const String baseURL =
  "https://raw.githubusercontent.com/Nerd-Programador/Firmware-IoT/main/ota/";

// --------- FUNÇÕES AUXILIARES ---------
int parseVersion(const String &v) {
  int a = 0, b = 0, c = 0;
  sscanf(v.c_str(), "%d.%d.%d", &a, &b, &c);
  return a * 10000 + b * 100 + c;
}

// --------- OTA ---------
void checkOTA() {

  String versionURL  = baseURL + AMBIENTE + "/version.json";
  String firmwareURL = baseURL + AMBIENTE + "/" + PLACA + ".bin";

  WiFiClientSecure client;
  client.setInsecure(); // HTTPS GitHub

  HTTPClient https;
  if (!https.begin(client, versionURL)) {
    Serial.println("OTA: erro ao iniciar HTTPS");
    return;
  }

  int httpCode = https.GET();
  if (httpCode != 200) {
    Serial.printf("OTA: erro HTTP %d\n", httpCode);
    https.end();
    return;
  }

  String payload = https.getString();
  https.end();

  int idx = payload.indexOf(PLACA);
  if (idx < 0) {
    Serial.println("OTA: placa não encontrada no version.json");
    return;
  }

  String remoteVersion = payload.substring(
    payload.indexOf(":", idx) + 2,
    payload.indexOf("\"", idx + 10)
  );

  Serial.printf("OTA: versão local  = %s\n", FW_VERSION);
  Serial.printf("OTA: versão remota = %s\n", remoteVersion.c_str());

  if (parseVersion(remoteVersion) <= parseVersion(FW_VERSION)) {
    Serial.println("OTA: firmware já está atualizado");
    return;
  }

  Serial.println("OTA: nova versão encontrada, iniciando update...");

#if defined(ESP8266)
  ESPhttpUpdate.update(client, firmwareURL);
#elif defined(ESP32)
  httpUpdate.update(client, firmwareURL);
#endif
}

// --------- SETUP ---------
void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("\nBoot iniciado");
  Serial.printf("Placa: %s\n", PLACA);
  Serial.printf("Ambiente: %s\n", AMBIENTE);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Conectando ao WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi não conectado, OTA cancelado");
    return;
  }

  Serial.println("WiFi conectado");
  checkOTA();   // 👈 OTA executa UMA ÚNICA VEZ
}

// --------- LOOP ---------
void loop() {
  // vazio por design
}
