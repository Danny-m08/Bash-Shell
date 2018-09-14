#define _GNU_SOURCE
#define __USE_POSIX
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <sys/wait.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <signal.h>
//#include <ctype.h>
#include <unistd.h>
#include<limits.h>
#include <stdbool.h>
#include <dirent.h>

//This function produces the prompt
//For every time input is expected
//from within the shell.
//
//Written By Daniel

void prompt( ){
 int len;
 len  = 32 + HOST_NAME_MAX + PATH_MAX + 10;

 char *prompt = malloc( len * sizeof(char) );	//Allocate space for the prompt string
 
 strcpy(prompt, getenv("USER"));
 strcat(prompt,"@");
 strcat(prompt,getenv("MACHINE"));
 strcat(prompt," :: ");
 strcat(prompt,getcwd(NULL,PATH_MAX));
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
                while( 1 ){
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
  
	
	//take this first timevalue, store in struct	
	struct timeval  first, second;
        gettimeofday(&first, NULL);


        char command[256];
        char ** argv;
        int x, status;
	pid_t child_id, temp_id;
        
	




 while(1){
        prompt();
        strcpy(command,readline(" "));

	if( !strcmp(command, "exit") )
        {
                gettimeofday(&second, NULL);		//before, ending, take second time value and put in struct
                printf("Exiting...\n\tSession time: %ds\n", (int) (second.tv_usec - first.tv_usec) / 1000000 +
         (int) (second.tv_sec - first.tv_sec));		//get difference between adn first and second struct
                break;
        }
                
	// WRITTEN BY KOREN COLE  
       if(strstr(command, "echo"))		// covers echo built in and env variables
        {
                char * echoArg;
                if(echoArg = strchr(command, '$'))                      //if the argument has a $
                {

                        echoArg++;              // remove the $
                        if(getenv(echoArg) != NULL)                     // check to see if environmental variable is valid
                        {
                                char* envVar = malloc(sizeof(getenv(echoArg)));
                                strcpy(envVar, getenv(echoArg));
                                printf("%s\n", envVar);
                                free(envVar);
                        }
                        else
                                printf("Argument does not exist\n");
                }
                else                                                    // if no $ in argument
                {
                        echoArg = strchr(command, ' ');
                        printf("No $: %s \n", echoArg);
                }
        }





        argv = malloc( 10 * sizeof(char*) );            //Still haven't figured out max # of tokens

	

 
       for(x = 0; x < 10; ++x)
		argv[x] = NULL;

        x = tokenize(argv,command);                     
	



	char * add;
	bool valid_directory = false;
	if((strcmp(argv[0], "cd") == 0) && (x > 1))
	{
		DIR* dir = opendir(argv[1]);		//open directory
		if (dir)				//if it can open 
			valid_directory = true;
	}
	if(x > 2)					//more than 2
        {
                printf("Error: Too many arguments\n");
                continue;
	}

	else if((strcmp(argv[0], "cd") == 0) && (x == 1)){	//just cd
                char* home = getenv("HOME");
                chdir(home);
                continue;
        }
	else if((strcmp(argv[0], "cd") == 0) && ( valid_directory == false))	//invalid directory 
        {
                printf("Not a directory\n");
                continue;
        }

		
	else if((strcmp(argv[0], "cd") == 0) && (x > 1)){		//valid directory 
            	 char* home = getenv("HOME");
		add = strcat(home, "/");
		add = strcat (home, argv[1]);
		chdir(add);
		continue;
			
	}



        if( (child_id = fork() ) == 0 ){
             
                execvp(argv[0], argv);				//code executed by child process
                printf("Unknown command %s\n", argv[0] );	//
                }


	while(1){
							//wait for child process to return
		x = waitpid(child_id, &status, 0);	//parent process code`
		if( x == child_id)
			break;
		}
        continue;
	}

 exit(EXIT_SUCCESS);






}

