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

void prompt( char ** prompt){

 char user[33];
 char  machine[HOST_NAME_MAX];
 char cwd[PATH_MAX + 1];
 int len;

 strcpy( machine, getenv("MACHINE"));
 strcpy(user, getenv("USER"));
 getcwd(cwd, PATH_MAX);
 len  = strlen(user) + strlen(machine) + strlen(cwd) + 10;
 *prompt = malloc( len * sizeof(char) );
 strcpy(*prompt, user);
 strcat(*prompt,"@");
 strcat(*prompt,machine);
 strcat(*prompt," :: ");
 strcat(*prompt,cwd);
 strcat(*prompt," -> ");

 return;
}


int main(){
	char * string = NULL;
	char command[256];
	//printf("Max machine name%i\n", HOST_NAME_MAX);
	//printf("Max path%i\n", PATH_MAX);
	
	
 while(1){
	prompt(&string);
//	printf("%s\n",string);
	strcpy(command,readline(string));
	printf("%s\n",command);
	}






}

