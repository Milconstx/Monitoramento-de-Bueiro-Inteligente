#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Definição dos pinos do sensor
#define pinTrigger D6
#define pinEcho D5

// Altura do bueiro em cm
const float alturaBueiro = 30.0;

// Credenciais Wi-Fi
const char* ssid = "Bueiro";
const char* password = "12345678";

// Cria servidor na porta 80
ESP8266WebServer server(80);

// Variáveis
float distancia;
float nivelLixo;

void setup() {
  Serial.begin(115200);

  pinMode(pinTrigger, OUTPUT);
  pinMode(pinEcho, INPUT);
  digitalWrite(pinTrigger, LOW);

  // Conecta no Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado! IP: ");
  Serial.println(WiFi.localIP());

  // Define a rota principal "/"
  server.on("/", []() {
    medirLixo(); // Atualiza o nível de lixo

    // Define classe de cor conforme o nível de lixo
    String statusClass;
    if (nivelLixo < alturaBueiro * 0.5) {
      statusClass = "status-green";   // até 50% cheio
    } else if (nivelLixo < alturaBueiro * 0.8) {
      statusClass = "status-orange";  // entre 50% e 80%
    } else {
      statusClass = "status-red";     // acima de 80%
    }

    // HTML estilizado
    String html = "<!DOCTYPE html><html lang='pt-BR'><head>"
                  "<meta charset='UTF-8'>"
                  "<meta http-equiv='refresh' content='10'>"
                  "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                  "<title>MONITORAMENTO DO BUEIRO</title>"
                  "<style>"
                  "body {background-color:#f5f2f4;font-family:Arial,sans-serif;color:#024b0e;padding:20px;text-align:center;}"
                  ".navbar {background-color:#03490c;color:white;padding:15px 0;margin-bottom:20px;font-size:1.5em;font-weight:bold;}"
                  ".data-container {border:2px solid #ccc;border-radius:10px;margin:20px auto;padding:15px;max-width:450px;background-color:white;box-shadow:0 4px 8px rgba(0,0,0,0.1);}"
                  ".data-item {margin:10px 0;}"
                  ".data-value {font-size:1.6em;font-weight:bold;padding:5px;border-radius:5px;display:inline-block;min-width:100px;}"
                  ".status-green {background-color:#ccffcc;color:green;}"
                  ".status-orange {background-color:#ffcc99;color:orange;}"
                  ".status-red {background-color:#ff9999;color:red;}"
                  "</style></head><body>"
                  "<div class='navbar'>MONITORAMENTO INTELIGENTE DO BUEIRO</div>"
                  "<h1>Status Atual do Bueiro</h1>"
                  "<div class='data-container'><hr>"
                  "<div class='data-item'><h2>Espaço Restante:</h2>"
                  "<p class='data-value " + statusClass + "'>" + String(distancia, 1) + " cm</p></div>"
                  "<div class='data-item'><h2>Nível de Residuos no Bueiro:</h2>"
                  "<p class='data-value " + statusClass + "'>" + String(nivelLixo, 1) + " cm</p></div>"
                  "</div></body></html>";

    server.send(200, "text/html", html);
  });

  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  server.handleClient(); // Mantém o servidor rodando
}

// Função que mede o lixo
void medirLixo() {
  digitalWrite(pinTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrigger, LOW);

  long duracao = pulseIn(pinEcho, HIGH);
  distancia = (duracao * 0.0343) / 2;

  nivelLixo = alturaBueiro - distancia;
  if (nivelLixo < 0) nivelLixo = 0;
  if (nivelLixo > alturaBueiro) nivelLixo = alturaBueiro;

  Serial.print("Distância: ");
  Serial.print(distancia);
  Serial.print(" cm | Nível de lixo: ");
  Serial.print(nivelLixo);
  Serial.println(" cm");
}
