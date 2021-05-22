// Use project enums instead of #define for ON and OFF.

// #include "servo.h"
#include "sensor.h"
#include <xc.h>
#include <pic16f877a.h>


// variables del sensor
#define PulsosMaximosPorSensor          ((unsigned char) 10) //este numero incluye los pulsos dados por los cambios de 0 a 1 y de 1 a 0 que provocan interrupcion en el puerto B

#define TiempoDeUnaCuentaTmr            ((unsigned char) 4)  //una cuenta del timer equivale a 4us, considerando las configuraciones hechas para el timer 1

#define FactorConversionSeg_Us          ((unsigned long) 1000000)  //factor de conversion de segundos a microsegundos
#define SegundosEnUnMinuto              ((float) 60)  //una cuenta del timer equivale a 4us, considerando las configuraciones hechas para el timer 1
#define CuentasMaximasTimer            ((unsigned long) 65535)  //cuentas maximas que puede contar el timer 1 considerando que es un timer de 16 bits


unsigned char ContadorSensor1 = 0;
unsigned char ContadorSensor2 = 0;

unsigned int TiempoInicioSensor1 = 0;
unsigned int TiempoInicioSensor2 = 0;

unsigned int TiempoFinSensor1 = 0;
unsigned int TiempoFinSensor2 = 0;

unsigned int RpmsSensor1 = 0;
unsigned int RpmsSensor2 = 0;

unsigned char PulsoEnAltoDetectadoSensor1 = 0;
unsigned char PulsoEnAltoDetectadoSensor2 = 0;

//variable para contar los tics del timer0
static unsigned int cuenta_timer0;
//variable del angulo del servomotor
static char angle;
//variable para contar tics en on (tiempo que esta en 1 la señal pwm)
static unsigned int Counter_on[2];
//variable para indicar el servo controlado
static unsigned char ServoControlado;
static int servo_code[] = {0b00000001, 0b00000010}; //pin 0 y 1


void __interrupt() SensorInterrupciones()
{
    //verificar si la interrupcion fue por cambio de estado en el puerto B
    if ( INTCONbits.RBIF == 1 )
    {
        //verificar cual sensor causo la interrupcion
        if ((PORTBbits.RB4 == 1 && PulsoEnAltoDetectadoSensor1 == 0) || (PORTBbits.RB4 == 0 && PulsoEnAltoDetectadoSensor1 == 1))
        {
            //verificar si es el primer pulso detectado
            if (ContadorSensor1 == 0)
            {
                //guardar la cuenta actual del timer
                TiempoInicioSensor1 = ( ((unsigned int)TMR1H) << 8) + TMR1L;
            }

            //Cambiar el estado de la variable PulsoEnAltoDetectadoSensor1 
            if(PulsoEnAltoDetectadoSensor1 == 0)
            {
                PulsoEnAltoDetectadoSensor1 = 1;
            }
            else   
            {
                PulsoEnAltoDetectadoSensor1 = 0;
            }
            
            //incrementar el contador del sensor 1
            ContadorSensor1++;
            //verificar si el contador ya alcanzo las cuentas necesarias de una vuelta
            if (ContadorSensor1 == PulsosMaximosPorSensor)
            {
                //guardar la cuenta actual del timer
                TiempoFinSensor1 = ( ((unsigned int)TMR1H) << 8) + TMR1L;
                //verificar cual tiempo es mayor
                if (TiempoFinSensor1 > TiempoInicioSensor1)
                {
                    //calcular las rpms
                    RpmsSensor1 = (unsigned int)( SegundosEnUnMinuto / ( ( (float)(TiempoFinSensor1 - TiempoInicioSensor1)) * TiempoDeUnaCuentaTmr / FactorConversionSeg_Us) );
                }
                else
                {
                    //calcular las rpms
                    RpmsSensor1 = (unsigned int)( SegundosEnUnMinuto / ( ( (float)((CuentasMaximasTimer - TiempoInicioSensor1) + TiempoFinSensor1) * TiempoDeUnaCuentaTmr) / FactorConversionSeg_Us) );
                }
                
                //restablecer el contador
                ContadorSensor1 = 0;
            }
        }
                    //verificar cual sensor causo la interrupcion
        if ((PORTBbits.RB5 == 1 && PulsoEnAltoDetectadoSensor2 == 0) || (PORTBbits.RB5 == 0 && PulsoEnAltoDetectadoSensor2 == 1))
        {
            //verificar si es el primer pulso detectado
            if (ContadorSensor2 == 0)
            {
                //guardar la cuenta actual del timer
                TiempoInicioSensor2 = ( ((unsigned int)TMR1H) << 8) + TMR1L;
            }
            
            //Cambiar el estado de la variable PulsoEnAltoDetectadoSensor2 
            if(PulsoEnAltoDetectadoSensor2 == 0)
            {
                PulsoEnAltoDetectadoSensor2 = 1;
            }
            else   
            {
                PulsoEnAltoDetectadoSensor2 = 0;
            }

            //incrementar el contador del sensor 1
            ContadorSensor2++;
            //verificar si el contador ya alcanzo las cuentas necesarias de una vuelta
            if (ContadorSensor2 == PulsosMaximosPorSensor)
            {
                //guardar la cuenta actual del timer
                TiempoFinSensor2 = ( ((unsigned int)TMR1H) << 8) + TMR1L;
                //verificar cual tiempo es mayor
                if (TiempoFinSensor2 > TiempoInicioSensor2)
                {
                     //calcular las rpms
                    RpmsSensor2 = (unsigned int)( SegundosEnUnMinuto / ( ( (float)(TiempoFinSensor2 - TiempoInicioSensor2) * TiempoDeUnaCuentaTmr) / FactorConversionSeg_Us) );
                }
                else
                {
                    //calcular las rpms
                    RpmsSensor2 = (unsigned int)( SegundosEnUnMinuto / ( ( (float)((CuentasMaximasTimer - TiempoInicioSensor2) + TiempoFinSensor2) * TiempoDeUnaCuentaTmr) / FactorConversionSeg_Us) );
                }
                
                //restablecer el contador
                ContadorSensor2 = 0;
            }
        }
            
        //limpiar bandera de interrupcion
        INTCONbits.RBIF = 0;
    }
    
}


//funcion de interrupcion para manejar las interrupciones/
void interrupt timer0_isr()
{
    if(TMR0IF == 1)
    {
        TMR0 = 248;
        TMR0IF = 0;
        cuenta_timer0++; // la cuenta incrementa cada 0.05ms
    }

    if(cuenta_timer0 >= 400) //se cumplieron los 20ms
    {
        cuenta_timer0 = 0;
        if (ServoControlado == 0)
        {
            ServoControlado = 1;
        }
        else
        {
            ServoControlado = 0;
        }
    }

    if (cuenta_timer0 <= (Counter_on[ServoControlado]))
    {
        PORTD = PORTD | servo_code[ServoControlado];
    }
    else
    {
        PORTD = PORTD & ~(servo_code[ServoControlado]);
    }

}

void servo_angle(char angle, unsigned char ServoControlado)
{

    Counter_on[ServoControlado] = (angle * 0.0112)/0.05;
}

void main(void) 
{
    
    // SENSOR
    //configurar interrupcion por cambio de estado en el puerto B
    Configurar_Entradas_De_Interrupciones_PuertoB();
    //configurar timer 1
    Configurar_Timer1();
    //iniciar la cuenta del timer
    Iniciar_Timer1();

    // SERVO
    //configurar salida del motor 1
    Configurar_Salida_Motor_1();
    //configurar salida del motor 2
    Configurar_Salida_Motor_2();
    //configurar timer 1
    Configurar_Timer0();
    ADCON1 = 0b00000111; //RA0 como salida digital
    CMCON = 7; //desactivar modulo comparador en puerto A
    TRISAbits.TRISA0 = 1; //A1 como input
    // puertos D0 y D1 son salidas
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;

    //bucle infinito
    while(1)
    {
        // si el sensor1 lee un 5% mas que el sensor 2, y el freno esta activo prender abs1 
        if (RpmsSensor1 > (RpmsSensor2 * 1.1) &&PORTAbits.RA0 == 0)
        {
            //activar servo1
            RD0 = 1;
        }
        else
        {
            RD0 = 0;
        }
     //   posinicial(servo_1)
                
        // if (RpmsSensor2 > (RpmsSensor1 * 1.1))
        // {
        //     //activar servo2
        //     RD1 = 1;
        // }
        //         else
        // {
        //     RD1 = 0;
        // }
     //   posinicial (servo_2)
    }
    return;
}
