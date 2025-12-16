## GitHub + ESP32 / ESP8266 (Código Único – MultiPlacas)

Arquitetura simples, leve e segura usando **GitHub como backend OTA**, compatível com **ESP8266 e ESP32** com **um único código-fonte**.

Repositório:
https://github.com/Nerd-Programador/Firmware-IoT

---

## 1. Configuração do GitHub

### 1.1 Estrutura de pastas (obrigatória)

```text
/ota
 ├── cozinha/
 │   ├── esp8266.bin
 │   ├── esp32.bin
 │   └── version.json
 │
 ├── sala/
 │   ├── esp8266.bin
 │   ├── esp32.bin
 │   └── version.json
```

Cada pasta representa um **ambiente**.

---

### 1.2 Arquivo version.json

Exemplo: `/ota/cozinha/version.json`

```json
{
  "esp8266": "1.0.4",
  "esp32": "2.1.0"
}
```

Regras:
- Formato `MAJOR.MINOR.PATCH`
- JSON pequeno
- Atualizar apenas quando houver firmware novo

---

### 1.3 Firmware (.bin)

- Compile normalmente
- Renomeie para:
  - `esp8266.bin`
  - `esp32.bin`
- Envie para a pasta do ambiente correto

---

### 1.4 URLs usadas pelas placas

```text
https://raw.githubusercontent.com/Nerd-Programador/Firmware-IoT/main/ota/AMBIENTE/version.json
https://raw.githubusercontent.com/Nerd-Programador/Firmware-IoT/main/ota/AMBIENTE/PLACA.bin
```

---

## 2. OTA MultiPlacas – Código Único

### 2.1 Definições fixas

```cpp
#define AMBIENTE   "cozinha"
#define FW_VERSION "1.0.3"
```

---

### 2.2 Pré-processamento de plataforma

```cpp
#if defined(ESP8266)
  #define PLACA "esp8266"
#elif defined(ESP32)
  #define PLACA "esp32"
#else
  #error "Plataforma não suportada"
#endif
```

---

### 2.3 Bibliotecas

```cpp
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <ESP8266httpUpdate.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <HTTPClient.h>
  #include <HTTPUpdate.h>
#endif

#include <WiFiClientSecure.h>
```

---

### 2.4 Controle de tempo (millis)

```cpp
unsigned long lastOTACheck = 0;
const unsigned long OTA_INTERVAL = 6UL * 60UL * 60UL * 1000UL;
```

---

### 2.5 Comparação de versão

```cpp
int parseVersion(const String &v) {
  int a = 0, b = 0, c = 0;
  sscanf(v.c_str(), "%d.%d.%d", &a, &b, &c);
  return a * 10000 + b * 100 + c;
}
```

---

### 2.6 URLs dinâmicas

```cpp
String baseURL = "https://raw.githubusercontent.com/Nerd-Programador/Firmware-IoT/main/ota/";
String versionURL  = baseURL + AMBIENTE + "/version.json";
String firmwareURL = baseURL + AMBIENTE + "/" + PLACA + ".bin";
```

---

### 2.7 Função OTA (ESP8266 e ESP32)

```cpp
void checkOTA() {
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  if (!https.begin(client, versionURL)) return;

  if (https.GET() == 200) {
    String payload = https.getString();

    int idx = payload.indexOf(PLACA);
    if (idx > 0) {
      String v = payload.substring(
        payload.indexOf(":", idx) + 2,
        payload.indexOf("\\"", idx + 10)
      );

      if (parseVersion(v) > parseVersion(FW_VERSION)) {
#if defined(ESP8266)
        ESPhttpUpdate.update(client, firmwareURL);
#elif defined(ESP32)
        httpUpdate.update(client, firmwareURL);
#endif
      }
    }
  }
  https.end();
}
```

---

### 2.8 Chamada no loop

```cpp
void loop() {
  if (millis() - lastOTACheck >= OTA_INTERVAL) {
    lastOTACheck = millis();
    checkOTA();
  }
}
```

---

## 3. Boas práticas

- Não executar OTA no boot
- Não usar delay
- Sempre comparar versões
- Um ambiente por pasta

---

## 4. Resultado

- Código único
- OTA automático por versão
- Funciona dentro e fora da rede
- Compatível com ESP8266 e ESP32
"""

path = "/mnt/data/README.md"
with open(path, "w", encoding="utf-8") as f:
    f.write(md_content)

path