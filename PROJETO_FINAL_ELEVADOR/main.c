#include "mcc_generated_files/mcc.h"
#include <stdio.h>
#include "main.h"

void Motor_Turn_Off(){
    if(CCP3CONbits.CCP3M != 0x00){
        CCP3CONbits.CCP3M = 0x00;
        Matrix_update_dir();      
    }
    return;
}
void Motor_Turn_On(){
    if(CCP3CONbits.CCP3M != 0x0C){
        CCP3CONbits.CCP3M = 0x0C;
        Matrix_update_dir();
    }
    return;
}
void New_request(int origin, int dest){
  int index_ori;
  if(origin == dest){return;}
  int index_;
  if(origin < dest){
      index_ = index + 1;
      for(int i=0; i<MAX_INDEX; i++){
          index_ = index_ % MAX_INDEX;
          if(FLOORS[index_] == origin){
              route[index_]=1;
              index_ori = index_;
              break;
            }  
          index_++;
        }
      for(int i=0; i<MAX_INDEX; i++){
          index_++;
          index_ = index_ % MAX_INDEX;
          if(FLOORS[index_]== dest)
            {
              route[index_] = 1;
              break;
            }          
        }     
    }
  
    if(origin >= dest){
      if(index > 3)index_ = index+1;
      else index_ = 3;
      
      for(int i=0; i<MAX_INDEX; i++){
          index_ = index_ % MAX_INDEX;
          if(FLOORS[index_] == origin){
              route[index_]=1;
              break;
            }  
          index_++;
        }
      for(int i=0; i<MAX_INDEX; i++){
          index_++;
          index_ = index_ % MAX_INDEX;
          if(FLOORS[index_]== dest)
            {
              route[index_] = 1;
              break;
            }          
        }     
      }  
  
  Matrix_update_dir();
  if(origin == current_floor & MOTOR_ON==0)
    {
      __delay_ms(2000);
      route[index_ori] = 0;
      Matrix_update_dir();
    }
  return;
}
void Read_UART(){    
    EUSART_Receive_ISR(); 
    uart_read_buffer[uart_read_buffer_count] = EUSART_Read();    
    if(uart_read_buffer[uart_read_buffer_count] =='\r'){
        int ori = uart_read_buffer[0] - '0';
        int dest = uart_read_buffer[1] - '0';
        ori--;
        dest--;
        New_request(ori,dest);
        uart_read_buffer_count = 0;  
        return;
      }
    
    uart_read_buffer_count++;


    return;
}
void Update_tx_buffer(){
    var.A = current_floor;   
    if(MOTOR_ON && MOTOR_DIR==0) var.M = 2;      
    else if(MOTOR_ON && MOTOR_DIR==1) var.M=3;
    else var.M=0;
    if(TMR0_ReadTimer() < 3) TMR0_Reload(); 
    position0 = var.HHH;     
    if(MOTOR_DIR != 0) var.HHH += (TMR0_ReadTimer()*1655)/1000;
    if(MOTOR_DIR == 0) var.HHH -= (TMR0_ReadTimer()*1655)/1000;
    TMR0_Reload();    
    if(var.HHH >= position0) var.VVV = ((var.HHH - position0)*100/3)&0x01FF;
    else var.VVV = ((position0 - var.HHH)*100/3)&0x01FF;    


    ADCON0bits.GO_nDONE = 1;         // inicia conversão
    while (ADCON0bits.GO_nDONE);     // espera terminar
    var.TTT = ((ADRESH << 8) + ADRESL);    
    
    uart_write_buffer[1] = var.A + 1 + '0';
    uart_write_buffer[3] = var.D + 1 + '0';
    uart_write_buffer[5] = var.M + '0';
    uart_write_buffer[7] = (var.HHH / 100) % 10 + '0';     // centena
    uart_write_buffer[8]= (var.HHH/ 10) % 10 + '0';      // dezena
    uart_write_buffer[9]=var.HHH % 10 + '0';  
    uart_write_buffer[11]= (var.VVV/100)%10 + '0';     // dezena
    uart_write_buffer[12]= ((var.VVV/10)%10) + '0'; 
    uart_write_buffer[14]= (var.VVV)%10 + '0';
    uart_write_buffer[16]= (var.TTT/100)%10 + '0';
    uart_write_buffer[17]= (var.TTT/10)%10 + '0';     
    uart_write_buffer[19]= (var.TTT%10) + '0';
}
void Write_UART(){
  Update_tx_buffer();
    for(int i=0; i<21; i++){
        EUSART_is_tx_ready();
        EUSART_Write(uart_write_buffer[i]); 
      } 
  return;
}
bool Route_empty(){
  for(int i=0; i<MAX_INDEX; i++){
      if (route[i] != 0) return false;
    }
  return true;
}
uint8_t Next_index(){
    int index_ = 0;
    for(int i=1; i<MAX_INDEX;i++){
        index_ = (index+i)%MAX_INDEX;
        if(route[index_]==1){        
        return index_;                
        }
    }
    return index_;
}
uint8_t Next_floor(){
    for(int i=1; i<MAX_INDEX;i++){
        int index_ = (index+i)%MAX_INDEX;
        if(route[index_]==1){
        next_floor = FLOORS[index_];
        return next_floor;                
        }
    }
    next_floor = 0;
    return next_floor;
}
void Start(){
    INTERRUPT_GlobalInterruptDisable();
    INTERRUPT_PeripheralInterruptDisable();
    INTCONbits.IOCIE = 0;
    INTCONbits.PEIE = 0;
    PIE1bits.ADIE = 0;
    PIE1bits.RCIE = 0;
    PIE2bits.C2IE = 0;
    PIE1bits.TMR1IE = 0;
    MOTOR_DIR = 0;
    Motor_Turn_On();
    __delay_ms(7000);    
    Motor_Turn_Off();
    Matrix_update_floor(FLOOR1);
    index = 0;
    MOTOR_DIR = 1;
    TMR0_Reload();
    position = 0;
    
    INTCONbits.IOCIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.ADIE = 1;
    PIE1bits.RCIE = 1;
    PIE2bits.C2IE = 1;
    PIE1bits.TMR1IE = 1;
    
  return;
}
void Idle(){
  idle = true;
  var.D =FLOOR1;
  if(current_floor == FLOOR1){
      MOTOR_DIR = 1;
      Motor_Turn_Off();
      index = FLOOR1;
    }
  else{
      MOTOR_DIR = 0;             
      Motor_Turn_On();  
    }
  return;
}
void Tx_spi(uint8_t *data, size_t dataSize){
    LATBbits.LATB1 = 0;    
    NOP();
    NOP();
    SPI1_ExchangeBlock(data,dataSize);// Tx
    LATBbits.LATB1 = 1;         // Desativa CS1    
}
void Ini_matrix(){
    uint8_t data[2];            // Buffer para tx spi
    uint8_t k=0;                // Ponteiro do arranjo da configuração das matrizes    
  
    for(uint8_t i=0;i<6;i++){   // Envia os 8 valores de configuração        
        data[0]= MATRIX_CONFIG[k];    // Define o endereço dos registradores de configuração
        data[1]= MATRIX_CONFIG[k+1];// Define o valor dos registradores de configuração 
        k=k+2;
        Tx_spi(data, 2); 
    }
    
    data[1] = 0b00000000;
    for(uint8_t i=1;i<9;i++){
    data[0] = i;
    Tx_spi(data, 2); 
    }    
}
void Matrix_update_floor(uint8_t display_floor){
    uint8_t data[2];                  
    display_floor = display_floor*4;            
     
   for(uint8_t i=5;i<=8;i++){
        data[0] = i;                   // Digito i da Matriz Direita       
        data[1] = MATRIX_FLOOR[display_floor];   // Valor do digito i da Matriz Direita
        Tx_spi(data, 2);        // Tx valores dos dígitos dig para as matrizes  
        display_floor++;
    }       
}
void Matrix_update_dir(){
    uint8_t data[2];
    uint8_t display_dir;
    
    bool route_[4];
    route_[0] = route[0];
    route_[1] = route[1] | route[5];
    route_[2] = route[2] | route[4];
    route_[3] = route[3];
    
    uint8_t route_leds = 0;
    
    for (int i = 0; i < 4; i++) { // Fila dos 4 andares do led
        if (route_[i])  route_leds |= (1 << i); 
    }
    
    if(CCP3CONbits.CCP3M == 0) display_dir = 6;      
    else display_dir = MOTOR_DIR*3;
    data[0] = 1;                   // Digito i da Matriz Direita       
    data[1] = MATRIX_DIR[display_dir] | route_leds;
    Tx_spi(data, 2); 
    display_dir++; 
     
    for(uint8_t i=2;i<=3;i++){         
        data[0] = i;                   // Digito i da Matriz Direita       
        data[1] = MATRIX_DIR[display_dir];   // Valor do digito i da Matriz Direita
        Tx_spi(data, 2); 
        display_dir++; 
         
    } 
}
void ISR_Floor4(){ 
    current_floor = FLOOR4;
    index = FLOOR4;
    
    Motor_Turn_Off();  
    __nop ();
    Motor_Turn_On();
    
    Matrix_update_floor(current_floor);
    TMR0_Reload();
    var.HHH = 180;    
      
    
    return;
   }
void ISR_Floor3(){     
    current_floor = FLOOR3;
    index++;     
    
    Motor_Turn_Off();  
    __nop ();
    Motor_Turn_On();  
    
    Matrix_update_floor(current_floor);   
    
    return;
}
void ISR_Floor2(){ 
    IOCBNbits.IOCBN3 = 0;
    IOCBFbits.IOCBF3 = 0;
    current_floor = FLOOR2;
    index++; 
    
    Motor_Turn_Off();  
    __nop ();
    Motor_Turn_On();  
    
    Matrix_update_floor(current_floor);
       
    IOCBNbits.IOCBN3 = 1;
    return;
}
void ISR_Floor1(){    
    IOCBNbits.IOCBN0 = 0;
    IOCBFbits.IOCBF0 = 0;
    current_floor = FLOOR1;
    index=FLOOR1; 
    
    Motor_Turn_Off();  
    __nop ();
    Motor_Turn_On();  
    
    Matrix_update_floor(current_floor); 
    TMR0_Reload();
    var.HHH = 0;
    
    IOCBNbits.IOCBN0 = 1;
    return;
}
void main(){
    // initialize the device
    SYSTEM_Initialize();

    IOCBF0_SetInterruptHandler(ISR_Floor1);
    IOCBF3_SetInterruptHandler(ISR_Floor2);
    EUSART_SetRxInterruptHandler(Read_UART);
    TMR1_SetInterruptHandler(Write_UART);
    CS1_SetHigh();    
    SPI1_Open(SPI1_DEFAULT);        // Configura MSSP1
    Ini_matrix();
    

    Start();
    
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    int next_index;
    while (1)
    {           
        bool is_empty = Route_empty();         
        if(is_empty==false){
            if(idle){
                idle = false;
                Motor_Turn_Off ();
                __delay_ms(500);
              }
            var.D = Next_floor();
            next_index = Next_index();
            if(current_floor <= var.D) MOTOR_DIR = 1;
            else MOTOR_DIR=0;
            
            Motor_Turn_On();          

            while(current_floor != var.D);
            index = next_index;
            route[index] = 0;
            
            Motor_Turn_Off();
            __delay_ms(2000);
            
             

                     
          }
        else Idle();       
       
    } 
   
        

}
