#define _GNU_SOURCE
#define __USE_POSIX
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <unistd.h>
#include<limits.h>



//This function produces the prompt
//For every time input is expected
//from within the shell.
//
//Written By Daniel

void prompt( ){
 
 char user[33];					//max linux user length is 32
 char  machine[HOST_NAME_MAX + 1];			//max machine name
 char cwd[PATH_MAX + 1];			//max Path length
 int len;

// strcpy( machine, getenv("MACHINE"));		//get machine string
 strcpy(user, getenv("USER"));			//get user string
 getcwd(cwd, PATH_MAX);				//get current working directory
 len  = 32 + HOST_NAME_MAX + PATH_MAX + 10;

 char *prompt = malloc( len * sizeof(char) );	//Allocate space for the prompt string
 
 strcpy(prompt, getenv("USER"));
 strcat(prompt,"@");
 strcat(prompt,getenv("MACHINE"));
 strcat(prompt," :: ");
 strcat(prompt,getcwd(cwd,PATH_MAX));
 strcat(prompt," ->");
 printf("%s", prompt);
 free(prompt);
 return;
}

//This function takes an array of pointers and an array of chars
//Parses char array and adds address of each token to array of pointers
//returns the number of tokens in the argument
//
//Written by Daniel
//I would like to point out that I wrote a mjority of this function
//for a similar project in UnixTools
//But the code is 100% mine

int tokenize( char ** arg, char *line){	
 int i = 0;
 int it = 0;
 char temp;
 while( line[i] != '\0' ){
 	if ( !isspace( line[i] ) && line[i] != '\'' && line[i] != '\"'){
        	arg[it] = &line[i];      
                ++it;
                ++i;
                while( i < 50 ){
                	if( isspace( line[i])){
                        	line[i] = '\0';
                                 break;
                                 }
                         else if( line[i] == '\0' )
                                 return it;
                                 ++i;
                         }
                }
        else if( line[i] == '\''){
                 ++i;
                 arg[it] = &line[i];
                 ++it;
                 while( 1 ){
        	         if( line[i] == '\'' ){
                	         line[i] = '\0';
                                 break;
                                 }
			 else if( line[i] == '\0' ){
				printf("Unmatched '.\n");
				return -1;
				}
                          ++i;
                          }
                 }
		else if( line[i] == '\"'){
                        ++i;
                        arg[it] = &line[i];
                        ++it;
                        while( 1 ){
                                if( line[i] == '\"' ){
                                        line[i] = '\0';
                                        break;
                                        }
				else if( line[i] == '\0' ){
                                	printf("Unmatched \".\n");
                                	return -1;
					}
                                ++i;
                                }
                        }
                ++i;
                }
        return it;

 }
		


int main(){
  
	char command[256];
	char ** argv;
	int x;

 while(1){
	prompt();
	strcpy(command,readline(" "));
 	//printf("%s\n",command);
	if( !strcmp(command, "exit") )
		break;


	printf("#%s#\n",command);
	argv = malloc( 10 * sizeof(char*) );	
	for(x = 0; x < 10; ++x)				//loop to intialize pointers to NULL
		argv[x] = NULL;
	
	x = tokenize(argv,command);			
	printf("%i\n",x);				
	
	//for(x = 0; argv[x] != NULL; ++x)		//loop to print arguments
	//	printf("#%s# ", argv[x]);
	//printf("\n");
		//print(argv);
	

	
	}

 exit(EXIT_SUCCESS);






}

