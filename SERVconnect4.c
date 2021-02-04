#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/time.h>

/* portul folosit */
#define PORT 2019
extern int errno;

void f_wait(int sig)
{
    waitpid(-1,NULL, WNOHANG);
}

int remiza(int tabla_joc[6][7])
{
    for(int i=0;i<6;i++)
    {
        for(int j=0;j<7;j++)
        {
            if(tabla_joc[i][j]==0)
            {
                return 0;
            }
        }
    }
    return 1;
}

int min(int a, int b)
{
    if(a<b)
        return a;
    else
        return b;
}

int check(int tabla_joc[6][7], int coloana)
{
    int i, j, ok, culoare, k, l;
    coloana=coloana-1;

    for(i=0; i<6, tabla_joc[i][coloana]==0; i++)
        ;
    culoare=tabla_joc[i][coloana];
    int linie=i;

    if(linie<3 && tabla_joc[linie+1][coloana]==culoare && tabla_joc[linie+2][coloana]==culoare && tabla_joc[linie+3][coloana]==culoare)
        return 1;

    if(coloana<3)//orizontal
        i=0;
    else
        i=coloana-3;
    for(; i<=coloana, i+3<7; i++)
    {
        ok=1;
        for(j=i; j<i+4; j++)
            if(tabla_joc[linie][j]!=culoare)
            {
                ok=0;
                break;
            }
        if(ok)
            return 1;
    }

    if(linie<3 || coloana<3) //diag principala
    {
        i=linie-min(linie, coloana);
        j=coloana-min(linie, coloana);
    }
    else
    {
        i=linie-3;
        j=coloana-3;
    }
    for(; i<=linie, j<=coloana, i+3<6, j+3<7; i++, j++)
    {
        ok=1;
        for(k=i, l=j; k<i+4; k++, l++)
            if(tabla_joc[k][l]!=culoare)
            {
                ok=0;
                break;
            }
        if(ok)
            return 1;
    }

    if(linie>2 || coloana<3) //diagonala sec
    {
        i=linie+min(5-linie, coloana);
        j=coloana-min(5-linie, coloana);
    }
    else
    {
        i=linie+3;
        j=coloana-3;
    }
    for(; i>=linie, j<=coloana, i-3>=0, j+3<7; i--, j++)
    {
        ok=1;
        for(k=i, l=j; k>i-4; k--, l++)
            if(tabla_joc[k][l]!=culoare)
            {
                ok=0;
                break;
            }
        if(ok)
            return 1;
    }


    return 0;
}

void actualizare_tabla(int tabla_joc[6][7],int mutare, int culoare)
{
    for(int i=5;i>=0;i--)
    {
          if(tabla_joc[i][mutare-1]==0)
          {
              tabla_joc[i][mutare-1]=culoare;
              break;
          }
    }
}
void afisare_tabla(int tabla_joc[6][7])
{
    for(int i=0;i<6;i++)
    {
        for(int j=0;j<7;j++)
        {
            printf("%d ",tabla_joc[i][j]);
        }
        printf("\n");
    }
}

void initializare_tabla_joc(int tabla_joc[6][7])
{
  for(int i=0;i<6;i++)
    {
        for(int j=0;j<7;j++)
        {tabla_joc[i][j]=0;}
    }
}

int main()
{
    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;
    int sd;	
    char informativ[1000]="CONNECT4\nAlegeti un username:\0";
    char lungime_informativ=strlen(informativ)+1;
    int client1,client2,copil;
    int length;
    fd_set readfds;
    fd_set actfds;
    struct timeval tv;
    int fd,nfds;

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }

    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));
    
    
    server.sin_family = AF_INET;	
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (PORT);
    
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

    if (listen (sd, 6) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }

    while(1)
    {
        signal(SIGCHLD,f_wait);
       

        printf ("[server]Asteptam jucatori la portul %d\n",PORT);
        fflush (stdout);
        
        length=sizeof(from);
        bzero(&from,sizeof(from));

        client1 = accept (sd, (struct sockaddr *) &from, &length);
        if(client1<0)
        {
            perror("[server]Eroare la accept().\n");
            continue;
        }

        write(client1,&lungime_informativ,sizeof(int));
        write(client1,informativ,lungime_informativ);

        FD_ZERO(&actfds);
        FD_SET(sd,&actfds);

        tv.tv_sec=10;
        tv.tv_usec=0;
        nfds=sd;

        int ok=0;
        while(ok!=1)
        {
            
            bcopy((char*) &actfds,(char*) &readfds,sizeof(readfds));
            if(select(nfds+1,&readfds,NULL,NULL,&tv)<0)
            {
                perror("[server]Eroare la select().\n");
                continue;
            }
            if(FD_ISSET(sd,&readfds))
            {
                length=sizeof(from);
                bzero(&from,sizeof(from));
   
                client2=accept(sd,(struct sockaddr *) &from, &length);
                if(client2<0)
                {
                    perror("[server]Eroare la accept().");
                     continue;
                }
                ok=1;

            }
            else
            {
                if(ok==0)
                 {
                    char invit[200];
                   
                    strcpy(invit,"Nu s-a gasit niciun partener de joc pentru dvs. Invitati un prieten la joc!");
                    int len=strlen(invit)+1;
                    write(client1,&len,sizeof(int));
                    write(client1,invit,len);
                    ok=-1;
                 } 
            }
        }
        


        write(client2,&lungime_informativ,sizeof(int));
        write(client2,informativ,lungime_informativ);


        if((copil=fork())<0)
        {
            perror("[server]Eroare la fork().\n");
            exit(1);
        }
        else
        {
            if(copil==0)
            {
                char username1[100];
                char username2[100]; 
                int lungime_username1;
                int lungime_username2;

                int tabla_joc[6][7]={0};
                int mutare1;
                int mutare2;

                int culoare;

                char text1[1000];
                char text2[1000];
                int lungime_text1,lungime_text2;

                strcpy(text1,"Veti juca impotriva jucatorului:");
                strcpy(text2,"Veti juca impotriva jucatorului:");

                read(client1,&lungime_username1,sizeof(int));
                read(client1,username1,lungime_username1);
                read(client2,&lungime_username2,sizeof(int));
                read(client2,username2,lungime_username2);
                   
                strcat(text1,username2);
                strcat(text2,username1);

                strcat(text1,"\nVeti avea culoarea rosu.\nDvs veti deschide jocul.\nSucces!");
                strcat(text2,"\nVeti avea culoarea galben.\nCelalalt jucator va deschide jocul.\nSucces!");
                text1[strlen(text1)]='\0';
                text2[strlen(text2)]='\0';
                lungime_text1=strlen(text1)+1;
                lungime_text2=strlen(text2)+1;

                write(client1,&lungime_text1,sizeof(int));
                write(client1,text1,lungime_text1);
                write(client2,&lungime_text2,sizeof(int));
                write(client2,text2,lungime_text2);

                culoare=1;
                write(client1,&culoare, sizeof(int));
                culoare=2;
                write(client2,&culoare,sizeof(int));
                
                int win=0,cod;
                int scor1=0,scor2=0;
                int repriza=1; 
                char scor[10];

                
                while(1)
                {
                    initializare_tabla_joc(tabla_joc);
                    while(1)
                    {                    
                        read(client1,&mutare1,sizeof(int));
                        if(mutare1==0)
                        {
                            scor2++;
                            write(client2,&mutare1,sizeof(int));
                            strcpy(text1,"");
                            strcpy(text1,"Jucatorul ");
                            strcat(text1,username1);
                            strcat(text1," a abandonat. Ati castigat. Scorul final este ");
                          sprintf(scor,"%d",scor2); strcat(text1,scor);strcat(text1,":"); sprintf(scor,"%d",scor1); strcat(text1,scor); 

                            strcat(text1,". Felicitari!\0");

                            lungime_text1=strlen(text1)+1;
                            write(client2,&lungime_text1,sizeof(int));
                            write(client2,text1,lungime_text1);
                            break;//trebuie facut break si in while-ul mai mare
                        }
                        
                        actualizare_tabla(tabla_joc,mutare1,1);

                        win=check(tabla_joc,mutare1);//verifica daca sunt secvente de lungime 4 de culoarea 1
                        if(win==1 || remiza(tabla_joc)==1) //castigator 1
                        {   cod=91;
                            if(win==1)
                            {
                                scor1++;
                                //printf("castigator juc 1");//pt verificare
                            }
                            fflush(stdout);
                            write(client1,&cod,sizeof(int));
                            write(client2,&cod,sizeof(int));
                            write(client2,&mutare1,sizeof(int));
                            char scor[10];
                            if(win==1)
                            {
                                strcpy(text1,"Felicitari! Ati castigat! Scorul pana in acest moment este ");
                                sprintf(scor,"%d",scor1); strcat(text1,scor);strcat(text1,":");sprintf(scor,"%d",scor2); strcat(text1,scor); 
                                strcat(text1,"\nContinuati cu inca o repriza? (y/n)\0");
                            }else{
                                strcpy(text1,"Remiza! Scorul pana in acest moment este ");
                                sprintf(scor,"%d",scor1); strcat(text1,scor);strcat(text1,":");sprintf(scor,"%d",scor2); strcat(text1,scor); 
                                strcat(text1,"\nContinuati cu inca o repriza? (y/n)\0");
                            }
                            lungime_text1=strlen(text1)+1; 
                            write(client1,&lungime_text1,sizeof(int));
                            write(client1,text1,lungime_text1);
                            if(win==1)
                            {
                                strcpy(text2,"Ati pierdut. Scorul pana in acest moment este ");
                                sprintf(scor,"%d",scor2);strcat(text2,scor);strcat(text2,":");sprintf(scor,"%d",scor1); strcat(text2,scor); 
                                strcat(text2,"\nContinuati cu inca o repriza? (y/n)\0");
                              
                            }else{
                                strcpy(text2,"Remiza! Scorul pana in acest moment este ");
                                sprintf(scor,"%d",scor2);strcat(text2,scor);strcat(text2,":");sprintf(scor,"%d",scor1); strcat(text2,scor); 
                                strcat(text2,"\nContinuati cu inca o repriza? (y/n)\0");
                            }
                            lungime_text2=strlen(text2)+1; 
                            write(client2,&lungime_text2,sizeof(int));
                            write(client2,text2,lungime_text2);
                            break;//citirea raspunsului se face in while-ul mai mare;
                        }
                        write(client2,&mutare1,sizeof(int));
                        read(client2,&mutare2,sizeof(int));
                        if(mutare2==0)
                        {
                            scor1++;
                            write(client1,&mutare2,sizeof(int));

                            strcpy(text2,"");
                            strcpy(text2,"Jucatorul ");
                            strcat(text2,username2);
                            strcat(text2," a abandonat. Ati castigat. Scorul final este ");
                            sprintf(scor,"%d",scor1); strcat(text2,scor);strcat(text2,":"); sprintf(scor,"%d",scor2); strcat(text2,scor);
                            strcat(text2,". Felicitari!\0");

                            lungime_text2=strlen(text2)+1;
                            write(client1,&lungime_text2,sizeof(int));
                            write(client1,text2,lungime_text2);

                            break;// trebuie facut break si in while-ul mai mare
                        }
                        actualizare_tabla(tabla_joc,mutare2,2);

                        win=check(tabla_joc,mutare2);//verifica daca sunt secvente de lungime 4 de culoare 2
                        if(win==1 || remiza(tabla_joc)==1) //castigator 2
                        {   
                            cod=92;
                            if(win==1)
                            {
                                scor2++;
                            }
                            
                            fflush(stdout);
                            write(client1,&cod,sizeof(int));
                            write(client2,&cod,sizeof(int));

                            write(client1,&mutare2,sizeof(int));
                            
                            if(win==1)
                            {
                                strcpy(text2,"Felicitari! Ati castigat! Scorul pana in acest moment este ");
                                sprintf(scor,"%d",scor2); strcat(text2,scor);strcat(text2,":");sprintf(scor,"%d",scor1); strcat(text2,scor); 
                                strcat(text2,"\nContinuati cu inca o repriza? (y/n)\0");
                            }else{
                                strcpy(text2,"Remiza! Scorul pana in acest moment este ");
                                sprintf(scor,"%d",scor2); strcat(text2,scor);strcat(text2,":");sprintf(scor,"%d",scor1); strcat(text2,scor); 
                                strcat(text2,"\nContinuati cu inca o repriza? (y/n)\0");
                            
                            }

                            lungime_text2=strlen(text2)+1; 
                            write(client2,&lungime_text2,sizeof(int));
                            write(client2,text2,lungime_text2);
                            if(win==1)
                            {
                                strcpy(text1,"Ati pierdut. Scorul pana in acest moment este ");
                                sprintf(scor,"%d",scor1); strcat(text1,scor);strcat(text1,":"); sprintf(scor,"%d",scor2); strcat(text1,scor); 
                                strcat(text1,"\nContinuati cu inca o repriza? (y/n)\0");
                            }else{
                                strcpy(text1,"Remiza! Scorul pana in acest moment este ");
                                sprintf(scor,"%d",scor1); strcat(text1,scor);strcat(text1,":"); sprintf(scor,"%d",scor2); strcat(text1,scor); 
                                strcat(text1,"\nContinuati cu inca o repriza? (y/n)\0");
                           
                            }
                            lungime_text1=strlen(text1)+1; 
                            write(client1,&lungime_text1,sizeof(int));
                            write(client1,text1,lungime_text1);
                            break;//citirea raspunsului se face in while-ul mai mare;
                        }

                        write(client1,&mutare2,sizeof(int));

                    }
                    char c1,c2;
                    read(client1,&c1,sizeof(char));
                    read(client2,&c2,sizeof(char));
                    if(c1=='n'|| c1=='N')
                    {   
                        if(c2=='y' || c2=='Y')
                        {
                            write(client2,&c1,sizeof(char));
                            strcpy(text2,"Ne pare rau, partida de joc se incheie aici. Partenerul dvs de joc a parasit jocul. O zi buna!:)\n\0");
                            lungime_text2=strlen(text2)+1;
                            write(client2,&lungime_text2,sizeof(int));
                            write(client2,text2,lungime_text2);
                            break;
                        }
                        else{break;}
                    }
                    else{
                        if(c2=='n' || c2=='N')
                        {
                            write(client1,&c2,sizeof(char));
                            strcpy(text2,"Ne pare rau, partida de joc se incheie aici. Partenerul dvs de joc a parasit jocul. O zi buna!:)\n\0");
                            lungime_text2=strlen(text2)+1;
                            write(client1,&lungime_text2,sizeof(int));
                            write(client1,text2,lungime_text2);
                            break;  
                        }
                        else
                        {
                            write(client1,&c1,sizeof(char));
                            write(client2,&c1,sizeof(char));
                            
                        }
                    }
                }
                close(client1);
                close(client2);
                exit(10);
            }
        }


    }
}
