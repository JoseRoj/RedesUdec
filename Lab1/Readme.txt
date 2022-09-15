Instrucciones de uso
-> Para poder trabajar primero tenemos que ir al directorio donde se encuentra el Lab1

-> A continuación se crea la carpeta build con : mkdir build, en donde se deberá colocar el archivo a enviar y la llave , ambos archivos se encuentran en la carpeta Archivos( "Foto.jpeg" y "Llave.key"). Luego sobre el directorio de la carpeta build se debe utilizar el comando "cmake .." y después "make", con esto se logra obtener los ejecutables.

-> Ya con los ejecutables, se tiene que ejecutar el server y luego client. Server requiere de 2 argumentos los cuales son la llave en común con el cliente y el archivo a enviar (el cual se encrypta dentro del programa).
	
Ejemplo: "./server "Nombre de la llave" "Nombre del archivo a encriptar" " en este caso sería,
"./server Llave.key Foto.jpeg ". Mientras que para el client en otra terminal tiene un único argumento el cual es la llave.
   Ejemplo: ./client "Nombre de la llave", es decir, "./client Llave.key" .

-> Finalmente client, generará el archivo desencryptado,es car, con su contenido legible y para ello podemos ocupar md5 para comprobar qué son exactamente él mismo archivo.
   
EXTRA: Para ver mas claro que el cliente genera el archivo podemos ejecutarlo una carpeta más abajo, esto es:(El directorio donde se realiza este comando es en Lab1) ./build/client LLave.key .
Tener en cuenta y la llave que ocupamos es LLave.key y que el archivo que se encripta tiene que estar en la carpeta desde donde se lanzará el cliente.
EXTRA2 : Se usa cifrado simétrico lo cual solo debe tener una llave tanto para cifrar como para descifrar.