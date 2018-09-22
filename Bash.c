#define _GNU_SOURCE
#define __USE_POSIX
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
//#include <ctype.h>
#include <unistd.h>
#include<limits.h>
#include <stdbool.h>
#include <dirent.h>
#include<string.h>


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
//out_redir and in redir_ hold pointer to their repective IO redirection file string
//pipe holds the number of pipes parsed
//Written by Daniel

int tokenize( char ** arg, char *line, char **out_redir, char **in_redir, int *pipe){	
 int i = 0;			//line iterator
 int it = 0;			//argv iterator
  *out_redir = *in_redir = NULL;	// string to output and input redirect
 bool in = false, out = false;		// in/out redirect

 while( line[i] != '\0' ){


	if( line[i] == '>' ){					//
		out = true;					//indicate there is output redirection
		line[i] = '\0';
		while( isspace( line[++i] ))			//read until no blank space
			continue;
		if( line[i] != '\0' )				//save pointer of file string
			*out_redir = &line[i];
		 while( ++i){					//
                        if( line[i] == '\0' || isspace(line[++i])){		
                                --i;
                                break;}
			}
		}
	
	else if( line[i] == '<' ){			
		in = true;					//indicate input redirection
		line[i] = '\0';		
		while( isspace( line[++i] ) )			//read until no blank space
			continue;	
		if(line[i] != '\0')
			*in_redir = &line[i];			//save pointer to file string	
	
		while( ++i ){
                	if(line[i] == '>' || line[i] == '\0' || isspace(line[i]) ){ 	//read until these characters are encountered
				--i;
				break;}
			}
			   
		}


		
                
        else if( line[i] == '\''){
                 arg[it] = &line[i];			//save pointer to ' char
		 ++i;		
                 ++it;
                 while( 1 ){
        	         if( line[i] == '\'' ){		//read until '
                	         line[i] = '\0';
                                 break;
                                 }
			 else if( line[i] == '\0' ){
				printf("Unmatched '.\n");	// print error if no end ' char is encountered
				return -1;
				}
                          ++i;
                          }
                 }
	else if( line[i] == '\"'){
               
                arg[it] = &line[i];			//save pointer to " char
		++i;               
		++it;
                 while( 1 ){
        	         if( line[i] == '\"' ){		//read until next "
                	         line[i] = '\0';
                                 break;
                                 }
			 else if( line[i] == '\0' ){
                               	printf("Unmatched \".\n");	//error if not end "
                               	return -1;
				}
                        ++i;
                        }
                  }
	else if( isspace(line[i]) )			//set blank space equal to null character for string seperation
		line[i] = '\0';

	else if( line[i] == '|' ){			//
		 arg[it] = &line[i];			//save pipe token to argv
		 line[i] = '\0';			//set val equal to null 
                 ++it;
		 ++(*pipe);				//increment pipe count
                 }


	else {
						
        	arg[it] = &line[i];      		//save any other character to the argv
                ++it;
                
                while( ++i ){
					
			if( line[i] == '<' || line[i] == '>' || isspace(line[i]) || line[i] == '\0' || line[i] == '|'){		//read til these characters
				--i;
				break;	
				}			
		      	                                 
                         }
		}


	 ++i;
         }
 
 
	if( (in == true && (*in_redir == NULL || it == 0)) || (out == true && (*out_redir == NULL) || it == 0) ){	//Invalid redirect if not output file when output bool is true
		printf("Invalid redirect syntax\n");					
		return -1;
			}
		
	if( pipe > 0 && it != (*pipe + 1) ){
		printf("Incorrect pipe syntax\n");	
		pipe = 0;
		}
 return it;
 }

//Used for pipe commands in argv
//Finds the pipe, sets argv = to NULL
//returns next set of command args index
//Written by Daniel



int getNextArgs( char ** argv ){
	int it;
	//printf("getNextArgs function ... \n");
	for( it = 0; argv[it] != NULL; ++it){
		printf("%p\n", argv[it]);
		if(  argv[it][0] == '\0' ){		//Null characters are saved to argv to indicate a pipe
			argv[it] = NULL;		//set argv equal to NULL for execv function
			//printf("%i\n", argv[it] == NULL );
			return it + 1;			//return index of next arg in argv
			}
		}
	return -1;
	}


//Written by Daniel
//Tokenizer parses for input and ouput file redirect
//pass them in along with argv
//and it will for and return once called.
//takes care of waiting for child process to return

bool forkANDexec(char * out_file, char *in_file, char **argv){
	int x, status;
        pid_t child_id, temp_id;
        int fd1 = -1, fd2;	
	char  path[PATH_MAX];

       if( (child_id = fork() ) == 0 ){
           	if (out_file != NULL ){							//open file if string to out file is not NULL
			fd1 = open( out_file, O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR);
			//if( fd1!= -1 )
				dup2(fd1, 1);						//redirect std out to file
			//printf("redirecting to %s\n", out_file);
			}
		if( in_file != NULL ){							//open input file
			fd1 = open( in_file, O_RDONLY );
			if( fd1 != -1 )
				dup2(fd1, 0);						//use input file for stdin redirect
			}

		strcpy( path, "/bin/");							//check if file is in bin
		strcat( path, argv[0]);
                execv ( path, argv   );
		strcpy( path, getcwd(path,PATH_MAX) );					//check if file is in cwd
		strcat( path, argv[0]);
		execv(  path, argv   );				//code executed by child process
                printf("Unknown command %s\n", argv[0] );				
              	exit(EXIT_FAILURE);
		}
	
	//	printf("child_id: %i\n",child_id);
	 while(1){
							//wait for child process to return
		x = waitpid(child_id, &status, 0);	//parent process code`
		if( x == child_id){
			if( fd1 != -1 )
				close(fd1);
			return true;
		 	}
		}
   
	

}










int main(){
  
	//take this first timevalue, store in struct	
	struct timeval  first, second;
        gettimeofday(&first, NULL);


        char *command = malloc(256 * sizeof(char));		//holds entire command string
        char ** argv;						//holds pointers to the beginning of each token in command
	char * in_file = NULL;					//string to input file
	char * out_file = NULL;					//string to output file


        int x,it;						//iterators / general used variables
	int pipe_ ;						//# of pipes
	
        argv = malloc( 10 * sizeof(char*) );            	//Still haven't figured out max # of tokens

	
 while(1){
       
//	strcpy(command, r
//	adline("Enter random path: ") );
	pipe_ = 0;						//reset pipe count

	prompt();						//print prompt

        strcpy(command,readline(" "));				//copy user input to command 
	
	if( !strcmp(command,"" ) )				//continue if empty
		continue;		

      for(it = 0; it < 10; ++it)				//reset args
		argv[it] = NULL;
			
        x = tokenize(argv,command, &out_file, &in_file, &pipe_);   //tokenize the command into argv pointers        
	printf("Tokens: %i\n", x);					
	printf("# of pipes: %i\n", pipe_);
			
	for(it=0; argv[it] != NULL; ++it )			//print tokens
		printf("%s ", argv[it]);
	printf("\n");

if( x!= -1 ){							//if no error in tokenizer execute following conditions

	if( !strcmp(command, "exit") )
        {
                gettimeofday(&second, NULL);		//before, ending, take second time value and put in struct
                printf("Exiting...\n\tSession time: %ds\n", (int) (second.tv_usec - first.tv_usec) / 1000000 + (int) (second.tv_sec - first.tv_sec));
		//get difference between adn first and second struct
                exit(EXIT_SUCCESS);
        }
                


























	else if( pipe_ > 0 ){				
		int i = 0;
		char ** temp = &argv[0];
		pid_t child_id;
		int pipes[pipe_ * 2];
		int status;
		char path[PATH_MAX];

		for( int x = 0; x < pipe_ * 2; ){
			pipe( &pipes[x] );
			x += 2;
			}		

		pipe (pipes);
		it = 0;
		for(x = 0; x <= pipe_; ++x){
			it += getNextArgs( &argv[it] );
			
			if ( (child_id = fork()) == 0){
				
				if( x % 2 == 0 ){ 	//if writing
				

				
				dup2(pipes[1],0);
				close(pipes[0]);
				close(pipes[1]);
	
				strcpy( path, "/bin/");
	                	strcat( path, argv[i] );
        	        	execv( path , &argv[i]);
                		strcpy(path, getcwd(path,PATH_MAX) );
	                	strcat(path, argv[i]);
        	        	execv(path, &argv[i]);        
				printf("%s\n", path);                  //code executed by child process
              	  		printf("Unknown command %s\n", argv[i] );       //
               			 exit(EXIT_FAILURE);			
				}
	
			i = it;
			argv = temp;
			}
		
		while(x = waitpid(child_id, &status, 0)){      //parent process code`
                	 if( x == child_id)
                       		break;		
				}
				
			
				

			}
	}

















	// WRITTEN BY Daniel
       else if( !strcmp("echo",argv[0]) ){		// covers echo built in and env variables
    	


	bool break_ = false;				//break boolean
	for(x = 1; argv[x] != NULL; ++x){
                 if(argv[x][0] == '$'){			//if env var
                         if( getenv(&argv[x][1]) == NULL ){	//if NO variable
				if( out_file == NULL )		//if not out redirect
                                 printf("%s: Undefined variable.\n",&argv[x][1]);		
				else {
					char string[500];
					strcpy(string, &argv[x][1]);
					strcat(string, ": Undefined variable.\n");
					it = open( out_file, O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR);
					write(it, string, strlen(string));
					close(it);
					}
                         	 break_ = true;
			        break;
                                 }
			}
		}
	if( break_ ) continue;

       	for(x = 1; argv[x] != NULL; ++x){
		if(argv[x][0] == '$'){
			if( out_file != NULL){				//redirect to file
			 it = open( out_file, O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR);
                         write(it, getenv(&argv[x][1]), strlen(getenv(&argv[x][1])));
			 close(it);
			 }	
			else		
			 printf("%s ", getenv(&argv[x][1]));		//stdout
			}
		else if(argv[x][0] == '\''){
			if( out_file != NULL ){				//redirect to file
			 it = open( out_file, O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR);
                         write(it, &argv[x][1], strlen(&argv[x][1]) );
			 close(it);
			 }
			else
			 printf("%s ", &argv[x][1]);			//stdout
			}
	
		else  if( out_file != NULL){
			it = open( out_file, O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR);	//redirect
                         write(it, &argv[x], strlen(argv[x]));		
			 close(it);
			}
		else	printf("%s ", argv[x] );				//stdout
	            }
	printf("\n");
	}

	// WRITTEN BY Daniel Marquez
	
	else if(strcmp(argv[0], "cd") == 0 ) {					//
		
		
		if( x == 1 || !strcmp(argv[1],"~") ) {
			chdir( getenv("HOME"));
			setenv("PWD",getenv("HOME"),1);
        		}		

		else if( argv[1][0] == '~' ){
			argv[1] = &argv[1][2];
			chdir(getenv("HOME"));
			it = chdir(argv[1]);
			if (it == -1 ){
				chdir(getenv("PWD"));
				printf("~/%s: No such file or directory.\n", argv[1] );
				}
			else
				setenv("PWD", getcwd(NULL,PATH_MAX), 1);
			}

		else if(x == 2){
							
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



	
	else
		forkANDexec(out_file, in_file, argv);

}
 }
// free(prompt);
 exit(EXIT_SUCCESS);

}




















































//Writen by Korren
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
     if((expPath = strstr(path, "..")) != NULL)  // expand ..
        {
                tempPath = malloc(strlen(getenv("PWD")));
                strcpy(tempPath, getenv("PWD"));
                //printf("temp path: %s\n", tempPath);
                varLocation = malloc(strlen(tempPath));
                strcpy(varLocation, tempPath);
                while(!commandFound)
                {
                        //printf("just ..\n");
                        temp = strrchr(varLocation,'/');
                        //printf("this is var temp: %s\n", temp); 
                        tempSize = strlen(varLocation) - strlen(temp);
                        //printf("tempsixe: %i \n", tempSize);
                        if(commandFound == false && tempSize == 0)
                        {
                                //printf("this is the path if comm is false: %s\n", varLocation);
                                printf("At the root, cannot go to any other directory.\n");
                                strcpy(path, varLocation);
                                free(varLocation);
                                free(tempPath);
                                return path;                    // exits early  
                        }
                        varLocation = malloc(tempSize +1);
                        strncpy(varLocation, tempPath, tempSize);
                        varLocation[tempSize] = '\0';
                        //printf("this is varLocation: %s\n", varLocation);
                        //strcpy(path,varLocation);
                        path = path + 3;                // remove the .. and next / 
                        //free(varLocation);
                        //printf("this is the path now:%s\n ", path);
                        if((expPath = strstr(path,"..")) == NULL)
                        {
                                commandFound = true;
                                strcpy(path, varLocation);
                                free(varLocation);
                                free(tempPath);
                        }
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
//>>>>>>> 04b9357fae57b482faf3b1eb5bf2c106630ce89d
}
}

}



//This function produces the prompt
//For every time input is expected
//from within the shell.
//
//Written By Daniel
char * pathresolution( char * path ){
	



	}




