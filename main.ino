// --- Definição dos Pinos ---
const int pinosBraille[6] = {2, 3, 4, 5, 6, 7};
const int PINO_CONFIRMACAO = 8;

// Estrutura para mapear o padrão de bits para uma letra
struct BrailleMapping {
  byte pattern;
  char letter;
};

// Tabela com os padrões 

/*Ponto 1 (Canto superior esquerdo) = Bit 0 (0b000001)

Ponto 2 (Canto médio esquerdo) = Bit 1 (0b000010)

Ponto 3 (Canto inferior esquerdo) = Bit 2 (0b000100)

Ponto 4 (Canto superior direito) = Bit 3 (0b001000)

Ponto 5 (Canto médio direito) = Bit 4 (0b010000)

Ponto 6 (Canto inferior direito) = Bit 5 (0b100000)*/

const BrailleMapping brailleTable[] = {
  {0b000001, 'a'}, {0b000011, 'b'}, {0b001001, 'c'}, {0b011001, 'd'},
  {0b010001, 'e'}, {0b001011, 'f'}, {0b011011, 'g'}, {0b010011, 'h'},
  {0b001010, 'i'}, {0b011010, 'j'}, {0b000101, 'k'}, {0b000111, 'l'},
  {0b001101, 'm'}, {0b011101, 'n'}, {0b010101, 'o'}, {0b001111, 'p'},
  {0b011111, 'q'}, {0b010111, 'r'}, {0b001110, 's'}, {0b011110, 't'},
  {0b100101, 'u'}, {0b100111, 'v'}, {0b111010, 'w'}, {0b101101, 'x'},
  {0b111101, 'y'}, {0b110101, 'z'}
};

void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i < 6; i++) {
    pinMode(pinosBraille[i], INPUT_PULLUP);
  }
  
  // Configura o botão de confirmação
  pinMode(PINO_CONFIRMACAO, INPUT_PULLUP);
  
  Serial.println("Sistema pronto. Pressione os pontos Braille e depois o botao de confirmacao.");
}

void loop() {
  // Lógica para detectar o clique do botão de confirmação
  static bool ultimoEstadoBtn = HIGH; 
  bool estadoAtualBtn = digitalRead(PINO_CONFIRMACAO);

  // Detecta a transição de solto (HIGH) para pressionado (LOW)
  if (estadoAtualBtn == LOW && ultimoEstadoBtn == HIGH) {
    
    //  pressionou o botão, vamos ler os pontos agora!
    byte padrao = lerPadraoBraille(pinosBraille);
    char letra = getLetterFromPattern(padrao);
    
    if (letra != '?') {
      Serial.print("Letra enviada: ");
      Serial.println(letra);
      // AQUI é onde vou colocar o comando para o DFPlayer tocar a letra depois
      // Ex: myDFPlayer.play(letra);
    } else {
      Serial.println("Padrao invalido ou nenhum ponto pressionado.");
    }
    
    delay(50); // Debounce simples para o botão
  }
  
  ultimoEstadoBtn = estadoAtualBtn;
}
// Função modular que identifica a letra
char getLetterFromPattern(byte pattern) {
  int numEntries = sizeof(brailleTable) / sizeof(brailleTable[0]);
  
  for (int i = 0; i < numEntries; i++) {
    if (brailleTable[i].pattern == pattern) {
      return brailleTable[i].letter;
    }
  }
  return '?'; // Retorna '?' se o padrão não for encontrado
}

//  função de leitura 
byte lerPadraoBraille(const int pinos[]) {
  byte padrao = 0;
  for (int i = 0; i < 6; i++) {
    // Inverte a lógica: LOW (pressionado) vira 1, HIGH (solto) vira 0
    if (digitalRead(pinos[i])) {
      padrao |= (1 << i);
    }
  }
  return padrao;
}