/* 
 * Main source code file for lsh shell program
 *
 * You are free to add functions to this file.
 * If you want to add functions in a separate file 
 * you will need to modify Makefile to compile
 * your additional functions.
 *
 * Add appropriate comments in your code to make it
 * easier for us while grading your assignment.
 *
 * Submit the entire lab1 folder as a tar archive (.tgz).
 * Command to create submission archive: 
      $> tar cvf lab1.tgz lab1/
 *
 * All the best 
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"
#define SIGINT  2   /* Interrupt the process */ 
#define READ_PIPE 0
#define WRITE_PIPE 1
/*
 * Function declarations
 */

void PrintCommand(int, Command *);
void PrintPgm(Pgm *);
void stripwhite(char *);
/* When non-zero, this global means the user is done using this program. */
int done = 0;

/*
 * Name: main
 *
 * Description: Gets the ball rolling...
 *
 */
void handle_sigint(int sig) 
{ 
    printf("Caught signal %d\n", sig); 
}
int main(void)
{
  Command cmd;
  int n;
  pid_t pid;
  signal(SIGINT, handle_sigint); 

  while (!done) {

    char *line;
    line = readline("> ");

    if (!line) {
      /* Encountered EOF at top level */
      done = 1;
    }//Exits the program on command;
    else if(compareStrings(line,"exit")){
	    done=1;	
	}
    else {
      /*
       * Remove leading and trailing whitespace from the line
       * Then, if there is anything left, add it to the history list
       * and execute it.
       */
      stripwhite(line);

      if(*line) {
        add_history(line);
        /* execute it */
        n = parse(line, &cmd);
        PrintCommand(n, &cmd);
	//checking for special case cd
	if(checkForCD(&cmd)){
	    Command *c = &cmd;
	    Pgm *p = c->pgm;	
		char s[100];
  	    
	    if (chdir(p->pgmlist[1]) != 0)  
    	    	perror("chdir() failed, maby your directory doesn´t exist?\n"); 
			}	
        else{
	
		pid = fork();
		if(pid==0){
	    	//child process
	    	executeCmd2(&cmd);
	    	exit(0);
	    }
		else{//parent process
            Command *cmdPointer = &cmd;
	    	int bg = cmdPointer->bakground;
			//Make process run in background or wait
	    	if(!bg)
				wait(0);
	    }
	}
 	}
    }
    
    if(line) {
      free(line);
    }
  }
  return 0;
}
int checkForCD(Command *cmd){
	Pgm *p = cmd->pgm;
	if(compareStrings(p->pgmlist[0],"cd"))
	    return 1;
	return 0;
}
/*
 *returns 1 if strings identical else -1
 *
 * */
int
compareStrings (char *s1, char *s2)
{
if(strcmp(s1,s2)==0)
	return 1;
return 0;
}

/*----HERE THE MAGIC HAPPENS-----*/
int executeCmd2(Command *cmd){
 
	int cmd_counter = 1;
	int fd[2];

    Pgm *p = cmd->pgm;
	while(p->next){
		cmd_counter ++;
		p=p->next;
	}
	printf("command counter: %d\n", cmd_counter);
	if(cmd_counter == 1){
   		if (cmd->rstdin)
			freopen(cmd->rstdin, "r", stdin);
    	if (cmd->rstdout)
			freopen(cmd->rstdout, "w", stdout);
	
    	execvp(p->pgmlist[0], p->pgmlist);
	}else{
        printf("In else \n");
		int i = 0;
		int MSGSIZE = 100;
		char inbuf[MSGSIZE]; 
		//pipe(fd_parent_to_child);
		pipe(fd);
		p = cmd->pgm;
		printf("before while\n");
		while(i<cmd_counter){
			printf("In while\n");
			pid_t pid = fork();
			printf("pid: %d\n", pid);	
			if(pid<0){ printf("fork failed");}
			if(pid==0){//child process
				printf("Child executing pipe");
    			close(fd[0]);
    			dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);
				execvp(p->pgmlist[0], p->pgmlist);
				exit(0);
			}
			else{//Parent process
				printf("In parent\n");
				wait(0);
				dup2(fd[0],STDIN_FILENO);
				//close(fd_child_to_parent[0]);
				read(fd[0], inbuf, MSGSIZE); 
				printf(inbuf);
			}
		p=p->next;
		i++;
		}
		
   		close(fd[0]);
    	close(fd[1]);
		execvp(p->pgmlist[0], p->pgmlist);
	}
}
/*
 * Name: PrintCommand
 *
 * Description: Prints a Command structure as returned by parse on stdout.
 *
 */
void
PrintCommand (int n, Command *cmd)
{
  printf("Parse returned %d:\n", n);
  printf("   stdin : %s\n", cmd->rstdin  ? cmd->rstdin  : "<none>" );
  printf("   stdout: %s\n", cmd->rstdout ? cmd->rstdout : "<none>" );
  printf("   bg    : %s\n", cmd->bakground ? "yes" : "no");
  PrintPgm(cmd->pgm);
}

/*
 * Name: PrintPgm
 *
 * Description: Prints a list of Pgm:s
 *
 */
void
PrintPgm (Pgm *p)
{
  if (p == NULL) {
    return;
  }
  else {
    char **pl = p->pgmlist;

    /* The list is in reversed order so print
     * it reversed to get right
     */
    PrintPgm(p->next);
    printf("    [");
    while (*pl) {
      printf("%s ", *pl++);
    }
    printf("]\n");
  }
}

/*
 * Name: stripwhite
 *
 * Description: Strip whitespace from the start and end of STRING.
 */
void
stripwhite (char *string)
{
  register int i = 0;

  while (isspace( string[i] )) {
    i++;
  }
  
  if (i) {
    strcpy (string, string + i);
  }

  i = strlen( string ) - 1;
  while (i> 0 && isspace (string[i])) {
    i--;
  }

  string [++i] = '\0';
}
