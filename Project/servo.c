
//agregar los archivos de cabecera necesarios
#include "servo.h"
#include <xc.h>
#include <pic16f877a.h>


unsigned int ConversionADC = 0; 
unsigned int VoltajeEnKiloVolts = 0; 

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

//Funcion para configurar la entrada donde se conectara el potenciometro 1
void Configurar_Entrada_Analogica_1(void)
{
    //configurar RA0 como entrada analogica
    ANSELbits.ANS0 = 1;
    //configurar como entrada RA0
    TRISAbits.TRISA0 = 1;
}

//Funcion para configurar la entrada donde se conectara el potenciometro 2
void Configurar_Entrada_Analogica_2(void)
{
    //configurar RA1 como entrada analogica
    ANSELbits.ANS1 = 1;
    //configurar como entrada RA1
    TRISAbits.TRISA1 = 1;
}

//Funcion para la configuracion inicial del ADC
void ConfigurarAdc(void)
{
    //configurar seleccion del reloj para la conversion ADC Fosc/32, segun la tabla de rangos recomendados.
    ADCON0bits.ADCS0 = 0;
    ADCON0bits.ADCS1 = 1;
    //configurar el canal donde se leera la señal analogica RA0/ANS0
    ADCON0bits.CHS0 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS3 = 0;
    //habilitar modulo ADC
    ADCON0bits.ADON = 1;
    //configurar foprmato del resultado a la derecha
    ADCON1bits.ADFM = 1;
    //configurar voltajes de referencia como el voltaje propio del micro
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
}

//Funcion para leer la entrada analogica 1
unsigned int Leer_Kilovolts_Entrada_Analogica_1(void)
{
    //configurar el canal donde se leera la señal analogica RA0/ANS0
    ADCON0bits.CHS0 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS3 = 0;
    //esperar el tiempo de adquicision
    __delay_us(30);
    //iniciar la conversion de ADC
    ADCON0bits.GO_DONE = 1;
    //esperar a que la conversion termine
    while (ADCON0bits.GO_DONE == 1);
    //obtener el valor de la conversion ADC
    ConversionADC = ((((unsigned int)ADRESH) << 8) + ADRESL);
    //convertir las cuentas de ADC a voltaje
    VoltajeEnKiloVolts = (unsigned int)(((unsigned long)ConversionADC * RangoDeVoltajeAdc * FactorConversionVoltsAKiloVolts) / ValorMaximoDeAdc);

    return VoltajeEnKiloVolts;
}

//Funcion para leer la entrada analogica 2
unsigned int Leer_Kilovolts_Entrada_Analogica_2(void)
{
    //configurar el canal donde se leera la señal analogica RA1/ANS1
    ADCON0bits.CHS0 = 1;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS3 = 0;
    //esperar el tiempo de adquicision
    __delay_us(30);
    //iniciar la conversion de ADC
    ADCON0bits.GO_DONE = 1;
    //esperar a que la conversion termine
    while (ADCON0bits.GO_DONE == 1);
    //obtener el valor de la conversion ADC
    ConversionADC = ((((unsigned int)ADRESH) << 8) + ADRESL);
    //convertir las cuentas de ADC a voltaje
    VoltajeEnKiloVolts = (unsigned int)(((unsigned long)ConversionADC * RangoDeVoltajeAdc * FactorConversionVoltsAKiloVolts) / ValorMaximoDeAdc);

    return VoltajeEnKiloVolts;
}

//Funcion para cambiar el estado de la salida 1
void Cambiar_Estado_Salida_1(unsigned char Estado)
{
    PORTDbits.RD0 = Estado;
}

//Funcion para cambiar el estado de la salida 2
void Cambiar_Estado_Salida_2(unsigned char Estado)
{
    PORTDbits.RD1 = Estado;
}

//Funcion para configurar el timer 1
void Configurar_Timer_1(void)
{
    //Configurar  Compuerta del Timer1 es activa en alto (Timer1 counts when gate is high)
    T1CONbits.T1GINV = 1; 
    T1CONbits.TMR1GE = 0; // If TMR1ON = 0: This bit is ignored
    //configurar prescalador del timer1
    T1CONbits.T1CKPS1 = 1;
    T1CONbits.T1CKPS0 = 0;
    //apagar oscilador LP
    T1CONbits.T1OSCEN = 0;
    //seleccionar la fuente del reloj como Fosc/4
    T1CONbits.TMR1CS = 0;
    //detener el timer 1
    T1CONbits.TMR1ON = 0;
    //contar 131ms
    TMR1H = 0;
    TMR1L = 0;    
}

//Funcion para activar la interrupcion del timer 1
void Activar_Interrupcion_Timer_1(void)
{
    //limpiar bandera de interrupcion del timer 1
    PIR1bits.TMR1IF = 0;
    //habilitar interrupcion del timer 1
    PIE1bits.TMR1IE = 1;
    //activar interrupciones perifericas
    INTCONbits.PEIE = 1;
    //activar interrupciones globales
    INTCONbits.GIE = 1;
}

//Funcion para iniciar el timer 1 desde la funcion main
void Iniciar_Timer_1_from_main(void)
{
    //limpiar bandera de interrupcion del timer 1
    PIR1bits.TMR1IF = 0;
    //iniciar timer 1
    T1CONbits.TMR1ON = 1;
}

//Funcion para iniciar el timer 1 desde la funcion de interrupcion
void Iniciar_Timer_1_from_interrupt(void)
{
    //limpiar bandera de interrupcion del timer 1
    PIR1bits.TMR1IF = 0;
    //iniciar timer 1
    T1CONbits.TMR1ON = 1;
}

//Funcion para configurar el timer 1
void Detener_Timer_1(void)
{
    //Detener timer 1
    T1CONbits.TMR1ON = 0;
}