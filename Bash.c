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
#include<string.h>
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
 
	if ( !isspace( line[i] ) && line[i] != '\'' && line[i] != '\"' ){
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
                 arg[it] = &line[i];
		 ++i;
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
               
                arg[it] = &line[i];
		++i;               
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
//Written by Koren Cole
int isCommand(char ** argv, int i)
{

        printf("argv[i]: %s \n", argv[i]);
        // how to find the type 
        if(strcmp(argv[i], "cd") == 0)
                return 1;
        else if(strcmp(argv[i], "echo") == 0 || strcmp(argv[i], "io") == 0 || strcmp(argv[i], "exit") == 0 )
                return 2;
        else if(i >= 1)
        {
                return 0;               // its an argument 
        }
        else
                return 3;               // external command
}

char *expandPath(char *path, int cmd)
{
        char * tempPath; // = malloc(strlen(path) * sizeof(char));
        char * envPath;
        char *temp;
        char *varLocation;
        char *expPath;
        bool commandFound = false;
        struct stat buffer;
        int exists;
        size_t tempSize;
        //expand the environmental variables
        if((envPath = strstr(path, "$")) != NULL)
        {
                envPath++;              //remove the $
                temp = strchr(envPath,'/');
                tempSize = strlen(envPath) - strlen(temp);
                memcpy(varLocation, envPath, tempSize);
                varLocation[tempSize]='\0';
                tempPath = getenv(varLocation);
                strcat(tempPath, temp);
                strcpy(path,tempPath);
        }
        if((expPath = strstr(path, "..")) != NULL)
        {
                tempPath = getenv("PWD");
                printf("temp path: %s\n", tempPath);
                // if just .. 
                if(strlen(path) <=2)
                {
                        printf("just ..\n");
                        temp = strrchr(tempPath,'/');
                        printf("this is var temp: %s\n", temp);
                        tempSize = strlen(tempPath) - strlen(temp);
                        printf("tempsixe: %i \n", tempSize);
                        varLocation = malloc(tempSize +1);
                        strncpy(varLocation, tempPath, tempSize);
                        varLocation[tempSize] = '\0';
                        printf("this is varLocation: %s\n", varLocation);
                        strcpy(path,varLocation);
                        free(varLocation);
                }
	if((expPath = strstr(path, ".")) != NULL)
		{
			//printf("relative to curr directory");
			expPath++;                      //gets rid of ./ at the beginning 
			//printf("this is exp Path: %s \n", expPath);
			tempPath = malloc(strlen(getenv("PWD")));
			strcpy(tempPath, getenv("PWD"));
			tempPath = realloc(tempPath, strlen(tempPath) + strlen(expPath));
			strcat(tempPath, expPath);
			//printf("this is temppath: %s \n", tempPath);
			strcpy(path, tempPath);
			free(tempPath);
		}
		
	//expand the ~ 
        if((expPath = strstr(path, "~")) != NULL)
        {
                printf("contains ~ \n");
                if(expPath == path)                     // if ~ at the beginning of the argument
                {
                        printf("at beginning of path\n");
                        expPath++;                      // remove ~ 
                        tempPath = malloc(strlen(getenv("HOME")));
                        strcpy(tempPath, getenv("HOME"));
                        tempPath = realloc(tempPath, strlen(tempPath) + strlen (expPath));
                        strcat(tempPath, expPath);
                        strcpy(path, tempPath);
                        free(tempPath);
                }
                else
                        printf("Incorrect syntax of \'~\' \n");
        }



        switch(cmd)
        {
                case 0 :                                                        //this is an arg
                        printf("Case 0, its an argument %s \n", path);
                        return path;
                        break;
                case 1:                                                         // this is CD
                        printf("Case 1, its a cd %s \n", path);
                        return path;
                        break;
                case 2:                                                         // this is built in 
                        printf("Case 2, its built in %s \n", path);
                        return path;
			break;
                case 3:
                        // need to loop through each case and see if its there or not
                        // have to find in path
                        tempPath = malloc(strlen(getenv("PATH")));
                        strcpy(tempPath,getenv("PATH"));
                        //printf("tempPath: %s\n", tempPath);
                        while(!commandFound)
                        {
                                varLocation = strchr(tempPath, ':');
                                tempSize = strlen(tempPath) - strlen(varLocation);
                                varLocation++;                          // takes out this colon
                                //printf("varLocation: %s\n", varLocation);
                                temp = calloc(tempSize + strlen(path) + 2, sizeof(char));
                                //printf("malloc vs strlen(should be zero): %i vs %i",tempSize + strlen(path) +2, strlen(temp));
                                strncpy(temp, tempPath,tempSize);
                                strcat(temp,"/");
                                strcat(temp, path);
                                strcat(temp, "\0");
                        // check if its here, otherwise start again 
                                //printf("find seg fault: %s  \n", temp);
                                if(stat(temp, &buffer) == 0 && buffer.st_mode & S_IXUSR)
                                {
                                        commandFound = true;
                                        //strcpy(path,temp);
                                        //free(temp);
                                        //tempPath = "\0";
                                        //printf("path: %s \n", tempPath); 
                                        //return path; 
                                }
                                else
                                {
                                        commandFound = false;
                                        if(strlen(varLocation) < 2)
                                        {
                                                printf("Command not found. \n");
                                                return path;
                                        }
                                        strcpy(tempPath, varLocation);
                                        //printf("temp path after false: %s \n", tempPath);
                                        free(temp);
                                }
                        }
                        if(commandFound == true)
                        {
                                free(tempPath);
                                return temp;
                        }
                        break;

                default:
                        printf("Case default %s \n", path);
        }
}
char ** pathRes(char ** args)
{
        int i;
        for(i=0; args[i] != NULL; i++)
        {
                args[i] = expandPath(args[i], isCommand(args, i));
                printf("this is args[%i]: %s \n", i, args[i]);
        }
        return args;

}


int main(){
  
	
	//take this first timevalue, store in struct	
	struct timeval  first, second;
        gettimeofday(&first, NULL);


        char command[256];
        char ** argv;
        int x, status;
	pid_t child_id, temp_id;
        
	

        argv = malloc( 10 * sizeof(char*) );            //Still haven't figured out max # of tokens

	
 while(1){
        
	prompt();
        strcpy(command,readline(" "));
	

	if( !strcmp(command,"" ) )
		continue;

      for(x = 0; x < 10; ++x)
		argv[x] = NULL;

        x = tokenize(argv,command);                     
	//for(x=0; argv[x] != NULL; ++x )
	//	printf("%s\n", argv[x]);

	argv = pathRes(argv);
	 
	 
	if( !strcmp(command, "exit") )
        {
                gettimeofday(&second, NULL);		//before, ending, take second time value and put in struct
                printf("Exiting...\n\tSession time: %ds\n", (int) (second.tv_usec - first.tv_usec) / 1000000 + (int) (second.tv_sec - first.tv_sec));
		//get difference between adn first and second struct
                break;
        }
                

	// WRITTEN BY KOREN COLE  
       else if( !strcmp("echo",argv[0]) ){		// covers echo built in and env variables
    	
	bool break_ = false;
	for(x = 1; argv[x] != NULL; ++x){
                 if(argv[x][0] == '$'){
                         if( getenv(&argv[x][1]) == NULL ){
                                 printf("%s: Undefined variable.\n",&argv[x][1]);
                         	 break_ = true;
			        break;
                                 }
			}
		}
	if( break_ ) continue;

       	for(x = 1; argv[x] != NULL; ++x){
		if(argv[x][0] == '$'){
			
			printf("%s ", getenv(&argv[x][1]));
			}
		else if(argv[x][0] == '\'')
			printf("%s ", &argv[x][1]);

		else if(argv[x][0] == '\"'){
			char * temp[10] = {NULL};
			for(x = 0; x < 10; ++x)
				temp[x] = NULL;
			tokenize(temp, &argv[x][1]);
			//for(x=0; temp[x] != NULL; ++x)
			//	printf("%s\n", *temp[x]);
			}
		else printf("%s ", argv[x] );
	            }
	printf("\n");
	}



// WRITTEN BY MICHAEL RYAN WITH HELP FROM KOREN COLE
//	char * add;
//	bool valid_directory = false;
	else if(strcmp(argv[0], "cd") == 0 ) {
		if( x == 1 || !strcmp(argv[1],"~") ) {
			chdir( getenv("HOME"));
			setenv("PWD",getenv("HOME"),1);
        		}
	


		else if(x == 2){
			//if ( !strcmp( getcwd(NULL, 100), getenv("HOME") ) )
				
			if( chdir(argv[1]) == -1 ){
				if (argv[1][0] == '$'){
					if( chdir( getenv(&argv[1][1])) == -1)
			  		 printf("%s: No such file or directory.\n", getenv(&argv[1][1]));
					 }
				else printf("%s: No such file or directory.\n", argv[1]);		
					}
			else setenv("PWD",getcwd(NULL,PATH_MAX),1);
			}

			
		else if(x > 2)				//more than 2
                	printf("Error: Too many arguments\n");
               			

		}



        else if( (child_id = fork() ) == 0 ){
             /*	char  path[PATH_MAX];
		strcpy( path, "/bin/");
		strcat( path, argv[0] );
                execv( path , argv);
		strcpy(path, getcwd(NULL,PATH_MAX) );
		strcat(path, argv[0]);
	*/ 	execv(argv[0], argv);				//code executed by child process
                printf("Unknown command %s\n", argv[0] );	// needs to be in this format to work with path resolution
                }
	else{
	
	 while(1){
							//wait for child process to return
		x = waitpid(child_id, &status, 0);	//parent process code`
		if( x == child_id)
			break;
		}
   
	}
 }

 exit(EXIT_SUCCESS);

}

