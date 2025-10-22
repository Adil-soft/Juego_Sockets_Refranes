#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <stdbool.h>

struct Jugador{
        
        int id;
        char nombre[10];
        char password ;
        int turno;
        int puntos;
        char Resultado;
        
        
};

int main(){
        int count_users = 1 ;
        int sock = 2 ;
          char *table="Jugadores";
        char *campo1="Id" ;
        char *campo2="SOCK";
        int value1 = 1 ;
        int value2 = 233;
        const char *query[100];
        int status =0;
        
        printf("MySQL client version: %s\n", mysql_get_client_info());
        //parte 1 : iniciar apuntador a la base de datos :
        MYSQL *con = mysql_init(NULL) ;
        //parte 2 : comprobar el correcto de inovcacion de mysql_init 
        if(con ==NULL){
        
                fprintf(stderr,"%s\n" , mysql_error(con));
                exit(EXIT_FAILURE);
        }
        //parte 3: establecer conexion con el servidor de base de datos local :
        if (mysql_real_connect(con, "localhost", "adil", "AdilHirchi2285@",NULL, 0, NULL, 0) == NULL){
                
            fprintf(stderr, "%s\n", mysql_error(con));
            mysql_close(con);
            printf("test 1 \n");
            exit(EXIT_FAILURE);
               
                
            
         }
         printf("hola 1\n");
         //parte 4 : crear la base de datos del juego de la ruleta de la suerte :
         if (mysql_query(con, "CREATE DATABASE IF NOT EXISTS JuegoRuletaSuerte")){
                 fprintf(stderr, "%s\n", mysql_error(con));
                 mysql_close(con);
                 printf("test 2\n");
                 exit(EXIT_FAILURE);
         }else if(mysql_query(con, "USE JuegoRuletaSuerte")){
                printf("test use\n");
         }
        printf("hola 2\n");
        //crear tabla de usuarios :
        if( mysql_query(con, "DROP TABLE IF EXISTS Jugadores")) {
                fprintf(stderr,"%s\n", mysql_error(con));        
        
        }
        if(mysql_query(con,"CREATE TABLE Jugadores(Id INT,SOCK INT,nombre VARCHAR(255),password VARCHAR(20) , puntos INT , resultado varchar(20) default 'Premio', ESTADO VARCHAR(20) DEFAULT 'DESCONECTADO')")){
                
                fprintf(stderr,"%s\n", mysql_error(con));
                mysql_close(con) ;
                printf("test3\n");
                exit(EXIT_FAILURE);
        
        }
        
        sprintf((char *)query, "INSERT INTO %s(%s,%s) VALUES ('%d','%d')", table, campo1,campo2, value1,value2);

         
         if( mysql_query(con,(const char*)query)) {
                fprintf(stderr,"%s\n", mysql_error(con));        
        
        }
        
        sprintf((char*) query, "SELECT *FROM %s WHERE %s=%d",table,campo1,value1) ;
         
         if( mysql_query(con,(const char*)query)) {
                fprintf(stderr,"%s\n", mysql_error(con));        
        
        }
        
         do{
              MYSQL_RES *result = mysql_store_result(con);

              if (result == NULL)
              {
                  perror("error\n");
                  exit(0);
              }

              MYSQL_ROW row = mysql_fetch_row(result);

              printf("%s\n", row[0]);

              mysql_free_result(result);

              status = mysql_next_result(con);

              if (status > 0) {
                  perror("Error\n");
                 }
         }while(status==0);
         
         //base de datos de la partida :
          if (mysql_query(con, "CREATE DATABASE IF NOT EXISTS PARTIDAS")){
                 fprintf(stderr, "%s\n", mysql_error(con));
                 mysql_close(con);
                 printf("test 2\n");
                 exit(EXIT_FAILURE);
         }else if(mysql_query(con, "USE PARTIDAS")){
                printf("test use\n");
         }
         
          if( mysql_query(con, "DROP TABLE IF EXISTS Mesas")) {
                fprintf(stderr,"%s\n", mysql_error(con));        
        
        }
         
           if(mysql_query(con,"CREATE TABLE Mesas(id_partida INT NOT NULL AUTO_INCREMENT,num_jugadores INT,nombre VARCHAR(20) , puntos INT , resultado varchar(20) default 'Premio')")){
                
                fprintf(stderr,"%s\n", mysql_error(con));
                mysql_close(con) ;
                printf("test3\n");
                exit(EXIT_FAILURE);
        
        }
         
         
         
         
         
         
         
         
         
         
         
         
        mysql_close(con);
        return 0 ; 
}























