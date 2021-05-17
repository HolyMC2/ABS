#ifndef SERVO
#define	SERVO


#define RangoDeVoltajeAdc               ((unsigned int) 5)
#define ValorMaximoDeAdc                ((unsigned int) 1023)
#define FactorConversionVoltsAKiloVolts ((unsigned int) 1000)
// a partir de aqui poner la declaracion de macros o prototipos de funciones

//prototipo de funcion para configurar el oscilador
void Configurar_Oscilador(void);

//prototipo de funcion para configurar las salidas que controlaran el motor 1
void Configurar_Salida_Motor_1(void);

//prototipo de funcion para configurar las salidas que controlaran el motor 2
void Configurar_Salida_Motor_2(void);

//prototipo de funcion para configurar la entrada donde se conectara el potenciometro 1
void Configurar_Entrada_Analogica_1(void);

//prototipo de funcion para configurar la entrada donde se conectara el potenciometro 2
void Configurar_Entrada_Analogica_2(void);

//Prototipo de la funcion para la configuracion inicial del ADC
void ConfigurarAdc(void);

//prototipo de funcion para leer la entrada analogica 1
unsigned int Leer_Kilovolts_Entrada_Analogica_1(void);

//prototipo de funcion para leer la entrada analogica 2
unsigned int Leer_Kilovolts_Entrada_Analogica_2(void);

//prototipo de funcion para cambiar el estado de la salida 1
void Cambiar_Estado_Salida_1(unsigned char Estado);

//prototipo de funcion para cambiar el estado de la salida 2
void Cambiar_Estado_Salida_2(unsigned char Estado);

//prototipo de funcion para configurar el timer 1
void Configurar_Timer_1(void);

//prototipo de funcion para habilitar la interrupcion del timer 1
void Activar_Interrupcion_Timer_1(void);

//prototipo de funcion para iniciar el timer 1 desde el main
void Iniciar_Timer_1_from_main(void);

//prototipo de funcion para iniciar el timer 1 desde la funcion de interrupcion
void Iniciar_Timer_1_from_interrupt(void);

//prototipo de funcion para detener el timer 1
void Detener_Timer_1(void);

#endif	/* SERVO */