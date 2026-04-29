# Assistente de Alfabetização em Braille (MVP)

> **Projeto Final** | *Engenharia de reabilitação e Tecnologia Assistiva*

Este repositório contém o código-fonte e as diretrizes de hardware para o MVP (Mínimo Produto Viável) de um Assistente de Alfabetização em Braille. 

O dispositivo é um brinquedo educativo, interativo e de baixo custo, projetado para oferecer autonomia a crianças com deficiência visual. Ele simula uma "cela Braille" ampliada, onde a criança pode explorar o alfabeto de forma tátil e sonora, além de participar de desafios interativos para a formação de letras.

---

## 🎯 LÓGICA DO DISPOSITIVO

### 1. Ligando o aparelho
Ao ser ligado, o dispositivo recepciona a criança com um áudio animado: *"Olá! Vamos aprender Braille?"*. Imediatamente, o microcontrolador verifica o estado da chave seletora para carregar as regras do jogo atual.

### 2. Modo Livre (Exploração)
Neste modo, a criança tem total autonomia para descobrir o alfabeto de forma orgânica.
* **Ação:** A criança abaixa as chaves retentivas que desejar, formando uma combinação Braille.
* **Validação:** O sistema não avalia a matriz em tempo real. A criança pode abaixar e levantar os pinos livremente. O aparelho só realiza a leitura da combinação no exato momento em que o **Botão de Confirmação** for pressionado.
* **Feedback Sonoro:** O dispositivo lê a combinação e toca o áudio correspondente (Ex: *"Letra A!"*). Se a criança inserir um padrão que não existe no alfabeto, o aparelho reproduz um som: *"Essa combinação não forma uma letra, tente outra vez!"*. O ciclo então se repete.

### 3. Modo Desafio (O Jogo)
Aqui, o aparelho assume o papel de um professor interativo.
* **Algoritmo de Sorteio:** O sistema embaralha e sorteia as 26 letras de forma aleatória. O sistema tem que garantir que não haja letras repetidas até que a criança conclua todo o ciclo do alfabeto. Somente após as 26 letras, um novo sorteio é gerado.
* **A Dinâmica:**
  1. O aparelho solicita uma letra (Ex: *"Faça a letra D!"*) e entra em estado de espera.
  2. A criança tateia, pensa e arruma os pinos como achar melhor, sem ser interrompida.
  3. Ao finalizar, ela aperta o Botão de Confirmação (entregando a prova).
  4. **Se acertar:** O aparelho toca um som de comemoração, sorteia e já pede a próxima letra.
  5. **Se errar:** O aparelho toca um som de incentivo (*"Ops, tente de novo!"*) e continua aguardando a tentativa correta para a mesma letra. Ele não avança até que a criança acerte.

---

## ⚠️ PONTOS IMPORTANTES

Para garantir a melhor experiência de usuário, o código-fonte deve pensar em duas diretrizes fundamentais:

* **1. Se o usuário apertar o botão várias vezes:**
Para evitar que a criança atropele os áudios apertando o botão várias vezes, o sistema deve ficar temporariamente "surdo" durante as falas. Se o alto-falante estiver reproduzindo qualquer instrução ou feedback, cliques no botão de confirmação **devem ser ignorados**.
*(Nota: Utilize a leitura digital do pino `BUSY` do módulo DFPlayer Mini. Ele opera em lógica invertida: Leitura `LOW` = áudio tocando, bloqueie o botão; Leitura `HIGH` = áudio finalizado, libere o clique).*

* **2. Interrupção Imediata (Prioridade da Chave de Modo):**
A detecção de mudança na chave de modos tem prioridade máxima no sistema. Se a criança mudar do "Modo Desafio" para o "Modo Livre" enquanto o alto-falante ainda estiver ditando um desafio, o sistema deve interromper a reprodução do áudio atual imediatamente e realizar a transição de modo.

* **3. OUTRAS COISAS QUE NÃO ESTOU LEMBRANDO AGORA**
---

## 🛠️ Hardware Necessário 

* **1x** Microcontrolador Arduino Nano
* **1x** Módulo MP3-TF-16P V3.0
* **1x** Alto-falante de 8 Ohms (Potência máxima 3W)
* **6x** Chaves Retentivas DPDT (Representando os 6 pontos da cela Braille)
* **1x** Botão Push-button Momentâneo (Botão de Confirmação)
* **1x** Chave Alavanca / Seletora de 2 posições (Modo Livre / Modo Desafio)
* **1x** Resistor de 1kΩ (Proteção da linha RX do módulo MP3)
* **1x** Cartão MicroSD (Formatado em FAT32)
* *Bateria de 3,7V.*
* * Módulo carregador de Bateria TP4056.*
* * Regulador de Tensão Step-up MT3608.*

---

## 💻 Diagrama Lógico de Pinos e Lógica Invertida

O projeto utiliza os resistores internos do Arduino (`INPUT_PULLUP`) para a leitura de todas as chaves mecânicas, simplificando a fiação e reduzindo o custo do hardware.

* **Lógica Ativa em Zero:** * Botão pressionado/abaixado = `LOW` (0)
    * Botão solto/em pé = `HIGH` (1)

**Leitura dos pontos braille:** O Arduino monitora o botão de confirmação, a matriz de 6 pontos Braille só é lida e processada no milissegundo em que a criança pressiona o botão de confirmação.

---

## ⚙️ Configuração do Ambiente e Instalação

### 1. Bibliotecas
Para usar os recursos do módulo MP3, instale a biblioteca abaixo:
* `DFRobotDFPlayerMini` (Para comunicação simples com o módulo de áudio).

### 🎵 Tabela Mestra de Áudios

**Atenção:** Todos os arquivos abaixo devem estar obrigatoriamente dentro de uma pasta chamada `mp3` na raiz do cartão SD, e devem ser nomeados exatamente com estes 4 dígitos iniciais.

| ID (Arquivo) | Descrição do Áudio | Categoria |
| :--- | :--- | :--- |
| `0001` a `0026` | Letras A até Z | Alfabeto |
| `0027` | "Olá, vamos começar a aprender?" | Sistema (Boas-vindas) |
| `0028` | "Modo livre selecionado" | Sistema (Feedback) |
| `0029` | "Modo desafio selecionado" | Sistema (Feedback) |
| `0030` | "Correto! Muito bem!" | Feedback Positivo |
| `0031` | "Ops, tente novamente" | Feedback Negativo |

### ⚡ Diagrama Elétrico
<img width="2856" height="2433" alt="assistente_bb" src="https://github.com/user-attachments/assets/7c5fd359-34e9-4ab2-aea9-8898c540f2db" />

