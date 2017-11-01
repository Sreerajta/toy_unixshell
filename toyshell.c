#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<stdlib.h>
#define MAX_ARGS 10


void parseinp(char line[256], int *c_num,char *commands[][10]);
void piping(char *cmd_array[][MAX_ARGS], int cmd_no);
	
void main(){

	//accepting input:
	char cwd[100];
	char line[256];
	char *commands[10][10];
	int c_num;
	int status;
	int fd;
	
	while(1){
	
		getcwd(cwd,sizeof(cwd));
		printf("%s:~$",cwd);	
		fgets(line,sizeof(line),stdin);
		line[strcspn(line, "\n")] = 0;
		parseinp(line,&c_num,commands);
		int pid=fork();
		if(pid==-1)
	 		printf("\nError: Fork failed");
		if(pid==0){
			if(c_num==1)
		 		execvp(commands[0][0],commands[0]);
			else if(c_num==2&&strcmp(commands[1][0],">")==0){
				 int fd = open(commands[1][1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
				dup2(fd,1);
				execvp(commands[0][0],commands[0]);
				exit(0);
			
			}
			
			else{
				piping(commands,c_num);
				
		}
		
		
		
		
	}	              
		else{
			wait(&status);
	        }
		
		if(pid==1){
	 
			getcwd(cwd,sizeof(cwd));
			printf("%s:~$",cwd);
		}
	}	




}






void parseinp(char line[256], int *c_num,char *commands[][10]){
	int i_command=0;
	int j_arg=0;
	char *token;
	token=strtok(line," ");
	int or_flag=0;
	
	while(token!=NULL){
	
		if(strcmp(token,"|")==0){
			commands[i_command][j_arg]=0;
			j_arg=0;
			i_command+=1;
		}		

		else if(strcmp(token,">")==0){
		        j_arg=0;
			commands[++i_command][j_arg]=token;
			j_arg++;
			or_flag++;		

		}

		else{
			commands[i_command][j_arg]=token;
			j_arg++;
		}

		token=strtok(NULL," ");

	}

	commands[i_command][j_arg]=0;
	i_command+=1;
	
	*c_num=i_command;
	
}




void piping(char *cmd_array[][MAX_ARGS], int cmd_no)
{
    int pfd1[2], pfd2[2];  

    int i, pid;

    for (i=0; i<cmd_no; i++) {
        
        if (i % 2 != 0)
            pipe(pfd1);    
        else
            pipe(pfd2);     

        pid = fork();
          
          if(pid==0){
            if (i == 0)
                dup2(pfd2[1], STDOUT_FILENO);

            else if (i == cmd_no - 1) {
                if (cmd_no % 2 != 0)
                    dup2(pfd1[0], STDIN_FILENO);
                else
                    dup2(pfd2[0], STDIN_FILENO);
            }

            else {
                if (i % 2 != 0) {
                    dup2(pfd2[0], STDIN_FILENO);
                    dup2(pfd1[1], STDOUT_FILENO);
                }
                else {
                    dup2(pfd1[0], STDIN_FILENO);
                    dup2(pfd2[1], STDOUT_FILENO);
                }
            }

            
            if(strcmp(cmd_array[i][0],">")!=0)
            execvp(cmd_array[i][0], cmd_array[i]);
            else{
	    	 int fd = open(commands[1][1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	    }

             
        }


        if (i == 0)
            close(pfd2[1]);

        else if (i == cmd_no - 1) {
            if (cmd_no % 2 != 0)
                close(pfd1[0]);
            else
                close(pfd2[0]);
        }

        else {
            if (i % 2 != 0) {
                close(pfd2[0]);
                close(pfd1[1]);
            }
            else {
                close(pfd1[0]);
                close(pfd2[1]);
            }
        }

        wait(&pid);
    }
}







