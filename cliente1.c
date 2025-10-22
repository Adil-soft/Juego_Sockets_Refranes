#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>



/*
---ESTRUCTURA PARA LA DEFINICION DE DIRECCIONES :
struct sockaddr_in {
        short sin_familly ;//LA FAMILIA DEL PROTOCOLO PARA TCP ES :AF_INET  ADDRESS FAMILLY INTERNET 
                          //que no es lo mismo que PF_INET : que identifica protocolo de familia de internet
        u_short sin_port ;//PARA DEFINIR EL PUERTO DE LA DIRECCION 
        struct in_addr sin_addr;//PARA DEFINIR LA DIRECCION 
        char sin_zero[8]


};
*/



int main(int argc, char *argv[]){

        char *server_ip , user[20];
        in_port_t server_port;
        int sock = 0 ;
        int rtnVal_inet =0; //valor devuelto por inet_pton 
        struct sockaddr_in sockAddr ;
        char buffer[100];
        socklen_t len_sockAddr ;
        fd_set selector ,fdset ;
        int salida =0, max_fd;
        int desconect =0;
        
        //COMPROBAR LOS ARGUMENTOS DE ENTRADA: 
        if(argc <=1 ){
                perror("Error:el numero de argumentos insertados es incorrecto. por favor intentalo de nuevo...¡\n");
                exit(EXIT_FAILURE) ;
        }else if(argc >=3){
              server_ip = argv[1];
              server_port=atoi(argv[2]);
              
             
              //definir el DNS : 
              if(strcmp("JugarRuletaDelaSUerte.com",server_ip) ==0){
                strcpy(server_ip,"127.0.0.1");
              }
                
        }
        

         printf("ip:%s\n",server_ip) ;
         printf("port:%d\n", server_port);
        
        //PARTE 1 : CREAR EL SOCKET : 
        if( ( sock = socket(AF_INET, SOCK_STREAM , 0 )) == -1){
                perror("Error: Ocurrio un problema en la apertura del socket\n");
                printf("Error: %d-la familia de la direccion indicado no es Valido\n",    EAFNOSUPPORT);
                exit(0) ;
        
        }
        
        //PARTE 2: ESTABLECER LOS DATOS DE DIRECCIONAMIENTO DE RED CON EL SERVIDOR :
                sockAddr.sin_family = AF_INET ;
                //convetir la direccion IPV4 O IPV6  de formato string (texto) a binario:
                rtnVal_inet = inet_pton(AF_INET, server_ip , &sockAddr.sin_addr.s_addr) ;      
                if(rtnVal_inet ==0){
                        perror("La direccion indicado es invalido ,por favor intentalo de nuevo.\n");
                        exit(EXIT_FAILURE);
                }else if(rtnVal_inet< 0) 
                        perror("la invocacion de la funcion inet_pont ha fallido \n");
                sockAddr.sin_port = htons(server_port); //convertir el puerto a codigo binario 
               
              
        //parte 3 : ESTABLECER LA CONEXION CON EL SERVIDOR y TRATAMIENTO DE ERRORES:
        
        len_sockAddr =sizeof(sockAddr);
        if( connect(sock, (struct sockaddr*) &sockAddr,len_sockAddr) <0){
                perror("La conexion con el servidor ha fallido\n");
                exit(EXIT_FAILURE);
         }else printf("good\n");
         
         
         //inicializar las estructuras FD_SET :
         FD_ZERO(&fdset);
         FD_ZERO(&selector);
         FD_SET(0,&fdset);
         FD_SET(sock,&fdset);
         
         
        //PARTE 4: TRANSMISSION DE LA INFORMACION :
        do{
                selector = fdset ;
                max_fd = sock+1;
                
                if((salida =select(max_fd,&selector,NULL,NULL,NULL))==-1){
                        perror("Error:problema de inovcacion de la funcion select\n");
                        close(sock);
                        exit(EXIT_FAILURE);
                }
                if(FD_ISSET(sock,&selector)){
                      bzero(buffer,sizeof(buffer));
                      recv(sock,buffer,sizeof(buffer),0) ;
                      printf("%s",buffer);
                      if(strcmp(buffer,"Numero de usuarios ha superado maximo\n"){
                                desconect = 1 ;
                      }else  if(strcmp(buffer,"+OK...Desconexion del servidor...\n")==0){
                                desconect = 1 ;
                }
                
                /*     
                recv(sock,buffer,sizeof(buffer),0) ;
                puts("enviar mensaje al servidor eco:");
                fgets(buffer,sizeof(buffer),stdin) ;
                strtok(buffer,"\n");
                if( send(sock, buffer,100,0)==-1){
                        perror("Error:problema de envio de mensaje al servidor\n");
                        exit(EXIT_FAILURE);
                        
                }
                /*recv(sock, buffer,sizeof(buffer),0);
                printf("mensaje del servidor:%s\n",buffer);*/
         }while(desconect==0) ;
        
        
        close(sock);
        exit(0) ;
        
        
        
}













































