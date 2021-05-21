#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#define MAX_LINE 80  /* The maximum length command */

/* function to initialize shell */
void InitializeShell(){
	for(int i=5;i>0;i--){
		system("clear");
		printf("\n\n\n");
		printf("\t\t******************************\n");
		printf("\t\t*                            *\n");
		printf("\t\t*                            *\n");
		printf("\t\t*       OS project #1        *\n");
		printf("\t\t*division of computer science*\n");
		printf("\t\t*         2017011912         *\n");
		printf("\t\t*        Namjeonghoon        *\n");
		printf("\t\t*                            *\n");
		printf("\t\t*    It starts in %d seconds  *\n",i);	
		printf("\t\t*                            *\n");
		printf("\t\t******************************\n");
		printf("\n\n\n");
		sleep(1);
		system("clear");
	}	
}
/*
	function to check the command is "exit". 
	if the command is "exit", return 1, or return 0.
	this return value will be used to determine whether to continue running the main loop.
*/
int CheckExit(char** args){
    if(!strcmp(args[0],"exit")) return 1;
    else return 0;
}
/*
	function to take input.
	first,initialize args with (null).
	then recive a single line of input on my_buf,
	cut my_buf by ' ' or '\n'. and store them in the my_args.(args)
	the return value idicates the length of args.
*/
int TakeInput(char** my_args){
    char my_buf[MAX_LINE];
    int my_cnt = 0;
    char* my_ptr;
    fgets(my_buf,sizeof(my_buf),stdin);
	my_ptr = strtok(my_buf," \n");
	for(;my_ptr!=NULL;my_cnt++){
	    my_args[my_cnt]=strdup(my_ptr);
		if(my_args[my_cnt][strlen(my_args[my_cnt])-1]=='\n'){
				my_args[my_cnt][strlen(my_args[my_cnt])-1]='\0';
			}
		my_ptr=strtok(NULL," \n");
	}
    return my_cnt;
}
/*
	function to check ampersand.
	if the end of command is &,
	the child process run in the background
*/
int CheckBack(char ** my_args,int my_cnt){
    if((my_cnt-1!=0)&&!strcmp(my_args[my_cnt-1],"&")){
			my_args[my_cnt-1]=NULL;
            return 1;
	}
    else return 0;
}
int main(){
	char *args[MAX_LINE/2+1]; /* command line arguments */
	int should_run = 1; /* flag to determine when to exit program */

	InitializeShell();
	while(should_run){
		int cnt,back_is_ampersand;
        pid_t pid;
		int fd[2];
		int status;
		memset(args,'\0',sizeof(args)); /* initialize args to (null) */
		 
		printf("\nosh>");
		fflush(stdout);		

        cnt = TakeInput(args);
        should_run = !CheckExit(args); 
		back_is_ampersand = CheckBack(args,cnt);
		pid = fork();

		if(pid<0){
           	fprintf(stderr, "Fork Failed");
            return 1;
        }
        else if(pid == 0){
            for(int i=0;i<cnt && args[i]!=NULL;i++){
                if(!strcmp(args[i],">")){ /* redirects the output of a command to a file */
                    args[i]=NULL;
                    fd[0]=open(args[i+1],O_WRONLY|O_CREAT,0666);
                    dup2(fd[0],STDOUT_FILENO);
                    close(fd[0]);
                }
                else if(!strcmp(args[i],"<")){ /* redirects the input of a command from a file */
                    args[i]=NULL;
                    fd[0]=open(args[i+1],O_RDONLY,0666);
                    dup2(fd[0],STDIN_FILENO);
                    close(fd[0]);
                }
                else if(!strcmp(args[i],"|")){ /* allow the output of one command to serve as input to another using a pipe */
                    args[i]=NULL;
                    pipe(fd);
					pid_t pid2 = fork();
                    if(pid2 < 0) fprintf(stderr, "Fork Failed");
                    else if(pid2 == 0){ 
                        dup2(fd[1],1); 
                        close(fd[0]); 
                        execvp(args[0],args);  
                    }
                    else{
                        dup2(fd[0],0); 
                        close(fd[1]); 
                        execvp(args[i+1],&args[i+1]);
                    }
				}
			}
			execvp(args[0],args);
        }
        else{
            if(back_is_ampersand){
				waitpid(pid,&status,WNOHANG);
                printf("Child Complete");
            }
            else{
                waitpid(pid,&status,0);
                printf("Child Complete");
            }
        }
	}
	return 0;
}