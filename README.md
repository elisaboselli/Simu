# Simu
Simulación 2017 (unrc)

Configuraciones en geany:

  *Para utilizar la libreria "lcgrand" (o cualquier otra libreria), seleccionar el triángulo al lado del botón "Construir" y seleccionar "Establecer comandos de construcción". En el renglón "Comandos de C", en el campo "Construir" (2) agregar despues de "%f": lcgrand.c -lm (o la libreria correspondiente). (EJEMPLO: gcc -Wall -o "%e" "%f" lcgrand.c -lm)
  
  *Para guardar los ejecutables en un direcctorio bin, en el mismo campo donde se agrego la libreria, agregar antes de "%e" (dentro de las comillas) la ruta del directorio deseado. (EJEMPLO: gcc -Wall -o "./bin/%e" "%f")
  
  *Para correr ejecutables que se encuentran en un directorio diferente al actual, en la misma ventana, en el renglón "Ejecutar comandos", agregar antes de "%e" (dentro de las comillas) la ruta del directorio. (EJEMPLO: "./bin/%e")
