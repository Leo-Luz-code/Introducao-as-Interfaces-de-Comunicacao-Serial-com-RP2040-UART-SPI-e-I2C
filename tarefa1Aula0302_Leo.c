#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "ws2818b.pio.h"
#include "src/ssd1306.h"
#include "src/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ADDRESS 0x3C
#define UART_ID uart0    // Seleciona a UART0.
#define BAUD_RATE 115200 // Define a taxa de transmissão.
#define UART_TX_PIN 0    // Pino GPIO usado para TX.
#define UART_RX_PIN 1    // Pino GPIO usado para RX.
#define GREEN_LED 11     // LED Verde GPIO11.
#define BLUE_LED 12      // LED Azul GPIO12.
#define RED_LED 13       // LED Vermelho GPIO13.

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7

// Configurações dos pinos.
const uint button_A = 5; // Botão B = 6.
const uint button_B = 6; // Botão A = 5.
#define tempo 2500

uint32_t current_time;

// Variáveis globais.
static volatile uint a = 1;
static volatile uint b = 1;
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos).
char apresentar[2];
char caracter;
ssd1306_t ssd; // Inicializa a estrutura do display.
bool cor = true;

// Prototipação da função de interrupção.
static void gpio_irq_handlerA(uint gpio, uint32_t events);

// Definição de pixel RGB.
struct pixel_t
{
    uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

static void gpio_irq_handlerA(uint gpio, uint32_t events);

// Inicializa a máquina PIO para controle da matriz de LEDs.
void npInit(uint pin)
{

    // Cria programa PIO.
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;

    // Toma posse de uma máquina PIO.
    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0)
    {
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
    }

    // Inicia programa na máquina PIO obtida.
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

    // Limpa buffer de pixels.
    for (uint i = 0; i < LED_COUNT; ++i)
    {
        leds[i].R = 0;
        leds[i].G = 0;
        leds[i].B = 0;
    }
}

// Atribui uma cor RGB a um LED.
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b)
{
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

// Limpa o buffer de pixels.
void npClear()
{
    for (uint i = 0; i < LED_COUNT; ++i)
        npSetLED(i, 0, 0, 0);
}

// Escreve os dados do buffer nos LEDs.
void npWrite()
{
    // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
    for (uint i = 0; i < LED_COUNT; ++i)
    {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

// Relaciona a posição na matriz 5x5 com o incremento 0-24.
int getIndex(int x, int y)
{
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0)
    {
        return y * 5 + x; // Linha par (esquerda para direita).
    }
    else
    {
        return y * 5 + (4 - x); // Linha ímpar (direita para esquerda).
    }
}

int main()
{
    stdio_init_all();

    while (true)
    {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
