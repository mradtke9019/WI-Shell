#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	
	char *input = NULL;
	size_t length = 0;
	
	if(argc > 1 && strcmp(argv[1], "batch.txt") == 0){
		printf("batch command here\n");
	}
	else if(argc == 1){
		// Command prompt begins
		printf("wish> ");
		while(getline(&input, &length, stdin) != EOF){
			//Parse the input as a command and then execute
			char * temp = malloc(sizeof(char) * strlen(input));
			if(temp == NULL) {
				printf("Error allocating memory\n");
				exit(1);
			}
			strcpy(temp, input);
			
			temp = strtok(input, " \n");
			int argCount = 0;
			// Determine the amount of args
			while(temp != NULL){
				temp = strtok(NULL, " \n");
				argCount++;
			}
			// Correctly allocate the memory and parse the args
			
			char** command = malloc(sizeof(char*) * argCount);
			if(command == NULL) {
				printf("Error allocating memory\n");
				exit(1);
			}
			
			command[0] = strtok(input, " \t\n");
			printf("%s ", command[0]);
			// Parsing the arguments
			int i = 0;
			while(command[i] != NULL){
				
				i++;
				command[i] = strtok(NULL, " \t\n");
				printf("%s ", command[i]);
				
			}
			printf("\n");
			// Execute forked program here
			pid_t child = fork();
			
			// Attempt to execute the command
			if(child == 0) {
				int result = access(command[0], X_OK);
				// Attempt to access it in a relative directory
				if(result == 0){
					execv(command[0],command);
				}
				// If not here, check builtin commands
				else{
					int pathLength = 0;
					char* binPath = "/bin/";
					pathLength = strlen(binPath) + strlen(command[0]);
					
					binPath = malloc(sizeof(char) * pathLength);
					if(binPath == NULL) {
						printf("Error allocating memory\n");
						exit(1);
					}
					
					strcpy(binPath,"/bin/");
					strcat(binPath, command[0]);
					command[0] = malloc(sizeof(char) * (strlen(command[0]) + strlen(binPath)));
					strcpy(command[0],binPath);
					result = access(binPath, X_OK);
					
					
					if(result == 0){
						execv(command[0],command);
					}
					else{
						pathLength = 0;
						char* userPath = "/usr";
						pathLength = strlen(userPath) + strlen(command[0]);
						
						userPath = malloc(sizeof(char) * pathLength);
						strcpy(userPath,"/usr");
						strcat(userPath, command[0]);
						
						command[0] = malloc(sizeof(char) * (strlen(command[0]) + strlen(userPath)));
						strcpy(command[0],userPath);
						
						i = 0;
						printf("\nInside user, %i args\n",argCount);
						while( i < argCount){
							printf("%s ",command[i]);
							i++;
						}
						result = access(userPath, X_OK);
						if(result == 0){
							execv(command[0],command);
						
						} else{
							printf("Couldnt execute command\n");
							exit(1);
						}
					}
				}
			} else {
				// wait for child process to complete before waiting for another command
				int returnStatus = 0;
				waitpid(child, &returnStatus, 0);
				printf("wish> ");
			}
			
			for(i = 0; i < argCount; i++)
				free(command[i]);
			free(command);
		}
	}
	free(input);
	return 0;
}