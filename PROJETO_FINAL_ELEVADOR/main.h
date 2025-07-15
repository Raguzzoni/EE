#ifndef MAIN_H
#define	MAIN_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <math.h>
#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
#ifdef	__cplusplus
}
#endif /* __cplusplus */


//==============================================================================
// Defini��es de Hardware e Constantes
//==============================================================================

#define MOTOR_PWM       LATAbits.LATA3    /**< @def Define o pino de controle PWM do motor (n�o utilizado diretamente no c�digo, mas reservado). */
#define MOTOR_DIR       LATAbits.LATA7    /**< @def Define o pino de controle de dire��o do motor. 1 para subir, 0 para descer. */
#define MOTOR_ON        CCP3CONbits.CCP3M /**< @def Macro para verificar o estado do motor atrav�s do registrador do m�dulo CCP3. */

#define FLOOR1          0                 /**< @def Define o identificador para o Andar 1. */
#define FLOOR2          1                 /**< @def Define o identificador para o Andar 2. */
#define FLOOR3          2                 /**< @def Define o identificador para o Andar 3. */
#define FLOOR4          3                 /**< @def Define o identificador para o Andar 4. */
#define MAX_INDEX       6                 /**< @def N�mero m�ximo de �ndices na rota do elevador (0-1-2-3-2-1). */
#define MAX_SIZE        10                /**< @def Tamanho m�ximo para buffers gen�ricos. */

//==============================================================================
// Estruturas e Tipos de Dados
//==============================================================================

/**
 * @struct uart_var
 * @brief Estrutura para agrupar as vari�veis de estado do elevador que ser�o transmitidas via UART.
 */
typedef struct {
    unsigned O    : 4;    /**< Andar de Origem da requisi��o. */
    unsigned D    : 4;    /**< Andar de Destino da requisi��o. */
    unsigned A    : 4;    /**< Andar Atual em que o elevador se encontra. */
    unsigned M    : 2;    /**< Estado do Motor (0: Parado, 2: Subindo, 3: Descendo). */
    unsigned HHH  : 8;    /**< Altura (Posi��o) atual do elevador em uma escala arbitr�ria. */
    uint16_t VVV;         /**< Velocidade atual do elevador em uma escala arbitr�ria. */
    uint16_t TTT;         /**< Leitura do sensor de Temperatura. */
} uart_var;

//==============================================================================
// Prot�tipos de Fun��es
//==============================================================================

/**
 * @brief Realiza a sequ�ncia de inicializa��o e calibra��o do elevador.
 * @details Move o elevador para o andar mais baixo (andar 1) para estabelecer uma posi��o inicial de refer�ncia ("homing").
 * Depois, configura e ativa todas as interrup��es necess�rias para a opera��o.
 */
void Start();

/**
 * @brief Desliga o motor do elevador.
 * @details Configura o registrador do m�dulo CCP3 para desativar a sa�da PWM, parando o motor.
 */
void Motor_Turn_Off();

/**
 * @brief Liga o motor do elevador.
 * @details Configura o registrador do m�dulo CCP3 para ativar a sa�da PWM, acionando o motor.
 */
void Motor_Turn_On();

/**
 * @brief Adiciona uma nova requisi��o de viagem � rota do elevador.
 * @param origin O andar de origem da chamada (0-3).
 * @param dest O andar de destino da chamada (0-3).
 */
void New_request(int origin, int dest);

/**
 * @brief Fun��o de tratamento de interrup��o para recep��o de dados UART.
 * @details L� os caracteres recebidos via UART, monta o comando (origem e destino) e chama New_request().
 */
void Read_UART();

/**
 * @brief Atualiza o buffer de transmiss�o UART com os dados mais recentes do elevador.
 * @details Coleta informa��es como andar atual, pr�ximo destino, estado do motor, altura, velocidade e temperatura,
 * e as formata no buffer 'uart_write_buffer'.
 */
void Update_tx_buffer();

/**
 * @brief Envia os dados do buffer de transmiss�o via UART.
 * @details Chama Update_tx_buffer() para garantir que os dados est�o atualizados e ent�o transmite o conte�do do buffer.
 */
void Write_UART();

/**
 * @brief Verifica se a rota do elevador est� vazia (sem requisi��es pendentes).
 * @return Retorna 'true' se n�o houver nenhuma requisi��o na rota, 'false' caso contr�rio.
 */
bool Route_empty();

/**
 * @brief Encontra o �ndice da pr�xima parada na rota do elevador.
 * @return O �ndice (na matriz 'FLOORS') da pr�xima parada programada.
 */
uint8_t Next_index();

/**
 * @brief Determina qual � o pr�ximo andar de destino com base na rota atual.
 * @return O n�mero do pr�ximo andar de destino (0-3).
 */
uint8_t Next_floor();

/**
 * @brief Coloca o elevador em modo de espera (idle).
 * @details Se n�o houver requisi��es, esta fun��o move o elevador de volta para o andar t�rreo (FLOOR1).
 */
void Idle();

/**
 * @brief Envia um bloco de dados via comunica��o SPI.
 * @param data Ponteiro para o buffer de dados a ser enviado.
 * @param dataSize O n�mero de bytes a serem enviados.
 */
void Tx_spi(uint8_t *data, size_t dataSize);

/**
 * @brief Inicializa a matriz de LED MAX7219 com as configura��es padr�o.
 */
void Ini_matrix();

/**
 * @brief Atualiza o display da matriz de LED para mostrar o n�mero do andar atual.
 * @param display_floor O n�mero do andar (0-3) a ser exibido.
 */
void Matrix_update_floor(uint8_t display_floor);

/**
 * @brief Atualiza o display da matriz de LED para mostrar a dire��o do movimento e as chamadas pendentes.
 */
void Matrix_update_dir();

/**
 * @brief Rotina de Tratamento de Interrup��o (ISR) para o sensor do Andar 4.
 * @details Acionada quando o elevador chega ao Andar 4. Atualiza o estado do sistema.
 */
void ISR_Floor4();

/**
 * @brief Rotina de Tratamento de Interrup��o (ISR) para o sensor do Andar 3.
 * @details Acionada quando o elevador chega ao Andar 3. Atualiza o estado do sistema.
 */
void ISR_Floor3();

/**
 * @brief Rotina de Tratamento de Interrup��o (ISR) para o sensor do Andar 2.
 * @details Acionada quando o elevador chega ao Andar 2. Atualiza o estado do sistema.
 */
void ISR_Floor2();

/**
 * @brief Rotina de Tratamento de Interrup��o (ISR) para o sensor do Andar 1.
 * @details Acionada quando o elevador chega ao Andar 1. Atualiza o estado do sistema.
 */
void ISR_Floor1();

//==============================================================================
// Vari�veis Globais
//==============================================================================

char uart_read_buffer[10];                               /**< Buffer para armazenar os dados brutos recebidos da UART. */
int uart_read_buffer_count = 0;                          /**< Contador de bytes para o buffer de recep��o da UART. */
char uart_write_buffer[21] = {'$','1',',','1',',','0',',','0','0','0',',','y','0','.','0',',','T','T','.','T','\r',}; /**< Buffer formatado para transmiss�o de dados via UART. */
uart_var var;                                            /**< Inst�ncia da struct que armazena todas as vari�veis de estado do elevador. */
const uint8_t FLOORS[6] = {0,1,2,3,2,1};                 /**< Define a sequ�ncia de percurso do elevador (0->1->2->3->2->1->0...). */
bool route[6] = {0,0,0,0,0,0};                           /**< Array que armazena as requisi��es de parada em cada ponto do percurso definido em FLOORS. */
uint8_t index = 0;                                       /**< �ndice atual do elevador na matriz de percurso 'FLOORS'. */
uint8_t current_floor = 0;                               /**< Armazena o andar atual do elevador (0 para Andar 1, 1 para Andar 2, etc.). */
int8_t next_floor = 0;                                   /**< Armazena o pr�ximo andar de destino. */
uint16_t position = 0;                                   /**< Vari�vel para c�lculos de posi��o/altura. */
uint16_t position0 = 0;                                  /**< Vari�vel para armazenar a posi��o inicial em um c�lculo de deslocamento. */
uint16_t positionf = 0.0;                                /**< Vari�vel para armazenar a posi��o final em um c�lculo de deslocamento. */
char position_string[4] = {'0','0','0','\0'};            /**< String para armazenar a posi��o convertida para texto (uso futuro). */
char velocidade_string[4] = {'0','0','0','\0'};          /**< String para armazenar a velocidade convertida para texto (uso futuro). */
uint16_t velocidade = 0;                                 /**< Armazena o valor calculado da velocidade. */
bool stop_2s = 0;                                        /**< Flag para controlar uma parada de 2 segundos (n�o totalmente implementado). */
bool idle;                                               /**< Flag que indica se o elevador est� no modo de espera (true) ou em opera��o (false). */

//==============================================================================
// Constantes da Matriz de LED
//==============================================================================

/**
 * @brief Mapas de bits para desenhar os n�meros dos andares (1, 2, 3, 4) na matriz de LED.
 */
const uint8_t MATRIX_FLOOR[]={
    //1  
    0b00000000, 0b00000001, 0b11111111, 0b01000001,   
    //2
    0b01110001, 0b10001001, 0b10000101, 0b01000011,
    //3
    0b01111110, 0b10001001, 0b10001001, 0b01000010,
    //4
    0b00001000, 0b11111111, 0b01001000, 0b00111000    
};

/**
 * @brief Comandos de configura��o para o controlador da matriz de LED MAX7219.
 */
const uint8_t MATRIX_CONFIG[] = {
    0x09,0x00,  // Decode mode = 0
    0x0A,0x00,  // Intensity 1/32
    0x0B,0x07,  // Scan Limit
    0x0C,0x01,  // Shutdown mode = 1
    0x0F,0x01,  // Display-Test = 1
    0x0F,0x00,  // Display-Test = 0
};

/**
 * @brief Mapas de bits para desenhar as setas de dire��o (subindo, descendo, parado) na matriz de LED.
 */
const uint8_t MATRIX_DIR[] ={
    // Subindo
    0b01000000, 0b00100000, 0b01000000,
    // Descendo
    0b01000000, 0b10000000, 0b01000000,
    // Parado
    0b01000000, 0b01000000, 0b01000000
};

#endif	/* MAIN_H*/

