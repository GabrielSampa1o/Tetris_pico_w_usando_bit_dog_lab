# 🎮 Tetris na Raspberry Pi Pico W

## 📜 Descrição do Projeto
Este projeto implementa o clássico jogo **Tetris** utilizando uma **Raspberry Pi Pico W**, um **display OLED SSD1306** e um **joystick analógico com botões**. Além disso, o projeto inclui **feedback sonoro com buzzers**, LEDs RGB para indicar status e lógica baseada em **interrupções** para melhor responsividade.

## 🛠️ Componentes Utilizados
- **Raspberry Pi Pico W**
- **Display OLED SSD1306** (conectado via I2C)
- **Joystick Analógico** (com botões)
- **Botões físicos (A e B)**
- **Matriz de LEDs WS2812 (opcional para efeitos visuais)**
- **Buzzers** (para feedback sonoro)
- **LEDs RGB**

## 🔧 Funcionalidades
- Controle do **Tetris** usando **joystick analógico** e **botões físicos**.
- **Movimentos e rotações** das peças usando **interrupções (IRQ) e debounce**.
- Exibição do **estado do jogo** no **display OLED SSD1306**.
- **Feedback sonoro** com buzzers ao mover peças, girá-las ou completar linhas.
- **LEDs RGB indicativos** para mostrar estados do jogo.
- Implementação do **auto-repeat** para botões.

## 🖥️ Estrutura do Código
O projeto é modularizado, contendo os seguintes arquivos:

### 🔹 Código Principal:
- **`projeto_tetris.c`** - Código principal que gerencia o jogo e o hardware.

### 🔹 Módulos de Hardware:
- **`ssd1306.c` / `ssd1306.h`** - Controle do display OLED SSD1306.
- **`auto_repeat.c` / `auto_repeat.h`** - Implementação do auto-repeat para os botões.
- **`buzzer.c` / `buzzer.h`** - Controle dos buzzers para efeitos sonoros.

### 🔹 Lógica do Jogo:
- **`tetris.c` / `tetris.h`** - Implementação do jogo Tetris, incluindo regras, lógica de movimentação e detecção de colisões.
- **`font.h`** - Definição dos caracteres exibidos no display OLED.

## 📌 Configuração do Hardware
| Componente          | Pino na Pico W |
|--------------------|--------------|
| **Joystick VRX**   | GPIO27 (ADC1) |
| **Joystick VRY**   | GPIO26 (ADC0) |
| **Botão do Joystick** | GPIO22 |
| **Botão A**        | GPIO5  |
| **Botão B**        | GPIO6  |
| **LED Vermelho**   | GPIO13 |
| **LED Verde**      | GPIO11 |
| **LED Azul**       | GPIO12 |
| **Buzzer A**       | GPIO21 |
| **Buzzer B**       | GPIO10 |
| **Display SSD1306 (I2C)** | GPIO14 (SDA) / GPIO15 (SCL) |

## 🚀 Como Compilar e Rodar

### 1️⃣ Instale o Raspberry Pi Pico SDK
Siga as instruções oficiais do SDK: [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)

### 2️⃣ Clone este repositório:
```sh
    git clone https://github.com/seu-usuario/tetris-pico-w.git
    cd tetris-pico-w
```

### 3️⃣ Abra o VS Code e importe o projeto:
- Vá até a **Extensão Raspberry Pi Pico**.
- Selecione **Import Project**.
- Escolha a pasta do repositório clonado.
- Clique em **Import**.

### 4️⃣ Compile o código:
- Utilize a opção de **Build** da extensão.

### 5️⃣ Carregue o binário na Pico
1. Pressione e segure o **botão BOOTSEL** da Raspberry Pi Pico W.
2. Conecte-a ao PC via **USB**.
3. Copie o arquivo `.uf2` gerado para a unidade montada.

## 🎮 Controles do Jogo

| Controle  | Função  |
|-----------|---------|
| **Joystick Esquerda**  | Move a peça para a esquerda |
| **Joystick Direita**   | Move a peça para a direita |
| **Joystick Baixo**     | Soft drop (queda rápida) |
| **Joystick Cima**      | Gira a peça no sentido horário |
| **Botão A**            | Rotação anti-horária |
| **Botão B**            | Rotação horária |
| **Botão do Joystick**  | Hard drop (queda instantânea) |

## 📸 Exemplos Visuais

- **Estado inicial do jogo no display OLED** 🟦
- **Peça em movimento e colisão com outra** 🟥
- **LEDs RGB piscando ao completar uma linha** 🌟
- **Buzzer emitindo som de sucesso ao limpar linhas** 🔊

## 📜 Referências
- [Código-fonte original inspirado](https://github.com/rbirkby/picotetris?tab=readme-ov-file)
- [Demonstração em vídeo do conceito](https://www.youtube.com/watch?v=YCEuKoBeN7E)

## 📢 Autor
- **Gabriel Silva Sampaio**

- [Demonstração do proejeto na bitdoglab](https://drive.google.com/file/d/18ZJv9vtnMhlL-CdfEQPN1Qivy5LSCdna/view?usp=sharing)
- 
---
🚀 **Divirta-se jogando Tetris na sua Raspberry Pi Pico W!** 🎮

