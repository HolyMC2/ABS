//incluir archivos de cabecera
#include "sensor.h"
#include <xc.h>
#include <pic16f877a.h>


void Configurar_Entradas_De_Interrupciones_PuertoB(void)
{
    //configurar pines del puerto B como entradas digitales
    //ANSELH = 0; en el PIC16f877a el puerto B solo es digital
    //configurar RB0, RB1, RB2 y RB3 como entradas *en el pic 16f877a solo los pines B4, B5, B6 y B7 tienen interrupciones, pag. 45
    TRISBbits.TRISB4 = 1;
    TRISBbits.TRISB5 = 1;
 
    //configurar los pines que podran ser utilizados como interrupcion por cambio de estado en el puerto B
    // IOCBbits.IOCB0 = 1; //habilita interrupcion por cambio de estado en RB0
    // IOCBbits.IOCB1 = 1; //habilita interrupcion por cambio de estado en RB1
    // IOCBbits.IOCB2 = 1; //habilita interrupcion por cambio de estado en RB2
    // IOCBbits.IOCB3 = 1; //habilita interrupcion por cambio de estado en RB3
    // IOCBbits.IOCB4 = 0; //Deshabilita interrupcion por cambio de estado en RB4
    // IOCBbits.IOCB5 = 0; //Deshabilita interrupcion por cambio de estado en RB5
    // IOCBbits.IOCB6 = 0; //Deshabilita interrupcion por cambio de estado en RB6
    // IOCBbits.IOCB7 = 0; //Deshabilita interrupcion por cambio de estado en RB7

    //limpiar bandera de interrupcion por cambio de estado en el puerto B
    INTCONbits.RBIF = 0;
    //habilitar interrupcion por cambio de estado en el puerto B
    INTCONbits.RBIE = 1;
    //habilitar interrupciones globales
    INTCONbits.GIE = 1;
    //habilitar interrupciones de perifericos
    INTCONbits.PEIE = 1;
}

void Configurar_Timer1(void)
{
    // Timer1 Register:
    // T1CONbits.T1GINV = 1; // Timer1 gate is active-high (Timer1 counts when gate is high) *no aplica pic 16f877a
    // T1CONbits.TMR1GE = 0; // If TMR1ON = 0: This bit is ignored *no aplica pic 16f877a
    T1CONbits.T1CKPS0 = 1; // 11 = 1:8 Prescale Value
    T1CONbits.T1CKPS1 = 1; // 11 = 1:8 Prescale Value
    T1CONbits.T1OSCEN = 0; // LP oscillator is off
    T1CONbits.T1SYNC = 0;
    T1CONbits.TMR1CS = 1; // Internal clock (FOSC/4)
    T1CONbits.TMR1ON = 0; // Detener Timer 1

    //configurar el timer 1 para que expire cada 100 ms  8bits = 256 -((delay*FOSC)/prescalar*4))
    TMR1H = 1; // Msb
    TMR1L = 31; // Lsb
}

void Iniciar_Timer1(void)
{
    //iniciar cuenta del timer 1
    T1CONbits.TMR1ON = 1; 
}

void Detener_Timer1(void)
{
    //detener cuenta del timer 1
    T1CONbits.TMR1ON = 0; 
}

