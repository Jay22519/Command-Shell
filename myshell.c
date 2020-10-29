#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<unistd.h>
#include<string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>

//Function to display correct directory in shell
void shelldisplay()
{
    char directory[PATH_MAX];
    getcwd(directory, sizeof(directory));

    printf("%s$",directory) ;
}

char **split_space(char *input,char *sep) ; //To split the command with given seperator

int Execution(char *input) ;
void ExecutionP(char **command , int len) ;
void ExecutionRed(char **command,int index) ;
int main()
{
    int done = 1 ;
    size_t in_size = 100 ; //Used while taking input using getline
    char **command ;
    char halt[5] = {'e','x','i','t','\0'} ;

    //For ignoring Ctrl +C and Crtl + Z in shell
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);

    while(done) //That is infinite loop until exit command isn't given
    {

        shelldisplay() ;

        char *input = (char*)malloc(sizeof(char)*1000) ;
        char *seperator = (char*)malloc(sizeof(char)*100) ;
        getline(&input,&in_size,stdin) ;

        /***
        Now I have a given command line argument which first I'll slice wrt " " seperator and then for each slicing
        slice to get new char** everytime wrt the seperator given which could be "&&" or "##" or ">"
        ***/
        int index =  0 ;
        command = split_space(input,seperator) ; //This splits input wrt " " and also find the seperator

        if(strcmp(command[0],halt) == 0)   //That is given command is exit
        {
            printf("Exiting shell...\n");
            done = 0 ;
        }
        else if(strlen(command[0]) != 0) //That is continue only if it is non empty command
        {
            if(strcmp(seperator," ") == 0)  //That is it is only single command
            {
                    int answer = Execution(input) ;
		    if(answer == -1 )
            	    {
                	printf("Shell: Incorrect command\n");
            	    }

            }
            else if(strcmp(seperator,"&&")==0)    //Parallel
            {
                    while(command[index] != NULL)
                    {
                        index +=1 ;
                    }
                    //index is now the number of parallel command

                    ExecutionP(command,index) ;
            }
            else if(strcmp(seperator,"##") == 0)       //Sequential
            {
                int a = 1 ;
                while(command[index] != NULL && a)
                {
                    a = Execution(command[index]) ; index +=1;

                }
            }
            else if(strcmp(seperator,">") == 0)           //Redirectin Operator
            {
                while(command[index] != NULL)
                {
                    index +=1 ;
                }
                ExecutionRed(command,index) ;

            }
        }

    }
    return 0 ;
}

void ExecutionRed(char **command , int index)
{
    if(index <2)      //Not enough parameters passed for redirection
    {
        //Error
        //printf("Not enough parameters") ;
        printf("Shell: Incorrect command\n");
    }
    else
    {
        char *seperator = (char*)malloc(sizeof(char)*100) ;
        char *seperator2 = (char*)malloc(sizeof(char)*100) ;
        char **command2 = split_space(command[0],seperator) ;
        size_t p_arg;
        char** path = split_space(command[1],seperator2);
        if(path[0] != NULL)
        {
            int f_out = open(path[0], O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
            if(f_out <0)
            {
                //printf("File not open\n") ;
                printf("Shell: Incorrect command\n");
            }
            else
            {
                //Now to redirect my console output , I'll have to use dup i.e ->
                int dup_copy = dup(STDOUT_FILENO) ;
                dup2(f_out,STDOUT_FILENO) ;
                close(f_out);
                int a = Execution(command2[0])  ;

                if(a == 1 && dup2(dup_copy, STDOUT_FILENO) != -1)
                {
                    close(dup_copy);
                }
                else
                {
                    printf("Shell: Incorrect command\n");
                }
            }
        }

    }
}


void ExecutionP(char **command , int len)
{
    int index = 0 ;
    int stopper = 1 ; //Since only one fork is to be used so I'll use stopper
    while(index < len && stopper)
    {
        char *seperator = (char*)malloc(sizeof(char)*100) ;
        char **command2 = split_space(command[index],seperator) ;



        if(strcmp(command2[0],"cd") == 0)    //If given command is cd so no need of creating fork
        {
            //printf("Inside cd and command is %s  %d\n",command[0],strlen(command[0])) ;
            if(chdir(command2[1]) < 0)
            {
                printf("Shell: Incorrect command\n");
            }
            stopper = 0 ;
        }


        else
        {
            pid_t c_pid ;
            c_pid = fork() ;
            if(c_pid <0)
            {
               printf("Shell: Incorrect command\n");
            }

            else if(c_pid == 0)
            {
                stopper = 0 ;
                int answer = execvp(*command2,command2) ;
                if(answer == -1 )
                {
                    printf("Shell: Incorrect command\n");
                }
                exit(1) ;

            }

            else
            {
                stopper = 1 ;
            }

        }

        index +=1 ;

    }

    //Now waiting for all process to finish
    for (int i = 0; i < len; i++)
    {
        waitpid(-1, NULL, WUNTRACED);
    }
}



int Execution(char *input)  //This function is for execution of a single command
{
    char *seperator = (char*)malloc(sizeof(char)*100) ;
    char **command = split_space(input,seperator) ;
    if(strcmp(command[0],"cd") == 0)
    {
        //printf("Inside cd and command is %s  %d\n",command[0],strlen(command[0])) ;
        if(chdir(command[1]) < 0)
        {
            printf("Shell: Incorrect command\n");
            return 0 ;
        }
    }
    else
    {
        pid_t c_pid ;
        c_pid = fork() ;

        //For ignoring Ctrl +C and Crtl + Z in shell
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        if(c_pid <0)
        {
            printf("Shell: Incorrect command\n");
            return 0 ;
        }

        else if(c_pid == 0)
        {

            int answer = execvp(*command,command) ;
            if(answer == -1 )
            {
                printf("Shell: Incorrect command\n");
            }
            exit(1) ;
        }

        else
        {
            int wc = wait(NULL) ;
        }
    }


    return 1 ;
}


char **split_space(char *input,char *sep)
{

    input[strcspn(input, "\n")] = 0;      //To remove any new line after command line input
    int brk = 0 ; //For breaking loop
    int ind = 0 ;
    while(input[ind] && !brk)
    {
        if(input[ind] == '&')
        {
            strcpy(sep,"&&") ;
            brk = 1 ;
        }
        else if(input[ind] == '#')
        {
            strcpy(sep,"##") ;
            brk = 1 ;
        }
        else if(input[ind] == '>')
        {
            strcpy(sep,">") ;
            brk = 1 ;
        }
        ind +=1 ;
    }

    if(!brk)
    {
        strcpy(sep," ") ;
    }

    //printf(" Sep is %s ",sep) ;
    char *input_copy = (char*)malloc(100*sizeof(char)) ;
    // Creating this , so that I don't have to pass my original input into strsep function
    strcpy(input_copy,input) ;
    char *slice = (char*)malloc(50*sizeof(char)) ;
    char **command = malloc(50*sizeof(char*)) ;
    int index = 0 ;
    while((slice = strsep(&input_copy,sep)) != NULL )
    {
        if(strlen(slice) != 0)
        {
            while(*slice== ' ')
            {
                slice++ ;
            }

            char *end_s = slice + (strlen(slice)) - 1 ;
            while(*end_s == ' ')
            {
                end_s -- ;
            }

            end_s[1] = '\0' ;


            command[index] = (char*)malloc(sizeof(char)*100) ;
            //strcpy(command[index],slice) ;
            command[index] = slice ;
            index +=1 ;
        }

    }

    return command ;

}

