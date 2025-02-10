# Projeto: Controle de LEDs e Display com Comunicação Serial no RP2040

## Descrição

Este projeto foi desenvolvido para consolidar conhecimentos sobre o uso de interfaces de comunicação serial em microcontroladores, utilizando a placa de desenvolvimento **BitDogLab** com o microcontrolador **RP2040**. O projeto integra a comunicação via UART e I2C, controle de LEDs comuns e endereçáveis, e a interação com botões utilizando interrupções e debounce.

## Funcionalidades

1. **Modificação da Biblioteca `font.h`**

   - Adição de caracteres minúsculos personalizados.

2. **Entrada de Caracteres via PC**

   - Caracteres digitados no Serial Monitor do VS Code são exibidos no display **SSD1306**.
   - Números de 0 a 9 também são exibidos na matriz 5x5 de LEDs **WS2812**.

3. **Interação com o Botão A (GPIO 5)**

   - Alterna o estado do LED RGB Verde.
   - Exibe mensagem informativa no display **SSD1306**.
   - Envia descrição da operação ao Serial Monitor.

4. **Interação com o Botão B (GPIO 6)**
   - Alterna o estado do LED RGB Azul.
   - Exibe mensagem informativa no display **SSD1306**.
   - Envia descrição da operação ao Serial Monitor.

## Componentes Utilizados

- **Placa BitDogLab com RP2040**
- **Matriz 5x5 de LEDs WS2812** (GPIO 7)
- **LED RGB** (GPIOs 11, 12 e 13)
- **Botão A** (GPIO 5)
- **Botão B** (GPIO 6)
- **Display SSD1306 via I2C** (GPIO 14 e GPIO 15)

## Requisitos do Projeto

- **Uso de Interrupções:** Todas as funcionalidades dos botões implementadas com rotinas de interrupção (IRQ).
- **Debouncing:** Tratamento de bouncing dos botões via software.
- **Controle de LEDs:** Utilização de LEDs comuns e WS2812.
- **Utilização do Display SSD1306:** Exibição de caracteres maiúsculos e minúsculos.
- **Envio de Informações via UART:** Comunicação com o PC usando o Serial Monitor.
- **Organização do Código:** Código bem estruturado e comentado.

## Execução do Projeto

1. Clone o repositório:
   ```bash
   git clone https://github.com/Leo-Luz-code/Introducao-as-Interfaces-de-Comunicacao-Serial-com-RP2040-UART-SPI-e-I2C.git
   ```
2. Compile e envie o código para a placa BitDogLab.
3. Utilize o Serial Monitor do VS Code para interagir com o projeto.

## Demonstração em Vídeo

Confira o funcionamento do projeto no vídeo abaixo:

[![Demonstração do Projeto](https://img.youtube.com/vi/ImYPL_FY4pU/maxresdefault.jpg)](https://www.youtube.com/shorts/ImYPL_FY4pU)

Ou acesse diretamente pelo [link do vídeo](https://www.youtube.com/shorts/ImYPL_FY4pU).

## Estrutura do Repositório

```
├── src/
│   ├── font.h
│   └── numeros.c
│   ├── ssd1306.h
│   └── ssd1306.c
├── tarefa1Aula0302_Leo.c
├── README.md
└── LICENSE
```

## Licença

Este projeto está licenciado sob a Licença MIT. Consulte o arquivo [LICENSE](./LICENSE) para mais informações.

---

Desenvolvido por **Leonardo Luz**.
