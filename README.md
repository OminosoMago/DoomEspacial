# DoomEspacial
-Este repositorio cuenta con varias partes, cuenta con el codigo del supuesto ordenador que estaria en tierra y el codigo del hipotetico cubesat.  
&nbsp;&nbsp;&nbsp;&nbsp;-Para correr el primero:  
    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-Primero debemos modificar client.c dentro de la carpeta client_listening y cambiar la macro de ip poniendo la ip de destino que le &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;corresponda a el satelite.  
    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-Tras esto solo debemos ejecutar el script run.sh que se encuentra en la carpeta de client_listening.  
    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-Este ejecutable sirve de dos cosas para enviar el input de teclado del ordenador y para recibir la imagen del satelite.  
&nbsp;&nbsp;&nbsp;&nbsp;-Para correr el codigo del satelite:  
    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-Al igual que con el codigo del cliente debemos ir a la carpeta de doom-ascii dentro de ella a la carpeta de src y alli entrar a &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;d_main.c, aqui debemos cambiar el macro por la ip del cliente  
    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-Luego debemos ir a la carpeta de doom-ascii-master y ejecutar el archivo run.sh  
    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-A veces es necesario limpiar los .o, para ello debemos usar el comando make clean, y luego el make, tras esto deberia correr sin &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;problemas.  
-Los archivos del juego los sacamos de este repositorio de github https://github.com/wojciech-graj/doom-ascii, nosotros hemos modificado ligeramente los archivos del juego para que envie la pantalla.  
