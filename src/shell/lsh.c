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


#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"
#define SIGINT  2   /* Interrupt the process */ 

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
	printf(line);
	printf("\n");
	pid = fork();
	if(pid==0){
	    //child process
	    printf("child executing... \n");     	
	    //executeCommand(line); 
	    executeCmd2(&cmd);
	    printf("child done\n");
	    }
	else{
		Command *cmdPointer = &cmd;
		int bg = cmdPointer->bakground;
		if(!bg){
		    wait(0);
		    printf("parent waiting..\n");
		}
		printf("Parent done, pid: %d\n",pid);
	    }
	printf("Done: %d\n",done);
	}
    }
    
    if(line) {
      free(line);
    }
  }
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
int executeCmd2(Command *cmd){
    	Pgm *p = cmd->pgm;
	execvp(p->pgmlist[0], p->pgmlist);
}
/*
int executeCommand(char *s)
{
    int validCommand=0;
    int status;
    char *args[2];
    printf("%s",s);
    args[1] = NULL;             // list of args must be NULL terminated

    if(compareStrings("ls", s)){
    	args[0] = "/bin/ls";        // first arg is the full path to the executable
    	validCommand = 1;
    }
    else if(compareStrings("date",s)){
    	args[0] = "/bin/date";        // first arg is the full path to the executable
    	validCommand = 1;
    }
    else if(compareStrings("who",s)){
    	args[0] = "/bin/who";        // first arg is the full path to the executable
    	validCommand = 1;
    }
    else if(compareStrings("exit",s)){
    done = 1;
    exit(0);
    }
    if(validCommand){
    	if ( fork() == 0 )
             execv( args[0], args ); // child: call execv with the path and the args
    	else
             wait( &status );        // parent: wait for the child (not really necessary)
    }
    return 0;
}
*/
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
