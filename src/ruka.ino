#include <Servo.h>

#define BAUD_RATE 115200
#define INPUT_PIN A0
#define INDICADOR_PIN 3
#define THRESHOLD_VALUE 15


#define TEMPO_FECHAR 1200  // Tempo exato para fechar mão considerando o estresse máximo no fio do dedo indicador.
#define TEMPO_ABRIR  1200  

int estado = 0; // 0 = Mão Fechada, 1 = Mão Aberta (Começa aberta) Vale notar que ele sempre começa invertido, ou seja, caso o estado esteja como 1, ele iniciará como 0

Servo myServo;

void setup() {
  Serial.begin(BAUD_RATE);
  myServo.attach(INDICADOR_PIN);
  myServo.write(90);
  
}

void loop() {
  static unsigned long servoStartTime = 0;
  static bool servoRunning = false;
  static unsigned long tempoAtualMovimento = 0;

  // Leitura e Filtro
  float sensor_value = analogRead(INPUT_PIN);
  float signal = Filter(sensor_value);

  // Serial Plotter
  Serial.print(sensor_value);
  Serial.print(",");
  Serial.print(signal);
  Serial.print(",");
  Serial.println(estado * 10); // Visualização do estado para realizar a calibragem do sensor

  // Lógica de Movimento
  if (signal > THRESHOLD_VALUE && !servoRunning) {
    
    // SE ESTIVER ABERTA -> VAI FECHAR
    if (estado == 1){
      myServo.write(0);           // Maior velocidade possível para a rotação no sentido antihorário
      servoStartTime = millis();
      servoRunning = true;
      tempoAtualMovimento = TEMPO_FECHAR; // Usa o primeiro valor
      estado = 0;                  // Muda o estado para fechada
    }
    // SE ESTIVER FECHADA -> VAI ABRIR (RESET)
    else if (estado == 0){
      myServo.write(180);          // Velocidade de abertura (Volta) Maior velocidade possível para rotação no sentido horário
      servoStartTime = millis();
      servoRunning = true;
      tempoAtualMovimento = TEMPO_ABRIR;  // Existem dois valores diferentes caso seja necessário realizar algum tipo de ajuste específico.
      estado = 1;                  // Muda o estado para aberto
    }
  }

  // Parada baseada no tempo definido para O MOVIMENTO ATUAL
  if (servoRunning && millis() - servoStartTime >= tempoAtualMovimento) {
    myServo.write(90);           // Para o servo
    servoRunning = false;
  }
}

// Filtro digital (código fornecido pela fabricante)
float Filter(float input) { 
    float output = input;
    {
        static float z1, z2;
        float x = output - (-0.73945727*z1 )- (0.59923508*z2);
        output = 0.00223489*x + (0.00446978*z1 )+ (0.00223489*z2);
        z2 = z1;
        z1 = x;
    }
    {
        static float z1, z2;
        float x = output - (-1.03789224*z1 )- (0.64082390*z2);
        output = 1.00000000*x + (2.00000000*z1 )+ (1.00000000*z2);
        z2 = z1;
        z1 = x;
    }
    {
        static float z1, z2;
        float x = output - (-0.59186255*z1 )- (0.80647974*z2);
        output = 1.00000000*x + (-2.00000000*z1 )+ (1.00000000*z2);
        z2 = z1;
        z1 = x;
    }
    {
        static float z1, z2;
        float x = output - (-1.33318587*z1 )- (0.85392964*z2);
        output = 1.00000000*x + (-2.00000000*z1 )+ (1.00000000*z2);
        z2 = z1;
        z1 = x;
    }
    return output;
}