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
#include "src/numeros.h"

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
char show[2];                           // Array para armazenar os caracteres de apresentação no display
char character;                         // Caracter lido
ssd1306_t ssd;                          // Inicializa a estrutura do display.
bool color = true;

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

// Controla a matriz de leds.
void led_matrix(int contador) // Recebe um valor numerico.
{
    npClear();
    switch (contador)
    {
    case 0:
        for (int linha = 0; linha < 5; linha++) // Percorre as linhas.
        {
            for (int coluna = 0; coluna < 5; coluna++) // Percorre as colunas.
            {
                int posicao = getIndex(linha, coluna);                                                                 // Recebe a posição do led.
                npSetLED(posicao, numero_0[coluna][linha][0], numero_0[coluna][linha][1], numero_0[coluna][linha][2]); // Ativa os led de acordo com a cor especificada.
            }
        }
        npWrite();
        break;
    case 1:
        for (int linha = 0; linha < 5; linha++)
        {
            for (int coluna = 0; coluna < 5; coluna++)
            {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero_1[coluna][linha][0], numero_1[coluna][linha][1], numero_1[coluna][linha][2]);
            }
        }
        npWrite();
        break;
    case 2:
        for (int linha = 0; linha < 5; linha++)
        {
            for (int coluna = 0; coluna < 5; coluna++)
            {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero_2[coluna][linha][0], numero_2[coluna][linha][1], numero_2[coluna][linha][2]);
            }
        }
        npWrite();
        break;
    case 3:
        for (int linha = 0; linha < 5; linha++)
        {
            for (int coluna = 0; coluna < 5; coluna++)
            {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero_3[coluna][linha][0], numero_3[coluna][linha][1], numero_3[coluna][linha][2]);
            }
        }
        npWrite();
        break;
    case 4:
        for (int linha = 0; linha < 5; linha++)
        {
            for (int coluna = 0; coluna < 5; coluna++)
            {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero_4[coluna][linha][0], numero_4[coluna][linha][1], numero_4[coluna][linha][2]);
            }
        }
        npWrite();
        break;
    case 5:
        for (int linha = 0; linha < 5; linha++)
        {
            for (int coluna = 0; coluna < 5; coluna++)
            {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero_5[coluna][linha][0], numero_5[coluna][linha][1], numero_5[coluna][linha][2]);
            }
        }
        npWrite();
        break;
    case 6:
        for (int linha = 0; linha < 5; linha++)
        {
            for (int coluna = 0; coluna < 5; coluna++)
            {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero_6[coluna][linha][0], numero_6[coluna][linha][1], numero_6[coluna][linha][2]);
            }
        }
        npWrite();
        break;
    case 7:
        for (int linha = 0; linha < 5; linha++)
        {
            for (int coluna = 0; coluna < 5; coluna++)
            {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero_7[coluna][linha][0], numero_7[coluna][linha][1], numero_7[coluna][linha][2]);
            }
        }
        npWrite();
        break;
    case 8:
        for (int linha = 0; linha < 5; linha++)
        {
            for (int coluna = 0; coluna < 5; coluna++)
            {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero_8[coluna][linha][0], numero_8[coluna][linha][1], numero_8[coluna][linha][2]);
            }
        }
        npWrite();
        break;
    case 9:
        for (int linha = 0; linha < 5; linha++)
        {
            for (int coluna = 0; coluna < 5; coluna++)
            {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero_9[coluna][linha][0], numero_9[coluna][linha][1], numero_9[coluna][linha][2]);
            }
        }
        npWrite();
        break;
    }
}

// Função de inicialização de todos os periféricos
void init_all_peripherals()
{
    // Inicializa os periféricos para uso de funções c padrão.
    stdio_init_all();

    // Inicializa a UART.
    uart_init(UART_ID, BAUD_RATE);

    // Inicializa a I2C com 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    // Configura os pinos GPIO para a UART.
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); // Configura o pino 0 para TX
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); // Configura o pino 1 para RX

    // Mensagem inicial.
    const char *init_message = "UART Demo - RP2\r\n"
                               "Digite algo e veja o eco:\r\n";
    uart_puts(UART_ID, init_message);

    // Inicializa os leds.
    gpio_init(GREEN_LED);
    gpio_set_dir(GREEN_LED, GPIO_OUT);
    gpio_init(BLUE_LED);
    gpio_set_dir(BLUE_LED, GPIO_OUT);
    gpio_init(RED_LED);
    gpio_set_dir(RED_LED, GPIO_OUT);

    // Inicializa os botôes.
    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN); // Configura o pino como entrada.
    gpio_pull_up(button_A);          // Habilita o pull-up interno.
    gpio_init(button_B);
    gpio_set_dir(button_B, GPIO_IN); // Configura o pino como entrada.
    gpio_pull_up(button_B);          // Habilita o pull-up interno.

    // Inicializa matriz de LEDs.
    npInit(LED_PIN);
    npClear();
    npWrite(); // Escreve os dados nos LEDs.

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Configura o pino GPIO para I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Configura o pino GPIO para I2C
    gpio_pull_up(I2C_SDA);                     // Habilita o pull up para os dados
    gpio_pull_up(I2C_SCL);                     // Habilita o pull up para o clock

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADDRESS, I2C_PORT); // Inicializa o display.
    ssd1306_config(&ssd);                                        // Configura o display.
    ssd1306_send_data(&ssd);                                     // Envia os dados para o display.

    // Função de interrupção.
    gpio_set_irq_enabled_with_callback(button_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handlerA);
    gpio_set_irq_enabled(button_B, GPIO_IRQ_EDGE_FALL, true);
}

int main()
{
    init_all_peripherals();

    // Loop principal
    while (true)
    {
        color = !color;

        // Lê o caracter inserido.
        if (uart_is_readable(UART_ID))
        {
            // Lê um caractere da UART.
            character = uart_getc(UART_ID);
            show[0] = character; // Guarda o caracter lido.
            show[1] = '\0';

            // Envia de volta o caractere lido (eco).
            uart_putc(UART_ID, character);

            // Envia uma mensagem adicional para cada caractere recebido.
            uart_puts(UART_ID, " <- Eco do RP2\r\n");
        }

        // Compara para identificar se foi inserido um caraster numérico.
        if (character >= '0' && character <= '9')
        {
            led_matrix(character - '0'); // Converte o caracter em um inteiro e aciona a matriz de led.
        }

        // Atualiza o conteúdo do display com animações.
        ssd1306_fill(&ssd, !color);                       // Limpa o display.
        ssd1306_rect(&ssd, 3, 3, 122, 58, color, !color); // Desenha um retângulo.
        ssd1306_draw_string(&ssd, show, 60, 30);          // Desenha uma string.
        ssd1306_send_data(&ssd);                          // Atualiza o display.
        sleep_ms(1000);
    }
}

// Função de interrupção com debouncing.
void gpio_irq_handlerA(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos.
    current_time = to_us_since_boot(get_absolute_time());

    // Verifica se passou tempo suficiente desde o último evento.
    // Diferencia o botão A do B.
    if (gpio == button_A && (current_time - last_time > 200000)) // 200 ms de debouncing.
    {
        last_time = current_time; // Atualiza o tempo do último evento.
        printf("Mudanca de Estado do Led. A = %d\n", a);
        gpio_put(GREEN_LED, !gpio_get(GREEN_LED)); // Alterna o estado.
        if (gpio_get(GREEN_LED) == false)          // Reconhece o estado do led.
        {
            uart_puts(UART_ID, " LED Azul desligado\r\n");
            ssd1306_fill(&ssd, !color);                       // Limpa o display.
            ssd1306_rect(&ssd, 3, 3, 122, 58, color, !color); // Desenha um retângulo.
            ssd1306_draw_string(&ssd, "LED Azul off", 17, 30);
            ssd1306_send_data(&ssd); // Atualiza o display.
        }
        else
        {
            uart_puts(UART_ID, " LED Azul ligado\r\n");
            ssd1306_fill(&ssd, !color);                       // Limpa o display.
            ssd1306_rect(&ssd, 3, 3, 122, 58, color, !color); // Desenha um retângulo.
            ssd1306_draw_string(&ssd, "LED Azul on", 17, 30);
            ssd1306_send_data(&ssd); // Atualiza o display.
        }
        a++; // Incrementa a variavel de verificação
    }

    // Diferencia o botão A do B.
    if (gpio == button_B && (current_time - last_time > 200000)) // 200 ms de debouncing.
    {
        last_time = current_time; // Atualiza o tempo do último evento.
        printf("Mudanca de Estado do Led. B = %d\n", b);
        gpio_put(BLUE_LED, !gpio_get(BLUE_LED)); // Alterna o estado.
        if (gpio_get(BLUE_LED) == false)         // Reconhece o estado do led.
        {
            uart_puts(UART_ID, " LED Verde desligado\r\n");
            ssd1306_fill(&ssd, !color);                       // Limpa o display.
            ssd1306_rect(&ssd, 3, 3, 122, 58, color, !color); // Desenha um retângulo.
            ssd1306_draw_string(&ssd, "LED Verde off", 14, 30);
            ssd1306_send_data(&ssd); // Atualiza o display.
        }
        else
        {
            uart_puts(UART_ID, " LED Verde ligado\r\n");
            ssd1306_fill(&ssd, !color);                       // Limpa o display.
            ssd1306_rect(&ssd, 3, 3, 122, 58, color, !color); // Desenha um retângulo.
            ssd1306_draw_string(&ssd, "LED Verde on", 14, 30);
            ssd1306_send_data(&ssd); // Atualiza o display.
        }
        b++; // Incrementa a variavel de verificação.
    }
}
