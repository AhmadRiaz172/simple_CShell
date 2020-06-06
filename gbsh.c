#include "start.c"
#include <cstring>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char *argv[] , char * envp[]) {

	struct data d;
	char command[50];
	
    while(1){

    	fprintf(stdout, "\033[0;32m");
    	fprintf(stdout, "%s@", d.user);
    	fprintf(stdout, "%s:", d.host);
    	fprintf(stdout, "\033[0;34m");
    	fprintf(stdout, " %s ", d.cwd);
 		fprintf(stdout, "\033[0;0m");
    	fprintf(stdout, " > ");
 		char e[200] = "SHELL=" ;
 		strcat( e,d.cwd );
 		strcat(e,"/");
 		strcat(e,"gbsh");
 		putenv(e);
        scanf("%[^\n]s ",command);
        if ( ! strcmp(command,"exit")  )
    		exit(0);
    	else if ( ! strcmp(command,"pwd") )
    		fprintf(stdout, "%s\n", d.cwd);
		else if ( ! strcmp(command,"clear") )
    		system("clear");
        else if ( ! strcmp (command,"ls") ){
            struct dirent *entry;
            DIR *dir = opendir(d.cwd);
            while ((entry = readdir(dir)) != NULL ) {
                if ( !strcmp(entry->d_name,".") || !strcmp(entry->d_name,"..") )
                    continue;
                printf("%s\n",entry->d_name ); 
            }
            closedir(dir);      
        }else if ( ! strncmp( command , "cd " ,3) ){
            if(!d.updateCWD(command+3))
                printf("File/Directory not found.\n");
        }else if ( !strcmp(command,"cd") ){
            d.navigateToHome();
	    }else if ( ! strcmp( command,"environ") ){
        	for (short i = 0; envp[i] != NULL; i++)
    		    printf("\n%s", envp[i]);
    		printf("\n");
        }else if ( ! strncmp(command,"setenv ",7) ){
        	char f[2] = " ";
        	char p[200] ;
        	p[0] = '\0';
        	char * token = strtok( command +7, f );
        	if ( token == NULL ){
        		printf("Too few parameters for this function.\n");
        		continue;
        	}


        	strcat(p,token);
        	strcat(p,"=");
        	token = strtok( NULL, f  );
        	
        //	printf("%d\n",overwrite );
        	if ( token != NULL )
      			strcat(p,token);
        
      		if (putenv(p) != 0)
      			printf("Shell unable to execute the command\n");
      		printf("%s\n",p);

        }else if( !strncmp(command,"unsetenv ",9) ){
			
			char * token = strtok( command +9, " " );
        	if ( token == NULL ){
        		printf("Too few parameters for this function.\n");
        		continue;
        	}
        	unsetenv(token);
        }else {// part d and e are implemeted here
            
            if ( fork() == 0 ){ // child proc.
                
                // to set the envirnment variable as required
                char str[100] = "parent=";
                strcat( str,d.cwd );
                strcat(str,"/gbsh");
                putenv(str);
                
                char delim[] = " ";
                char *ptr = strtok(command, delim);
                int c = 0 ;
                char parameters[100][100]; 



                while( ptr != NULL ){  // This will read the command line by line and try to find any redirection
                    if ( !strncmp(ptr,">",1)  ){
                        ptr = strtok(NULL,delim); 
                        int ofile = open( ptr,O_WRONLY| O_CREAT);
                        dup2 ( ofile,STDOUT_FILENO);

                    }
                    else if ( !strncmp(ptr,"<",1)  ){
                        ptr = strtok(NULL,delim); 
                        int ifile = open( ptr,O_RDONLY );
                        dup2 ( ifile,STDIN_FILENO);
                    }else{
                        strcpy( parameters[c],ptr );
                        c++;
                    }

                    ptr = strtok(NULL,delim);
                 
                }
                            
                if (c == 1){
                    char * args[] = { parameters[0],NULL };
                    execvp( args[0],args ); 
                }else if ( c == 2 ){
                    char * args[] = { parameters[0],parameters[1],NULL };
                    execvp( args[0],args );      
                }

            }else{// To avoid Zombie process
                wait(NULL);
            }


        }// part d and e ending.









        while ((getchar()) != '\n'); // to clear input buffer
        command[0] = '\0';// to set string to NULL
    }

	// shell code here
	
	// ...
	
	// ...

	exit(0); // exit normally	
        
}
