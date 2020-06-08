#include "start.c"

// If any problem occur enter clear

int out;
int save_out;
bool ofileused = 0; 	
void checkforRedirectino( char command[100] );
void check_if_to_run_in_backgroung(char command[100]);
bool runbackground = 0;
int wfor;
int main(int argc, char *argv[] , char * envp[]) {
	
	signal(SIGINT, SIG_IGN);// This is to ignore sigint command; 
	struct data d;
	char command[50];
	
    while(1){
		//displaying name,user , working directory
    	fprintf(stdout, "\033[0;32m");
    	fprintf(stdout, "%s@", d.user);
    	fprintf(stdout, "%s:", d.host);
    	fprintf(stdout, "\033[0;34m");
    	fprintf(stdout, " %s ", d.cwd);
 		fprintf(stdout, "\033[0;0m");
    	fprintf(stdout, " > ");

    	//setting env variable
 		char e[200] = "SHELL=" ;
 		strcat( e,d.cwd );
 		strcat(e,"/");
 		strcat(e,"gbsh");
 		putenv(e);

 		//getting command from user
        scanf("%[^\n]s ",command);
        // try using & at end without any space
        check_if_to_run_in_backgroung(command);

        // executing entered command
        if ( ! strncmp(command,"exit",4)  )
    		exit(0);
    	
    	else if ( ! strncmp(command,"pwd",3) ){
    		checkforRedirectino(command);
    		printf("%s\n", d.cwd);

    	}else if ( ! strcmp(command,"clear") )
    		system("clear");
        
        else if ( ! strncmp (command,"ls",2) ){
        	checkforRedirectino(command);
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
	    
	    }else if ( ! strncmp( command,"environ",3) ){
        	checkforRedirectino(command);
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
        
        }else {// part d , e and g are implemeted here(all commands other than above will go here)
            
           	wfor = fork();
            if ( wfor == 0 ){ // child proc.
                
                // to set the envirnment variable as required
                char str[100] = "parent=";
                strcat( str,d.cwd );
                strcat(str,"/gbsh");
                putenv(str);
                
                char delim[] = " ";
                char *ptr = strtok(command, delim);
                int c = 0 ;
                char parameters[100][100]; 
                int rw[2];
                int p = pipe(rw);
                
                if ( p == -1 )
                	printf("pipe not created\n");
                
                while( ptr != NULL ){  // This will read the command word by word and try to find any redirection and piping
                    if ( !strncmp(ptr,">",1)  ){
                        ptr = strtok(NULL,delim); 
                        ofileused = 1;
						out = open(ptr, O_RDWR|O_CREAT, 0600);
						save_out = dup(fileno(stdout)); // saving the copy of standard output buffer
						dup2(out, fileno(stdout));                    			           
                    }
                    else if ( !strncmp(ptr,"<",1)  ){
                        ptr = strtok(NULL,delim); 
                        int ifile = open( ptr,O_RDONLY );
                        dup2 ( ifile,STDIN_FILENO);
                    }
                    else if ( !strncmp(ptr,"|",1) ){
                    	//This part will allow dynamic chaining of multiple pipes.
                    	//chainlength ++ ;
                    	if ( fork() == 0 ){
                    		
                			close(rw[0]);
                			dup2(rw[1],STDOUT_FILENO);//redirecting
                

                    		if (c == 1){
			                    char * args[] = { parameters[0],NULL };
			                    execvp( args[0],args );
			                    printf("command not found\n"); 
            				  	for ( int i = 0 ; i < c ; i++ ) printf("%s\n",parameters[i] );

			                }else if ( c == 2 ){
			                    char * args[] = { parameters[0],parameters[1],NULL };
			                    execvp( args[0],args );      
			           	    	printf("command not found\n"); 
							  	for ( int i = 0 ; i < c ; i++ ) printf("%s\n",parameters[i] );


			                }else if ( c == 3 ){
			               		char * args[] = { parameters[0],parameters[1],parameters[2],NULL };
			                    execvp( args[0],args );      			                	
			                    printf("command not found\n");                
			                  	for ( int i = 0 ; i < c ; i++ ) printf("%s\n",parameters[i] );

			                }
                    	
                    	}else{
                    		wait(NULL);
                    		c = 0;
							close (rw[1]);
							dup2(rw[0],STDIN_FILENO);//redirecting again                			
                    	}

                    	
                    }



                    else{
                        strcpy( parameters[c],ptr );
                        c++;
                    }

                    ptr = strtok(NULL,delim);
                }
                
                if (c == 1){ // final command will run here in case of piping 
                    char * args[] = { parameters[0],NULL };
                    execvp( args[0],args );
                    printf("command not found\n"); 
                	for ( int i = 0 ; i < c ; i++ ) printf("%s\n",parameters[i] );

                }else if ( c == 2 ){
                    char * args[] = { parameters[0],parameters[1],NULL };
                    execvp( args[0],args );      
               		printf("command not found\n"); 
          		  	for ( int i = 0 ; i < c ; i++ ) printf("%s\n",parameters[i] );

                
                }else if ( c == 3 ){
               		char * args[] = { parameters[0],parameters[1],parameters[2],NULL };
                    execvp( args[0],args );      			                	
                	printf("command not found\n");                
	        	  	for ( int i = 0 ; i < c ; i++ ) printf("%s\n",parameters[i] );

                }
              
              
             


            }else{// To avoid Zombie process
               if ( /* true || */runbackground == 0){ // uncomment the part
               		int s;
                	waitpid(wfor, &s,NULL);
            	 	
                }
               else
            		printf("Trying to run command in background\n" ),runbackground = 0;
            }


        }// part d, e and g are ending here.

    	

    	// This proc will direct output back to standard consol if file used for output
    	if ( ofileused == 1 ){
			dup2(save_out, fileno(stdout));
			close(save_out);
			ofileused = 0;
    		fflush(stdout); close(out);    
    	}

        while ((getchar()) != '\n'); // to clear input buffer
        command[0] = '\0';// to set string to NULL
    }

	


	exit(0); // exit normally	    
}









// This procedure will check output redirection in ls,environ and pwd and direct if required
void checkforRedirectino( char command[50] ){
	char delim[] = " ";
	char *ptr = strtok(command, delim);
	while ( ptr != NULL ){
		if ( !strncmp(ptr,">",2) ){
			ptr = strtok(NULL,delim); 
			ofileused = 1;
			out = open(ptr, O_RDWR|O_CREAT, 0600);
			save_out = dup(fileno(stdout));
			dup2(out, fileno(stdout));
		}
		ptr  = strtok(NULL,delim);
	}
}


// try using & at end without any space
void check_if_to_run_in_backgroung(char command[100]){
 	if (!strncmp( command + (	strlen(command) - 1 ),"&",1 ))
 		command[strlen(command) - 1] = '\0',runbackground = 1;
}

