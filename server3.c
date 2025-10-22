#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>
#include "menu.h"
#include <signal.h>
#include "funciones.h"
#include <sys/time.h>
#include <mysql/mysql.h>
#include <stdbool.h>

//estructura de control de tiempo de espera :
/*struct timeval{
        long tv_sec;
        long tv_usec;

}*/
#define Puerto 2002
#define MAX_USERS 1

int main(){
        MYSQL *con; 
        int sock=0 , id_cliente=0, len_buffer , count_users=0, salida=0, i =0, id=0, j =0,turno=0, valid_user = 0 ;
        char * server_ip, buffer[100] ;
        in_port_t server_port ;
        struct sockaddr_in sock_server ;
        struct sockaddr_in sockaddr_cliente; 
        socklen_t sockaddr_cliente_len =0;
        struct timeval tiempo ; // asignar variable tiempo a la esctructura timeval 
        tiempo.tv_sec = 3;
        tiempo.tv_usec=500000;//500000us=0.5s
        fd_set  fdset , selector;
        char *ignorar , *user,*password , *pass, *usuario;
        int solicitud_jugar =0;
        
        //PARTE 1 : CREAR EL SOCKET DE PARTE DEL SERVIDOR :
        sock=socket(AF_INET, SOCK_STREAM, 0) ;
        
        //tratamiento de errores de la apertura del socket :
        if(sock==-1) {
                perror("Error:problema de apertura del socket.intente de nuevo.gracias\n");
                exit(EXIT_FAILURE);
        }
        //configurar las opciones del socket por medio de la funcion setsockopt:
        int value = 1 ;
        setsockopt(sock,SOL_SOCKET,SO_REUSEADDR ,&value,sizeof(value));
        
        
        //Parte 2 : definir la estructura de direccionamiento :
        sock_server.sin_family = AF_INET ;
        sock_server.sin_addr.s_addr =htonl(INADDR_ANY) ;
        sock_server.sin_port = htons(Puerto) ;
        
        //PARTE 3 : asignar datos de direccion al servidor :
        if (bind (sock, (struct sockaddr *) &sock_server, sizeof (sock_server)) == -1)
	{
		perror("Error:problema en la invocacion de bind");
		exit(EXIT_FAILURE);
	}
	
        //PARTE 3 : SERVIDOR SE PONE EN ESTADO PASIVO EN ESPERA DE PETICIONES DEL CLIENTE :
        if( listen(sock,SOMAXCONN) == -1){
               perror("Error:porblema de invocacion de la funcion listen.\n");
               exit(EXIT_FAILURE);
        }
        
       
       //GESTION DE LA CONCURENCIA DE SOCKETS ,POR ELLOS USAMOS LAS ESTRUCTURAS FD_SET PARA INDICARLE A SELECT QUE SOCKET ESTA OBSERVANDO:
       //DONDE CADA BIT DEL ARRAY DE FD_SET REPRESENTA UN SOCKET :
       FD_ZERO(&fdset); //iniciar todos los bits a zero 
       FD_ZERO(&selector);
       FD_SET(sock,&fdset) ;//pone a 1 el bit indicado por sock
       FD_SET(0,&fdset);
       //FD_ISSET(sock, &fdset) ; //comprobar si el bit indicado por sock esta puesto a 1. 
       
       //PARTE 4 : TRATO DE PROCESOS ERRANTES : INVOCAR LA FUNCION SIGNAL PARA CEDER AL PROCESO PADRE LA GESTION O IGNORANCIA DE PROCESOS
       //HIJO ZUMBIS por medio de la funcion manejador:
       signal(SIGINT ,manejador) ;
       
        //PARTE 5 : EL SERVIDOR ESTABLECE LA CONEXION CON EL CLIENTE :
       //accept crea otro socket especial para cada peticion solicitada ,mientras siga atendiendo mas clientes
       //sockaddr_cliente : el servidor a la hora de aceptar el socket cliente,guarda la informacion de direccion
       //del cliente en la estructura sockaddr_cliente 
       sockaddr_cliente_len = sizeof(sockaddr_cliente);
       //DEFINIR UN BUCLE INFINITO PARA SERVIR LA SOLICITUD DE LOS CLIENTES(POLLING) :
       while(true){
                        
                //bloquear el porgrama en espera de recibir cualquier nueva solicitud de un cliente :
                //FD_SETSIZE :INDICA EL NUMERO DE LOS DESCRIPTORES QUE TIENEN QUE SER COMPROBADOS 
                  selector =fdset ;
                  if((salida= select(FD_SETSIZE,&selector,NULL,NULL,NULL))==-1){
                        perror("Error: ocurrio algun problema en la invocacion de la funcion select\n");
                        exit(EXIT_FAILURE);
                  
                  }else if(salida ==0){
                        perror("Error: tiempo de espera al cliente ha sido expirado.\n");
                        printf("el servidor se pondra en escucha a otra nueva soliciutd...\n");
                  }else if(salida >0){
                        //INDENTIFICAR EL SOCKET QUE RECIBIO LA CONEXION :
                        for(i = 0 ; i < FD_SETSIZE; i++){
                                if(i == sock){
                                      
                                    if( (id_cliente= accept(sock, (struct sockaddr*) &sockaddr_cliente,&sockaddr_cliente_len))==-1)
                                    {
                                         perror("Error:Problema para establecer la conexion con el cliente.\n");
                                         exit(EXIT_FAILURE);
                                    }else{
                                        //CONTROL DE NUMERO DE USUARIOS :
                                        if(count_users < MAX_USERS ){
                                        //conectar con la base de datoa del juego de la ruleta de la suerte :
                                                if((con=connectDB())==0) printf("Error:problema de conexion de base de datos\n");
                                                UseDataBase(con);
                                               //insertar datos identificadores del usuario en la base de datos :
                                               InsertData(con,count_users,id_cliente, "Conectado") ;
                                                
                                                bzero(buffer,sizeof(buffer));
                                                printf("ok ,usuario :%d\n",count_users);
                                                strcpy(buffer,"+OK, USARIO CONECTADO\n"); 
                                                FD_SET(id_cliente,&fdset);  
                                                send(id_cliente, buffer, strlen(buffer),0);
                                                count_users++;  
                                                bzero(buffer,sizeof(buffer));
                                                strcpy(buffer,"Insertar Usuario");
                                                send(id_cliente,buffer,strlen(buffer),0);
                                                printf("test ...\n");
                                                printf("id cliente:%d\n", id_cliente);
                                                //NOTIFICAR LOS JUGADORES DE LA INCORPORACION DE UN NUEVO JUGADOR :
                                                for(int j =0; j < count_users;j++){
                                                        printf("%d\n",j);
                                                        printf("test 1\n");
                                                        bzero(buffer,sizeof(buffer));
                                                        sprintf(buffer,"El Jugador ""%d"" ha sido Incorporado\n",id_cliente);
                                                        id= SelectCliente(con,j);
                                                        printf("%d\n",id);
                                                        send(id_cliente,buffer,strlen(buffer),0);
                                                        printf("test2\n");
                                                        
                                                }
                                              
                                        
                                        }else{
                                                //limpiar el buffer :
                                                bzero(buffer,sizeof(buffer));
                                                //copiar mensaje de notificacion en buffer :
                                                strcpy(buffer,"Numero de usuarios ha superado maximo");
                                                //enviar la notificacion el usuario :
                                                send(id_cliente, buffer, sizeof(buffer),0) ;
                                                //cerrar el socket correspondiente al usuario rechazado :
                                                close(id_cliente);
                                                
                                        
                                        }
                                    
                                    } 
                                }else if(i ==0){
                                     //preparar el buffer para lectura de datos de entrada: 
                                     bzero(buffer,sizeof(buffer));
                                     fgets(buffer,sizeof(buffer),stdin);
                                     strtok(buffer,"\n");
                                     if(strcmp(buffer,"salir") ==0){
                                              for(j =0; j < count_users;j++){
                                                        
                                                        bzero(buffer,sizeof(buffer));
                                                        strcpy(buffer,"+OK...Desconexion del servidor...\n");
                                                        id= SelectCliente(con,j);
                                                        printf("%d\n",id);
                                                        send(id_cliente,buffer,strlen(buffer),0);
                                                        printf("test finalizar 2\n");
                                                        close(id);
                                                        FD_CLR(id,&selector);
                                                        printf("test finalizar 3 \n");
                                                        
                                                }
                                               close(sock);
                                               printf("+OK ...SERVIDOR DEL JUEGO DESCONECTADO CON EXITO...\n");
                                               exit(0); 
                                              
                                                
                                    }else{
                                        bzero(buffer,sizeof(buffer));
                                        int bytes_recibidos=recv(i,buffer,sizeof(buffer),0);
                                        
                                        //comprobar los datos recibidos de recv:
                                        if(bytes_recibidos > 0 ){
                                                for(j=0 ; j < count_users ;j++){
                                                     //RECORRER LA BASE DE DATOS PARA RETORNAR LOS ID SOCKET DE LOS CLIENTES :
                                                     id= SelectCliente(con,j);  
                                                     //comprobar a quien le toca el turno :  
                                                     if(i == id){
                                                         turno = id ;
                                                     }
                                                }
                                                if(strcmp(buffer,"salir")==0){
                                                        DesconectarCliente(con,i,selector,count_users);
                                                
                                                }else if(strcmp(buffer,"USUARIO")==0){
                                                        sscanf(buffer,"%s %s",ignorar,user) ;
                                                        
                                                        //comprobar si el usuario existe :
                                                        if( selectClientePorUsuario(con, user)!=0 ){
                                                                bzero(buffer,sizeof(buffer));
                                                                sprintf(buffer,"+OK USUARIO CORRECTO,insertar PASSWORD  \n");
                                                                valid_user = 1 ;      
                                                                
                                                        }else{
                                                                bzero(buffer,sizeof(buffer));
                                                                sprintf(buffer,"Error: USUARIO NO EXISTE\n");
                                                        }
                                                        send(turno,buffer,sizeof(buffer),0);
                                                        
                                                  }else if(strcmp(buffer,"PASSWORD")==0 && valid_user==1){
                                                        sscanf(buffer,"%s %s",ignorar,password);
                                                         pass=testPassword(con, turno);
                                                        if(strcmp(pass,password) ==0){
                                                                bzero(buffer,sizeof(buffer));
                                                                sprintf(buffer,"+OK PASSWORD CORRECTO\n");
                                                        }else{
                                                                bzero(buffer,sizeof(buffer));
                                                                sprintf(buffer,"+OK PASSWORD INCORRECTO...\n");
                                                        }
                                                        send(turno,buffer,sizeof(buffer),0);
                                                  }else if(strcmp(buffer,"REGISTRAR")==0){
                                                         sscanf(buffer, "%s -U %s -P %s ",ignorar, usuario, password);
                                                         if(registrarUsurio(con, usuario,password) ==1){
                                                                bzero(buffer,sizeof(buffer));
                                                                 sprintf(buffer,"+OK USAURIO REGISTRADO CON EXITO\n");
                                                         }else {
                                                                bzero(buffer,sizeof(buffer));
                                                                sprintf(buffer,"+OK USAURIO REGISTRADO CON EXITO\n");   
                                                         }
                                                          send(turno,buffer,sizeof(buffer),0);
                                                        
                                                  }else if(strcmp(buffer,"INICIAR PARTIDA")==0){
                                                      solicitud_jugar++;
                                                      bzero(buffer,sizeof(buffer));
                                                      sprintf(buffer,"...INICIANDO LA PARTIAD...\n");
                                                      send(turno, buffer,sizeof(buffer),0);
                                                      if(solicitud_jugar < 1) {
                                                        sprintf(buffer,"...Esperando nuevos usaurios para el juego Grupal...\n");
                                                        bzero(buffer,sizeof(buffer));
                                                        send(turno, buffer,sizeof(buffer),0);
                                                        
                                                      }else{
                                                        
                                                              insertPartida(con, solicitud_jugar);
                                                              for(i = 0 ; i < count_users;i++){
                                                                 id= SelectCliente(con,j);
                                                                  RuletaDeLaSuerte(id);
                                                              }
                                                              
                                                              
                                                               
                                                      
                                                      
                                                      }
                                                      
                                                       
                                                  }
                                                }
                                                
                                         }     
                                        
                                        
                                }
                        
                        }
                  
                  }
                       
                   
       
               
        }
        printf("hola 3 \n");
        

        close(sock);

        return 0; 
}































