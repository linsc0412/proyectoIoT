Tendedero Automático Inteligente + Garaje y Control de Iluminación

Este proyecto implementa un sistema de casa inteligente basado en Arduino UNO programado con Arduino IDE, que incluye:

Tendedero automático que se retrae cuando detecta lluvia.

Apertura automática de garaje mediante un sensor ultrasónico HC-SR04.

Control automático de iluminación LED usando una fotoresistencia (LDR) con filtrado y anti-ruido.

El sistema integra sensores, actuadores y lógica de control para demostrar automatización doméstica a pequeña escala.

Características principales del sistema

✔️ 1. Tendedero Automático (Servo + Sensor de Lluvia)

Sensor digital de lluvia (LOW = lluvia detectada).

El servo retrae la ropa cuando llueve.

Cuando NO hay lluvia, el servo extiende la ropa automáticamente.

✔️ 2. Garaje Automático (Servo + HC-SR04)

HC-SR04 mide distancia del “vehículo”.

Si un objeto está a 20 cm o menos, la puerta del garaje se abre.

Si no hay nada cerca, se mantiene cerrada.

✔️ 3. Control de Iluminación (LDR + LEDs)

LDR mide niveles de luz ambiental.

Si hay demasiada luz o flash → LEDs apagados.

Si hay poca luz → LEDs encendidos.


Software Utilizado
Arduino IDE

Este proyecto está desarrollado y cargado al Arduino UNO mediante:

🔧 Arduino IDE (Versión recomendada: 1.8.x o 2.x)

Funciones del entorno:
Edición del código en C/C++

Selección del puerto y placa (Arduino UNO)

Compilación y carga al microcontrolador

Uso del Monitor Serial para visualizar lecturas

Cómo ejecutar el proyecto
1. Instalar Arduino IDE

Descargar desde:
https://www.arduino.cc/en/software

2. Abrir el código (.ino)

Cargar este archivo en Arduino IDE.

3. Configurar placa y puerto

Placa: Arduino UNO

Puerto: el que detecte tu PC (COMx)

4. Subir el código

Presiona Upload para compilar y cargar.

5. Abrir el Monitor Serial

Selecciona Tools → Serial Monitor

Baud rate: 9600
