/**
A simple bash shell
Created by Jeremy Galang
**/

#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <limits.h>

int main(int argc, char **argv){
char input [200];
char* name; //name of the shell, default goes to 308sh> 
int killed = 0;

//everything execept the built-in commands should be passed into exec, the only if statements you should have are the built-in ones.

//initial prompt execution, checks to see that the user has entered an argument
if(argc > 1 && strcmp(argv[1], "-p") == 0 && argc != 2){
//takes in the second argument as long as it's not null
 name = argv[2];
}
else{ //default shell name
 name = "MyShell>"; 
}


while(1){
	
	usleep(1500); //this is saved for printing the background exit status separately 
	printf("%s ", name); //print the name of the shell
	
	fgets(input, 200, stdin); //stdin takes user input until user presses enter
	
	int len = strlen(input);
	input[len-1] = '\0'; //assign the last element to be null
	
	char* parts[len]; 
	int index = 0; 
	char* part = strtok(input, " "); //split the string
	
	while(part != NULL){ //given on the board on how to split a string
		parts[index] = part;
		index++;
		part = strtok(NULL, " ");
		parts[index] = NULL;
	}
	
	if(strcmp(parts[0], "exit") == 0){ //exits the shell
		return 0; 
	}

	else if(strcmp(parts[0], "pwd") == 0){ //print working directory
		char cwd[PATH_MAX]; 
		printf("%s\n", getcwd(cwd, sizeof(cwd)));
	}

	else if (strcmp(parts[0], "cd") == 0){//change directory 
		
		if(parts[1] == NULL){ //default to home if a second argument isn't sepcified
			chdir(getenv("HOME"));
		}
		
		else{ //change directories
		
		int check = chdir(parts[1]); //change directories to the specified location
		
			if (check == -1) { //if it fails
				printf("File/Directory does not exist\n");
		
			}
		
		}
		
	}

	else if(strcmp(parts[0], "pid") == 0){ //process id
		printf("Process ID: %d\n", getpid());
	}

	else if(strcmp(parts[0], "ppid") == 0){ //parent process id
		printf("Process ID: %d\n", getppid());
	}
 
	else{ 
	
		int boolean = 0; //boolean that checks if there's an ampersand
		
		if(strcmp(parts[index-1], "&") == 0){
		
			boolean = 1;
			parts[index-1] = NULL; //get rid of it so we can pass it through
		}
		
		if(boolean == 1){ //if it's a background process      
		
			int child = fork(); //create a child process
		
			if(child == 0){ 
				execvp(parts[0], parts); //execute the command
				printf("Cannot exec %s: no such file or directory\n", parts[0]); //this shouldn't execute if execvp works
			}
			else{ 
			
				pid_t temp; 
				int status;
		
				if(fork() == 0){ //create another child process to act as the "observer"
			
					temp = fork(); //create a "worker"
				
					if(temp == 0){ //if that worker is a child
						
						execvp(parts[0], parts);	
						printf("Cannot exec %s: no such file or directory\n", parts[0]); //this shouldn't execute if execvp works
		
					}
		
					else{	//otherwise print out it's information and wait, should run in the background 
				
						printf(">>>[%d] %s\n", temp, parts[0]); //print that the child was created					
						waitpid(temp, &status, 0);
						usleep(1000);
						printf("\n"); //for formatting

						if(status > 0){//check if process got killed
							printf(">>>[%d] %s killed (%d)\n", temp, parts[0], WEXITSTATUS(status));
						}
						else{ //otherwise print exit status
							printf(">>>[%d] %s Exit %d\n", temp, parts[0], WEXITSTATUS(status));
							printf("%s ", name); //reprinting the name of the shell after printing the exit status
						}
						
						exit(0);
				}	
				
			
			}
			
			}
		
		}
		
		else{ //non background processes
		
			pid_t temp; 
			int status;
			temp = fork(); //create a child process
			
			if(temp == 0){ 
				printf(">>>[%d] %s\n", getpid(), parts[0]); //announce the creation of a child
				execvp(parts[0], parts); //execute the command
				printf("Cannot exec %s: no such file or directory\n", parts[0]); //this shouldn't execute if execvp works
				exit(0); //this prevents me from having to type in "exit" multiple times
			}
		
			else{
				waitpid(temp, &status, 0); //wait on the parent id
				printf(">>>[%d] %s Exit %d\n", temp, parts[0], WEXITSTATUS(status)); //print the exit status
			
			}
		}
		
	}
	
}

}
