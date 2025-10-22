#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <stdbool.h>
#include <time.h>



void manejador (int signum){
    char yes[2];
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT,manejador);
    char msg[]= "Ctrl-c ha sido presionado. Desea usted parar el servidor del Juego? y/n ";
    printf("%s:",msg);
    scanf("%s", yes);
    if (strcmp(yes,"yes")==0){
        printf("EL servidor ha sido cancelado.\n");
        exit(1);
    }else
        printf("El servidor siguie en esucha ....\n");

}


MYSQL * connectDB(){

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
            return 0;
            
         }else return con ;
}


void UseDataBase(MYSQL *con){
        mysql_query(con, "USE JuegoRuletaSuerte");
        

}


void InsertData(MYSQL *con, int count_users, int sock,char *estado){
        char *table ="Jugadores" ;
        char *campo1 ="Id";
        char *campo2="SOCK" ;
        char *campo3="ESTADO" ;
        const char *query[100];
        int value1 , value2;
        value1 = count_users;
        value2=sock ;
        char *value3 =estado ;
 sprintf((char *)query, "INSERT INTO %s(%s,%s,%s) VALUES ('%d','%d','%s')", table, campo1,campo2,campo3, value1,value2,value3);

         
         if( mysql_query(con,(const char*)query)) {
                fprintf(stderr,"%s\n", mysql_error(con));        
        
        }
        


}



int SelectCliente(MYSQL *con, int id){

         int status =0 ;
         char *table ="Jugadores";
         char *campo1="SOCK";
         char *campo2="Id";
         int value1 = id;
         char *query[100];
         int sock ;
         
         sprintf((char*) query, "SELECT %s FROM %s WHERE %s=%d",campo1,table,campo2,value1) ;
         
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

              //printf("%s\n", row[0]);
              sock=atoi(row[0]);
              return sock;
              mysql_free_result(result);

              status = mysql_next_result(con);

              if (status > 0) {
                  perror("Error\n");
                  exit(0);
                 }
         }while(status==0);



}



int SelectClienteConectado(MYSQL *con, int id){

         int status =0 ;
         char *table ="Jugadores";
         char *campo1="SOCK";
         char *campo2="Id";
         int value1 = id;
         char *query[100];
         int sock ;
         char *campo3 ="ESTADO";
         char *value3 = "CONECTADO" ;
         
         
         sprintf((char*) query, "SELECT %s FROM %s WHERE %s=%d AND %s =%s",campo1,table,campo2,value1,campo3,value3) ;
         
         if( mysql_query(con,(const char*)query)) {
                fprintf(stderr,"%s\n", mysql_error(con));        
        
        }
        
         do{
              MYSQL_RES *result = mysql_store_result(con);

              if (result == NULL)
              {
                  perror("error\n");
                  return 0;
              }

              MYSQL_ROW row = mysql_fetch_row(result);

              //printf("%s\n", row[0]);
              sock=atoi(row[0]);
              return sock;
              mysql_free_result(result);

              status = mysql_next_result(con);

              if (status > 0) {
                  perror("Error\n");
                  return 0;
                 }
         }while(status==0);

        return 0 ;


}


void update(MYSQL *con, int id, char *estado){

        
         int status =0 ;
         char *table ="Jugadores";
         char *campo1="ESTADO";
         char *campo2="SOCK";
         int value = id;
         char *query[100];
         int sock ;
         char *conect_state =estado;
           
         sprintf((char*) query, "UPDATE %s SET %s=%s WHERE %s=%d",table,campo1,conect_state,campo2,value) ;
           
         if( mysql_query(con,(const char*)query)) {
                fprintf(stderr,"%s\n", mysql_error(con));        
        
        }
          
         


}


void DesconectarCliente(MYSQL *con , int sock ,fd_set selector , int count_users){
        
        char buffer[100];
        int i =0 , id_cliente = 0 ;
        
       //cerar el socket del cliente :
       close(sock) ;
       FD_CLR(sock,&selector) ;
       
       //actualizar la base de datos : 
       update(con,sock,"DESCONECTADO") ;
       
       //notificar los demas jugadores :
       bzero(buffer,sizeof(buffer));
       
       //definir el mensaje de notificacion :
       sprintf(buffer,"El jugador %d ha sido desconectado del juego\n", sock);
       
       //Recorrer la base de datos para distribuir el mensaje de notificacion :
       for(int i = 0 ; i < count_users; i++){
                id_cliente = SelectClienteConectado(con, i) ;
                send(id_cliente, buffer,strlen(buffer),0) ;
       
       }
       
       
       
       
       
       

}





int selectClientePorUsuario(MYSQL *con,char *user){

         int status =0 ;
         char *table ="Jugadores";
         char *campo1="nombre";
         char *campo2="Id";
         char *query[100];
         int sock ;
         char *campo3 ="ESTADO";
         char *value3 = user ;
         
         
         sprintf((char*) query, "SELECT %s FROM %s WHERE %s=%s",campo1,table,campo1,value3) ;
         
         if( mysql_query(con,(const char*)query)) {
                fprintf(stderr,"%s\n", mysql_error(con));        
        
        }
        
         do{
              MYSQL_RES *result = mysql_store_result(con);

              if (result == NULL)
              {
                  perror("error\n");
                  return 0;
              }

              MYSQL_ROW row = mysql_fetch_row(result);

              //printf("%s\n", row[0]);
              sock=atoi(row[0]);
              return sock;
              mysql_free_result(result);

              status = mysql_next_result(con);

              if (status > 0) {
                  perror("Error\n");
                  return 0;
                 }
         }while(status==0);

        return 0 ;


}


char* testPassword(MYSQL * con , int sock){

        int status =0 ;
         char *table ="Jugadores";
         char *campo1="PASSWORD";
         char *campo2="SOCK";
         int value1 = sock;
         char *query[100];
         char *campo3 ="ESTADO";
         char *password;
         
         
         sprintf((char*) query, "SELECT %s FROM %s WHERE %s=%d",campo1,table,campo2,value1) ;
         
         if( mysql_query(con,(const char*)query)) {
                fprintf(stderr,"%s\n", mysql_error(con));        
        
        }
        
         do{
              MYSQL_RES *result = mysql_store_result(con);

              if (result == NULL)
              {
                  perror("error\n");
                  return 0;
              }

              MYSQL_ROW row = mysql_fetch_row(result);

              //printf("%s\n", row[0]);
              password=row[0];
              return password;
              mysql_free_result(result);

              status = mysql_next_result(con);

              if (status > 0) {
                  perror("Error\n");
                  return 0;
                 }
         }while(status==0);

        return 0 ;
        

        

}


bool registrarUsurio(MYSQL *con,char *user, char *password){
        char *table ="Jugadores" ;
        char *campo1 ="nombre";
        char *campo2="PASSWORD" ;
        char *campo3="ESTADO" ;
        const char *query[100];
        char *value1 =user ;
        char *value2 = password;
        
 sprintf((char *)query, "INSERT INTO %s(%s,%s) VALUES ('%s','%s')", table, campo1,campo2,value1,value2);

         
         if( mysql_query(con,(const char*)query)) {
                fprintf(stderr,"%s\n", mysql_error(con));    
                return 0 ;    
        
        }
        
        return 1 ;

}


void insertPartida(MYSQL *con,int num_jugadores){
        char *table ="Mesas" ;
        char *campo1 ="num_jugadores";
    
        const char *query[100];
        int value1 =num_jugadores ;
        
 sprintf((char *)query, "INSERT INTO %s(%s) VALUES ('%d')", table, campo1,value1);

         
         if( mysql_query(con,(const char*)query)) {
                fprintf(stderr,"%s\n", mysql_error(con));        
        
        }
        


}


int RuletaDeLaSuerte(int jugador)
{
    char aux[100];
    char buffer[100];
    char buffer2[100];
    int i,j;
    int suerte, x=0;
    FILE *pf;
    system("clear");

    srand(time(NULL));
    suerte=rand()%15;
    

    if(pf=fopen("refranes.txt", "r"))
    {
        while(fgets(aux,100, pf))
        {
            if(x==suerte)
            {
                i=strlen(aux);
                for(j=0;j<i-1;j++)
                {
                    if(aux[j]!=' ' && aux[j]!= '\n' && aux[j]!=',')
                    {
                        aux[j]='-';
                        
                    }
                    else aux[j]=' ';
                }
                bzero(buffer2, sizeof(buffer2));
                sprintf(buffer2, "REFRAN A RESOLVER\n");
                send(jugador, buffer2,strlen(buffer2), 0);

                bzero(buffer, sizeof(buffer));
                strcpy(buffer, aux);
                send(jugador, buffer,strlen(buffer), 0);
            }
            x++;
        }
        
    }
    fclose(pf);
    return suerte;  
}
































































































