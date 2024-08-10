## Compilación
Para compilar los programas hay que abrir una terminal de comandos con la dirección donde se encuentren descargados los codigos .cpp.
Al abrirla se debe ingresar el comando "g++ -o generator generator.cpp" y presionar la tecla "Enter", de igual forma para compilar el sorter es necesario escribir el comando "g++ -o sorter sorter.cpp" en la terminal y presionar la tecla "Enter".

## Ejecución de los programas
Lo primero es generar el archivo binario, este se crea mediante el uso del generator.cpp, una vez compilado se escribe el siguiente comando en la terminal "./generator -size <SIZE> -output <OUTPUT FILE PATH>", 
donde <SIZE> se reemplaza por el tamaño deseado del archivo binario ya sea SMALL(512MB), MEDIUM(1GB) o LARGE(2GB), y <OUTPUT FILE PATH> se reemplaza por la ruta donde se desea que se cree el archivo + \Nombre deseado del archivo.bin.

## Ejecución del sorter
Una vez creado el archivo binario, este debe ser ordenado por el sorter.cpp para que lo convierta en .txt, para ejecutar el sorter se escribe el comando "./sorter -input <Nombre del archivo.bin> -output <Nombre deseado del.txt> -alg <Algoritmo deseado>"
El algoritmo puede ser quicksort(QS) que es el más óptimo, insertionsort(IS) no tan óptimo, y bubblesort(BS) nada óptimo.
