// Use project enums instead of #define for ON and OFF.

#include "servo.h"
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

// variables del servo
#define EstadoEnAlto                    ((unsigned char) 1) // Estado en Alto de la señal
#define EstadoEnBajo                    ((unsigned char) 0) // Estado en Bajo de la señal
#define TRUE                            ((unsigned char) 1) // Estado en Alto de la señal
#define FALSE                           ((unsigned char) 0) // Estado en Bajo de la señal


#define CienPorciento                   ((unsigned char) 100) // cien porciento
#define PeriodoDePwmEnMicrosegundos     ((unsigned int) 20000) // periodo de pwm es de 20ms = 20,000us
#define ServoMaxTiempoEnAltoEnMicroseg  ((unsigned int) 2500) // El maximo tiempo de la señal en alto para mover un servomotor a 180 grados es 2.5ms = 2,500us
#define TiempoDeCompensacionEnMicroseg  ((unsigned int) 500) // Offset o compensacion para ligar el voltaje del potenciometro con la posicion de 0 grados del servomotor 0.5ms = 500us
#define Servo180GradosEnMicroseg        (ServoMaxTiempoEnAltoEnMicroseg - TiempoDeCompensacionEnMicroseg) // Tiempo en alto para mover un servomotor a 180 grados es 2.5ms = 2,500us, pero para ligarlo con el voltaje del potenciometro, tenemos que restarle un offset o compensacion
#define ServoMax  ((unsigned int) 2500) // El maximo tiempo de la señal en alto para mover un servomotor a 180 grados es 2.5ms = 2,500us
#define UnaCuentaTmr1_En_Us             ((unsigned char) 2) // una cuenta del timer 1 ocurre cada 2us = 0.002ms = 0.000002 segundos, segun mi calculo de excel
#define CuentasMaxParaDesbordeTmr1      ((unsigned long) 65536) // numero de cuentas necesarias para que se desborde el timer 1, segun mi calculo de excel
#define NumeroDeServosAControlar        ((unsigned char) 2) // numero de servomotores a controlar

#define DejarLos8BitsMenosSignificativos  ((unsigned long) 0x00FF)
#define DejarLos8BitsMasSignificativos    ((unsigned long) 0xFF00)

typedef enum {
    CambioEnlasSalidas = 0,
    CompensacionDeLaSalida
} ServoPwmStates;

enum {
    Servo_1 = 0,
    Servo_2,
    TiempoMuerto
};

ServoPwmStates EstadoDeLaSignal = CambioEnlasSalidas;
//variable que indica el porcentaje de duty cycle
unsigned char Pwm_Duty_Cycle = 0;
// si esta variable esta en 0 significa que debemos contar el timpo en bajo y si esta en 1 significa que debemos contar el tiempo en alto
unsigned char EstadoActual = EstadoEnBajo; 
//variable que contiene el tiempo a contar en bajo
unsigned long TiempoEnBajo_EnUs = 0;
//variable que contiene el tiempo a contar en alto
unsigned long TiempoEnAlto_EnUs = 0;
//variable que contine el valor a precargar en el registro  TMR1L
unsigned char ValorDelRegistroTMR1L = 0;
//variable que contine el valor a precargar en el registro  TMR1H
unsigned char ValorDelRegistroTMR1H = 0;
//variable que indica el actual servo que se esta controlando
unsigned char ServoControlado = Servo_1;

//vector para almacenar el voltaje en kilovolts del potenciometro 1 y 2
unsigned int VoltajeEnKilovolts[] = {0, 0};
//vector para almacenar el tiempo en alto actual de cada uno de los servomotores en microsegundos
unsigned int VectorDeTiempoEnAltoDeLosServos[] = {500, 500}; //iniciar en la posicion 0 grados todos los servomotores (0.5ms = 500us)
//vector para almacenar el tiempo de compensacion de la señal en alto de cada uno de los servomotores en microsegundos
unsigned int VectorDeTiempoDeCompensacionEnAlto[] = {0, 0}; 

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

void main(void) 
{
    
    // //configurar oscilador interno a 8Mhz    *el pic 16f877a no tiene oscilador interno
    // OSCCON = 0x71;
    // //esperar a que el oscilador interno este estable
    // while(OSCCONbits.HTS == 0); 
    
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
    Configurar_Timer_1();
    //activar la interrupcion del timer 1
    Activar_Interrupcion_Timer_1();
    //iniciar la cuenta del timer 1
    Iniciar_Timer_1_from_main();
    
    //bucle infinito
    while(1)
    {
        // si el sensor1 lee un 5% mas que el sensor 2, y el freno esta activo prender abs1 
        if (RpmsSensor1 > (RpmsSensor2 * 1.05) | )
        {
            //activar servo1
        }
                
        if (RpmsSensor2 > (RpmsSensor1 * 1.05) | )
        {
            //activar servo2
        }
    }
    return;
}
