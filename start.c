#ifndef START
#define START
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <iostream>
#include <stdio.h>
#include <cstring>

struct data{
	
	char host[20];
    char cwd[PATH_MAX];
    char *user;
    
    data(){
    	user=getenv("USER");
	    gethostname( host,sizeof(host) );
    	getcwd( cwd,sizeof(cwd)) ;
    }
    
    bool updateCWD(const char * loc){
    	if (chdir(loc) == 0 ){
    		getcwd( cwd,sizeof(cwd)) ;
    		return 1;
    	}
    	return 0;
    }

    void navigateToHome(){
    	while( strcmp( cwd+(strlen(cwd)-strlen(user)) , user ) )
    		chdir(".."),getcwd( cwd,sizeof(cwd)) ;		
    }


};

#endif