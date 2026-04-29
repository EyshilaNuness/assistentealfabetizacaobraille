# Assistente de Alfabetização em Braille (MVP)

> **Projeto Final** | *Engenharia de reabilitação e Tecnologia Assistiva*

Este repositório contém o código-fonte e as diretrizes de hardware para o MVP (Mínimo Produto Viável) de um Assistente de Alfabetização em Braille. 

O dispositivo é um brinquedo educativo, interativo e de baixo custo, projetado para oferecer autonomia a crianças com deficiência visual. Ele simula uma "cela Braille" ampliada, onde a criança pode explorar o alfabeto de forma tátil e sonora, além de participar de desafios interativos para a formação de letras.

---

## 🎯 Funcionalidades e Modos de Operação

O sistema possui dois modos principais, selecionados por uma chave física:

* **Modo Livre (Exploração):** A criança tem controle total. Ela abaixa os pinos Braille desejados e aperta o botão de confirmação. O sistema lê a matriz e reproduz em áudio qual letra foi formada (ou avisa se a combinação for inválida).
* **Modo Desafio (O Jogo):** O dispositivo atua como um professor. Ele sorteia uma letra aleatória e pede para a criança montá-la. O sistema aguarda a criança tatear e ajustar os botões. A validação só ocorre quando o botão de confirmação é pressionado, fornecendo feedback positivo (acerto) ou estímulo para tentar novamente (erro).

### 🛡️ Engenharia de Interação (Regra de Ouro)
Para evitar que o usuário atropele os áudios ou cause comportamentos erráticos no sistema apertando botões repetidamente, o projeto utiliza o pino `BUSY` do módulo MP3. **O sistema fica "surdo" para o botão de confirmação enquanto qualquer áudio estiver sendo reproduzido**, garantindo que a criança escute as instruções até o fim.

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

