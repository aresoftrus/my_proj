

#include <WiFi.h>
#include <HTTPClient.h>

// Конфигурация Wi-Fi
const char* ssid = "HUAWEI-B535-B30E";
const char* password = "TH8NL2HF42Q";

// Конфигурация встроенного DAC
const int dacChannel = 25; // Выберите GPIO-пин для DAC

// Конфигурация DAC
const int dacResolution = 12; // 8-битный ЦАП, можно изменить на 12 или 16 бит, если нужно

void setup() {//Serial.begin(115200);
  // Инициализация встроенного DAC
  pinMode(dacChannel, OUTPUT);
  dacWrite(dacChannel, 0);

  // Подключение к Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

void loop() {
  // Создание HTTP-клиента
  HTTPClient http;

  // URL аудио-потока
  String url = "https://shr3.radioca.st/stream";

  // Начало HTTP-запроса
  http.begin(url);

  // Отправка HTTP-запроса и получение ответа
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    WiFiClient *stream = http.getStreamPtr();
    uint8_t buffer[512];
    
    while (stream->available()) {
      int bytesRead = stream->read(buffer, sizeof(buffer));
      if (bytesRead > 0) {
        for (int i = 0; i < bytesRead; i++) {
          // Запись данных на DAC
          dacWrite(dacChannel, buffer[i]<< 4);

          // Если вы используете более высокий разряд ЦАП, масштабируйте данные соответствующим образом
          
          // Например, для 12-битного ЦАП:
          // dacWrite(dacChannel, buffer[i] << 4);

          // Например, для 16-битного ЦАП:
          // dacWrite(dacChannel, buffer[i] << 8);
        }
      }
    }
  }

  // Завершение HTTP-запроса
  http.end();

  delay(1000); // Пауза перед повторной попыткой
}
