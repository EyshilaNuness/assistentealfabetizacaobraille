#include <Arduino.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// ==========================================
// 1. MAPEAMENTO DE HARDWARE (ESP32-C3)
// ==========================================
const int PIN_P1 = 0;
const int PIN_P2 = 1;
const int PIN_P3 = 3;
const int PIN_P4 = 4;
const int PIN_P5 = 5;
const int PIN_P6 = 6;

const int PIN_MODO = 7;      
const int PIN_CONFIRMA = 21;  
const int PIN_BUSY = 8;      

const int PIN_RX_DF = 2;     
const int PIN_TX_DF = 10;    

// ==========================================
// 2. VARIÁVEIS DO SISTEMA
// ==========================================
HardwareSerial mySerial(1);
DFRobotDFPlayerMini myDFPlayer;

bool modoLivre = true;
bool ultimoEstadoModo = true;
bool ultimoEstadoConfirma = HIGH;

enum EstadoDesafio { INICIAR_SORTEIO, PEDIR_LETRA, ESPERAR_RESPOSTA, FEEDBACK_CORRETO };
EstadoDesafio estadoAtual = INICIAR_SORTEIO;

int alfabeto[26];
int indiceSorteio = 0;
int letraAlvo = 0;
bool dfplayerPronto = false; 

// ==========================================
// 3. DICIONÁRIO BRAILLE (Bit 0=P1, Bit 5=P6)
// ==========================================
const byte dicionarioBraille[26] = {
  B000001, B000011, B001001, B011001, B010001, // A, B, C, D, E
  B001011, B011011, B010011, B001010, B011010, // F, G, H, I, J
  B000101, B000111, B001101, B011101, B010101, // K, L, M, N, O
  B001111, B011111, B010111, B001110, B011110, // P, Q, R, S, T
  B100101, B100111, B111010, B101101, B111101, // U, V, W, X, Y
  B110101                                      // Z
};

// ==========================================
// FUNÇÕES DE ÁUDIO E DIAGNÓSTICO
// ==========================================
void printTextoAudio(int idAudio) {
  Serial.print("[SISTEMA] Solicitando Audio ID ");
  Serial.print(idAudio);
  Serial.print(": ");
  
  if (idAudio >= 1 && idAudio <= 26) {
    Serial.print("\"Letra ");
    Serial.print((char)('A' + idAudio - 1));
    Serial.println("!\"");
  } else {
    switch(idAudio) {
      case 27: Serial.println("\"Olá! Vamos aprender Braille?\""); break;
      case 28: Serial.println("\"Modo Livre Selecionado!\""); break;
      case 29: Serial.println("\"Modo Desafio Selecionado!\""); break;
      case 30: Serial.println("\"Correto! Muito bem!\""); break;
      case 31: Serial.println("\"Ops, tente novamente!\""); break;
      default: Serial.println("\"Som desconhecido\""); break;
    }
  }
}

void executarAudio(int idAudio) {
  printTextoAudio(idAudio);
  
  if (dfplayerPronto) {
    // ATRASO 1: Dá tempo do módulo respirar entre os comandos curtos
    delay(400); 
    myDFPlayer.play(idAudio); 
  }
}

void embaralharAlfabeto() {
  for (int i = 0; i < 26; i++) {
    alfabeto[i] = i + 1;
  }
  for (int i = 25; i > 0; i--) {
    int j = random(0, i + 1);
    int temp = alfabeto[i];
    alfabeto[i] = alfabeto[j];
    alfabeto[j] = temp;
  }
  indiceSorteio = 0;
  Serial.println("\n[LOG] -> Alfabeto de desafios embaralhado!");
}

byte lerEPlotarBraille() {
  byte combinacao = 0;
  
  bool b1 = (digitalRead(PIN_P1) == LOW);
  bool b2 = (digitalRead(PIN_P2) == LOW);
  bool b3 = (digitalRead(PIN_P3) == LOW);
  bool b4 = (digitalRead(PIN_P4) == LOW);
  bool b5 = (digitalRead(PIN_P5) == LOW);
  bool b6 = (digitalRead(PIN_P6) == LOW);

  if (b1) bitSet(combinacao, 0);
  if (b2) bitSet(combinacao, 1);
  if (b3) bitSet(combinacao, 2);
  if (b4) bitSet(combinacao, 3);
  if (b5) bitSet(combinacao, 4);
  if (b6) bitSet(combinacao, 5);

  Serial.println("\n--- MAPA DA CELULA LIDA ---");
  Serial.print("  P1: "); Serial.print(b1 ? "[X] ABAIXADO" : "[ ] SOLTO");
  Serial.print("   |   P4: "); Serial.println(b4 ? "[X] ABAIXADO" : "[ ] SOLTO");
  Serial.print("  P2: "); Serial.print(b2 ? "[X] ABAIXADO" : "[ ] SOLTO");
  Serial.print("   |   P5: "); Serial.println(b5 ? "[X] ABAIXADO" : "[ ] SOLTO");
  Serial.print("  P3: "); Serial.print(b3 ? "[X] ABAIXADO" : "[ ] SOLTO");
  Serial.print("   |   P6: "); Serial.println(b6 ? "[X] ABAIXADO" : "[ ] SOLTO");
  Serial.print("Byte Binario Resultante: B");
  for (int i = 5; i >= 0; i--) {
    Serial.print(bitRead(combinacao, i));
  }
  Serial.println("\n---------------------------");

  return combinacao;
}

int identificarLetra(byte combinacao) {
  for (int i = 0; i < 26; i++) {
    if (dicionarioBraille[i] == combinacao) {
      return i + 1; 
    }
  }
  return 0; 
}

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  
  // ATRASO 2: Espera o computador conectar o Monitor Serial
  delay(3000); 
  
  Serial.println("\n\n=========================================");
  Serial.println("     SISTEMA BRAILLE - PASTA MP3 ATIVA     ");
  Serial.println("=========================================");

  pinMode(PIN_P1, INPUT_PULLUP);
  pinMode(PIN_P2, INPUT_PULLUP);
  pinMode(PIN_P3, INPUT_PULLUP);
  pinMode(PIN_P4, INPUT_PULLUP);
  pinMode(PIN_P5, INPUT_PULLUP);
  pinMode(PIN_P6, INPUT_PULLUP);
  pinMode(PIN_MODO, INPUT_PULLUP);
  pinMode(PIN_CONFIRMA, INPUT_PULLUP);
  pinMode(PIN_BUSY, INPUT_PULLUP); 

  // --- SOLUÇÃO DO "CLIQUE FANTASMA" ---
  // ATRASO 3: Dá um tempo para a energia nos fios estabilizar antes de ler
  delay(200); 
  ultimoEstadoConfirma = digitalRead(PIN_CONFIRMA); // Lê o estado REAL do botão agora
  ultimoEstadoModo = digitalRead(PIN_MODO);         // Lê o estado REAL da chave agora
  modoLivre = (ultimoEstadoModo == HIGH);
  
  Serial.print("[MODO INICIAL] Iniciado no modo: ");
  Serial.println(modoLivre ? "EXPLORACAO (LIVRE)" : "JOGO (DESAFIO)");

  // --- SOLUÇÃO DO DFPLAYER ---
  mySerial.begin(9600, SERIAL_8N1, PIN_RX_DF, PIN_TX_DF);
  
  // ATRASO 4: OBRIGATÓRIO. Dá 2 segundos pro DFPlayer ligar de vez
  delay(2000); 
  
  Serial.println("[HARDWARE] Inicializando comunicação com o DFPlayer...");
  
  // =========================================================================
  // O TRUQUE DO CHIP CLONE: Parâmetros "false, true" forçam a inicialização!
  // =========================================================================
  if (myDFPlayer.begin(mySerial, false, true)) {
    Serial.println("[HARDWARE] -> DFPlayer Mini conectado com sucesso!");
    dfplayerPronto = true;
    
    myDFPlayer.volume(10); // Ajuste do volume
    delay(1000); // ATRASO 5: Pausa essencial para o cartão SD ser lido sem engasgo
    
    executarAudio(27); // Boas-vindas
  } else {
    Serial.println("[HARDWARE] -> Erro: DFPlayer nao respondeu. Verifique fiação e/ou INVERTA RX/TX.");
  }

  randomSeed(analogRead(2)); 
}

// ==========================================
// LOOP PRINCIPAL
// ==========================================
void loop() {
  bool estadoModoAtual = digitalRead(PIN_MODO);
  bool estadoConfirmaAtual = digitalRead(PIN_CONFIRMA);
  
  // Verifica se o som está tocando fisicamente
  bool tocandoAudio = (dfplayerPronto) ? (digitalRead(PIN_BUSY) == LOW) : false;

  // 1. Mudança de Modo (Prioridade Máxima)
  if (estadoModoAtual != ultimoEstadoModo) {
    delay(50); // Debounce
    if (digitalRead(PIN_MODO) == estadoModoAtual) {
      ultimoEstadoModo = estadoModoAtual;
      modoLivre = (estadoModoAtual == HIGH);
      
      Serial.println("\n[CHAVE] Mudança de modo detectada!");
      if (dfplayerPronto) myDFPlayer.stop(); 
      delay(200); // Pausa para o módulo respirar entre comandos

      if (modoLivre) {
        executarAudio(28); 
      } else {
        executarAudio(29); 
        estadoAtual = INICIAR_SORTEIO; 
      }
    }
  }

  // Se o áudio estiver tocando, ignora o botão de confirmação (Fica Surdo)
  if (tocandoAudio) return;

  // Detecção de clique no botão de confirmação
  bool confirmou = false;
  if (estadoConfirmaAtual == LOW && ultimoEstadoConfirma == HIGH) {
    delay(50); // Debounce
    if (digitalRead(PIN_CONFIRMA) == LOW) {
      confirmou = true;
      Serial.println("\n[CLIQUE] Botão Confirmar acionado!");
    }
  }
  ultimoEstadoConfirma = estadoConfirmaAtual;

  // 2. Lógica do Modo Livre
  if (modoLivre) {
    if (confirmou) {
      byte leitura = lerEPlotarBraille();
      int letraIdentificada = identificarLetra(leitura);

      if (letraIdentificada > 0) {
        executarAudio(letraIdentificada); 
      } else {
        executarAudio(31); 
      }
    }
  } 
  
  // 3. Lógica do Modo Desafio
  else {
    switch (estadoAtual) {
      
      case INICIAR_SORTEIO:
        embaralharAlfabeto();
        estadoAtual = PEDIR_LETRA;
        break;

      case PEDIR_LETRA:
        if (indiceSorteio >= 26) {
          Serial.println("\n[DESAFIO] Parabéns! Jogo finalizado com sucesso.");
          estadoAtual = INICIAR_SORTEIO; 
        } else {
          letraAlvo = alfabeto[indiceSorteio];
          Serial.print("\n[DESAFIO] Professor pede a letra: ");
          Serial.println((char)('A' + letraAlvo - 1));
          
          executarAudio(letraAlvo); 
          estadoAtual = ESPERAR_RESPOSTA;
        }
        break;

      case ESPERAR_RESPOSTA:
        if (confirmou) {
          byte leitura = lerEPlotarBraille();
          int letraIdentificada = identificarLetra(leitura);

          if (letraIdentificada == letraAlvo) {
            Serial.println("[RESULTADO] Correto!");
            executarAudio(30); 
            indiceSorteio++; 
            estadoAtual = FEEDBACK_CORRETO; 
          } else {
            Serial.println("[RESULTADO] Errado. Tentando novamente.");
            executarAudio(31); 
          }
        }
        break;

      case FEEDBACK_CORRETO:
        if (!tocandoAudio) {
          estadoAtual = PEDIR_LETRA;
        }
        break;
    }
  }
}
