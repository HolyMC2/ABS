

//incluir el archivo .h
#include "servo.h"
#include <pic16f877a.h>
#include <xc.h>


#define _XTAL_FREQ 20000000


//Funcion para configurar las salidas que controlaran el motor 1
void Configurar_Salida_Motor_1(void)
{
    //mandar un estado logico de cero a nuestra salida RD0
    PORTDbits.RD0 = 0;
    //configuramos como salidas RD0
    TRISDbits.TRISD0 = 0;
}

//Funcion para configurar las salidas que controlaran el motor 2
void Configurar_Salida_Motor_2(void)
{
    //mandar un estado logico de cero a nuestra salida RD1
    PORTDbits.RD1 = 0;
    //configuramos como salidas RD1
    TRISDbits.TRISD1 = 0;
}

//Funcion para configurar el timer 0, expira cada 0.05ms
void Configurar_Timer0(void)
{
    /*****Port Configuration for Timer ******/
    OPTION_REG = 0b00000100;  // Timer0 with external freq and 32 as prescalar // Also Enables PULL UPs
    TMR0=248;       // Load the time value for 0.0001s; delayValue can be between 0-256 only
    TMR0IE=1;       //Enable timer interrupt bit in PIE1 register
    GIE=1;          //Enable Global Interrupt
    PEIE=1;         //Enable the Peripheral Interrupt
    /***********______***********/      
}

