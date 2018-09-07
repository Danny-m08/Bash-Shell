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
 
 char user[33];			//max linux user length is 32
 char  machine[HOST_NAME_MAX];	//max machine name
 char cwd[PATH_MAX + 1];	//max Path length
 int len;

 strcpy( machine, getenv("MACHINE"));		//get machine string
 strcpy(user, getenv("USER"));			//get user string
 getcwd(cwd, PATH_MAX);				//get current working directory
 len  = strlen(user) + strlen(machine) + strlen(cwd) + 10;

 char *prompt = malloc( len * sizeof(char) );	//Allocate space for the prompt string
 
 strcpy(prompt, user);
 strcat(prompt,"@");
 strcat(prompt,machine);
 strcat(prompt," :: ");
 strcat(prompt,cwd);
 strcat(prompt," ->");
 printf("%s", prompt);
 free(prompt);
 return;
}

//This function takes an array of pointers and an array of chars
//Parses char array and adds address of each token to array of pointers
//
//Written by Daniel

void tokenize( char ** arg, char *line){		//Function still needs work
 
 int i = 0;
 int it = 0;
 while( line[i] != '\0' ){
	if( !isspace( line[i] ) && line[i] != '\'' && line[i] != '\"'){
		arg[it] = &line[i]; 
		it ++;
		i ++;
		while( !isspace( line[i] ) )
			++i;	
		}

	else if( line[i] == '\'' ){
		++i;
		if (line[i] != '\''){
			arg[it] = &line[i];
			++i;
			++it;
			while(line[i] != '\''){
				if(line[i] == '\n'){
					printf( "Unmatched '.\n");
					return;
					}
				else ++i;
				}
			}
		++i;
		}
	else ++i;
	}
 }
		
//Returns the number of tokens in command
//
//Written by Daniel

int token_count(char * line){			//Not returning the correct number
	int it = 0;				//still needs work
	int count = 0;
	while(line[it] != '\0'){
	 if (line[it] == '\''){
		++count;
		++it;
		while(line[it] != '\'')
			++it;
		++it;
		}

	 else if (line[it] == '\"'){
                ++count;
                ++it;
                while(line[it] != '\"')
                        ++it;
                ++it;
                }

	 else if( !isspace( line[it] ) ){
		++count;
		++it;
		while( !isspace( line[it] ) )
			it++;
		++it;
		}

	 else ++it;
	 }
 return count;
 }
			

int main(){
  
	char command[256];
	char **argv = {NULL};
	int x;

 while(1){
	prompt();
	strcpy(command,readline(" "));
 	x = 0;
	
	printf("%i\n", token_count(command));
	printf("%i\n", token_count(command));
	argv = malloc( token_count(command) * sizeof(char*) );	
		
	tokenize(argv,command);
	//print(argv);
	
	if( strcpy(command, "exit") )
		exit(EXIT_SUCCESS);
	
	}






}

