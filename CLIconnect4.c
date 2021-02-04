#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include<arpa/inet.h>
#include <termios.h>


extern int errno;
int port;

void afisare_tabla(int tabla_joc[6][7])
{
    printf("\n");

    for(int i=-1;i<6;i++)
    {
        for(int j=0;j<7;j++)
        {
          if(i==-1)
          {
            printf("\033[1;34m");
            printf("%d ",j+1);
            printf("\033[0m");
          }
          else
          {
            if(tabla_joc[i][j]==1)
            {
              printf("\033[1;31m"); 
              printf("%d ",tabla_joc[i][j]);
              printf("\033[0m"); 
            }
            else{
              if(tabla_joc[i][j]==2)
              {
                printf("\033[1;33m");
                printf("%d ",tabla_joc[i][j]);
                printf("\033[0m");  
              }else{
              printf("%d ",tabla_joc[i][j]);
              }
            }
          }
        }
        printf("\n");
    }

    
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

void initializare_tabla_joc(int tabla_joc[6][7])
{
  for(int i=0;i<6;i++)
    {
        for(int j=0;j<7;j++)
        {tabla_joc[i][j]=0;}
    }
}
int valid(int mutare, int tabla_joc[6][7])
{
    if(mutare==0)
    {
        //jucatorul vrea sa incheie jocul;
        return -1;
    }
    else
    {
        if(mutare>0 && mutare<8)
        {
		for(int i=0;i<6;i++)
		{
			if(tabla_joc[i][mutare-1]==0)
				return 1;
		}
		return 0;
	}
        else
            return 0;
    }
}

void clrscr()
{
    system("clear");
}

void clean_stdin(void)/*functie preluata de la https://stackoverflow.com/questions/17318886/fflush-is-not-working-in-linux/23885008#23885008?newreg=45e52b6e2c7e450dabaa2f5504b79a1c */
{
  int stdin_copy = dup(STDIN_FILENO);
  tcdrain(stdin_copy);
  tcflush(stdin_copy, TCIFLUSH);
  close(stdin_copy);
}

int main (int argc, char *argv[])
{ 
  int sd;		
  struct sockaddr_in server;	 
  char text_informatitv[1000];
  int lungime_text_informativ;

  char username[100];
  int lungime_username;
  int mutare;	
  int tabla_joc[6][7]={0};	
  int culoare;
  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }


  port = atoi (argv[2]);


  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }


  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons (port);
  
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }

  read(sd,&lungime_text_informativ,sizeof(int));
  read (sd, text_informatitv,lungime_text_informativ);
  
  printf ("%s\n", text_informatitv);

  scanf("%s",username);
  username[strlen(username)]='\0';
  lungime_username=strlen(username);

  write (sd, &lungime_username, sizeof(int));
  write(sd,username,lungime_username);

  strcpy(text_informatitv,"");
  read (sd, &lungime_text_informativ,sizeof(int));
  read(sd,text_informatitv,lungime_text_informativ);
  printf ("%s\n", text_informatitv);
 
  char aux[200];
  strcpy(aux,"Nu s-a gasit niciun partener de joc pentru dvs. Invitati un prieten la joc!");

  if(strcmp(text_informatitv,aux)==0)
  {
    strcpy(text_informatitv,"");
    read (sd, &lungime_text_informativ,sizeof(int));
    read(sd,text_informatitv,lungime_text_informativ);
    printf ("%s\n", text_informatitv);
  }

  read(sd,&culoare,sizeof(int));
  sleep(7);
  int ok;
  char c;
  while(1)
  { 
	clrscr();
      	initializare_tabla_joc(tabla_joc);
      	if(culoare==1)
      	{
        	printf("Este randul dvs. Indroduceti mutarea 1-7 (sau 0 pentru parasirea jocului)...");
      	}
      	else{
        	printf("Asteptati mutarea partenerului de joc...");
      	}
  	afisare_tabla(tabla_joc);
      	while(1)
      	{
        
        	if(culoare==1)
        	{
         		while(scanf("%d", &mutare)!=1)
          		{
            			while(getchar()!='\n');
          		}

          		while((ok=valid(mutare,tabla_joc))!=1)
          		{
            			if(ok==-1)
            			{
                		//vreau sa inchid jocul;
                			break;
            			}
            			while(scanf("%d", &mutare)!=1)
            			{
              				while(getchar()!='\n');
            			}
          		}
          		if(ok==-1)//mutarea e 0, jucatorul abandoneaza jocul
          		{
            			write(sd,&mutare,sizeof(int));   
            			break;
          		}
          		
			write(sd,&mutare,sizeof(int));
			actualizare_tabla(tabla_joc, mutare,culoare);
          		
			clrscr();
          		printf("Asteptati mutarea partenerului de joc...");
          		afisare_tabla(tabla_joc);

          		read(sd,&mutare,sizeof(int));
          		
			if(mutare==0) //celalat jucator a abandonat 
         		{
            			//astept mesaj de la server
            			strcpy(text_informatitv,"");
            			read(sd,&lungime_text_informativ,sizeof(int));
            			read(sd,text_informatitv,lungime_text_informativ);
            			printf("%s\n",text_informatitv);
            			break;
          		}
          		else{
            			if(mutare==91)
            			{
            				strcpy(text_informatitv,"");
            				read(sd,&lungime_text_informativ,sizeof(int));
            				read(sd,text_informatitv,lungime_text_informativ);
              				printf("%s\n",text_informatitv);  
			  	   	do{ 
                				while(scanf(" %c", &c)!=1)
                				{	 
                    					while(getchar()!='\n');
                				}
             				}while(c!='y' && c!='n' && c!='Y' && c!='N' );
          
         		  		write(sd,&c,sizeof(char));
              				break;
            			}
            			else{
              				if(mutare==92)
              				{
                				read(sd,&mutare,sizeof(int));
                				actualizare_tabla(tabla_joc,mutare,2);
                				clrscr();
                				afisare_tabla(tabla_joc);
                				strcpy(text_informatitv,"");
                				read(sd,&lungime_text_informativ,sizeof(int));
                				read(sd,text_informatitv,lungime_text_informativ);
                				printf("%s\n",text_informatitv);
				        	do{
            	  					while(scanf(" %c", &c)!=1)
            	  					{	 
              							while(getchar()!='\n');
            	  					}
                				}while(c!='y' && c!='n' && c!='Y' && c!='N' );    
         		    			write(sd,&c,sizeof(char));
                				break;
              
              				}
            			}
          		}
         		actualizare_tabla(tabla_joc,mutare,2);
          		clrscr();
          		printf("Este randul dvs. Indroduceti mutarea 1-7 (sau 0 pentru parasirea jocului)...");
          		afisare_tabla(tabla_joc);
          		clean_stdin();
		}
        	else
        	{
          		read(sd,&mutare,sizeof(int));
          		if(mutare==0) //celalat jucator a abandonat 
          		{
            			//astept mesaj de la server
            			text_informatitv[0]='\0';
            			read(sd,&lungime_text_informativ,sizeof(int));
            			read(sd,text_informatitv,lungime_text_informativ);
            			printf("%s\n",text_informatitv);
            			break;
          		}
          		if(mutare==92)
            		{
              			text_informatitv[0]='\0';
              			read(sd,&lungime_text_informativ,sizeof(int));
              			read(sd,text_informatitv,lungime_text_informativ);
              			printf("%s\n",text_informatitv);
              			do{
            				while(scanf(" %c", &c)!=1)
            				{	 
              					while(getchar()!='\n');
            				}
              			}while(c!='y' && c!='n' && c!='Y' && c!='N' );
         			write(sd,&c,sizeof(char));
              			break;
            		}
            		else{
              			if(mutare==91)
              			{
                			read(sd,&mutare,sizeof(int));
                			actualizare_tabla(tabla_joc,mutare,1);
                			clrscr();
                			afisare_tabla(tabla_joc);
                			strcpy(text_informatitv,"");
                			read(sd,&lungime_text_informativ,sizeof(int));
                			read(sd,text_informatitv,lungime_text_informativ);
                			printf("%s\n",text_informatitv);
                			do{
            	  				while(scanf(" %c", &c)!=1)
            	 				{	 
              						while(getchar()!='\n');
            	  				}
              				}while(c!='y' && c!='n' && c!='Y' && c!='N' );
         				write(sd,&c,sizeof(char));
                			break;
              			}
            		}
          		actualizare_tabla(tabla_joc,mutare,1);
          		clrscr();
          		printf("Este randul dvs. Indroduceti mutarea 1-7 (sau 0 pentru parasirea jocului)...");
          		afisare_tabla(tabla_joc);
          		clean_stdin();
          		while(scanf("%d", &mutare)!=1)
          		{
              			while(getchar()!='\n');
          		}
          		int ok;
          		while((ok=valid(mutare,tabla_joc))!=1)
          		{
            			if(ok==-1)
              			{
              				//vreau sa inchid jocul;
              				break; 
              			}
              			while(scanf("%d", &mutare)!=1)
              			{
              				while(getchar()!='\n');
              			}

          		}
            		if(ok==-1)
            		{
                		write(sd,&mutare,sizeof(int));
                		break; 
            		} 
            		write(sd,&mutare,sizeof(int));
            		actualizare_tabla(tabla_joc,mutare,culoare);
            		clrscr();
            		printf("Asteptati mutarea partenerului de joc...");
            		afisare_tabla(tabla_joc);
        	}
      }
      if(mutare==0)
      {
      	break;//unul dintre jucatori a abandonat;
      }
      if(c=='y' || c=='Y')
      {
            read(sd,&c,sizeof(char));
            if(c=='n'||c=='N')
            {
              read(sd,&lungime_text_informativ,sizeof(int));
              read(sd,text_informatitv,lungime_text_informativ);
	      printf("%s",text_informatitv);
              break;
            }
        }
        else
        {break;}
        
      
 }
  /* inchidem conexiunea, am terminat */
 close (sd);
}
