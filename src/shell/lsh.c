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

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

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
int main(void)
{
  Command cmd;
  int n;

  while (!done) {

    char *line;
    line = readline("> ");

    if (!line) {
      /* Encountered EOF at top level */
      done = 1;
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
	//printf(line);
	//printf("\n");
	//printf("&cmd: %d\n",&cmd);
	//printf("&cmd: %d\n",cmd);
	printf(" %d",strcmp(line, "qwe"));
	printf(compareStrings(line, "qwe"));
	if(strcmp(line, "qwe")==0)
		printf("qwe was typed \n");	
     	executeCommand(line); 
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
int executeCommand(char *s)
{  Command *cmd;
  int bkg = cmd -> background
  int ifd = STDIN_FILENO;
  int ofd = STDOUT_FILENO;
  pid_t pid;

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
    	args[0] = "/bin/ls";        // first arg is the full path to the executable
    	validCommand = 1;
    }
  if(validCommand){
      if ( fork() == 0 ){
             execv( args[0], args ); // child: call execv with the path and the args
            if(bkg){ signal(SIGINT, SIG_IGN);
                    dup2(ifd, STDIN_FILENO);
                    execArgsPiped(p, ofd);
                    exit(EXIT_SUCCESS);

            }
          }
    	else{
         if (!bg) waitpid(pid, NULL, 0);
      }
            
    }
    return 0;
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

void pipeMethod(int *piped){
  if(pipe(piped) == -1){
    perror(NULL);
  }
  return;
}

void execArgsPiped(Pgm *p, int parsed){
char ** str = p->pgmlist;
int piped[2];
pid_t pid;

if((p->next) == NULL){
  dup2(parsed, STDOUT_FILENO);
    if (execvp(*str, str) ==-1){
      perror(NULL);
      exit(EXIT_FAILURE);
    }
  }
    else{
        pipeMethod(piped);
        pid = fork();
        if(pid == 0){//child process
          close(piped[0]);
          execArgsPiped(p->next, piped[1]);
          exit(EXIT_SUCCESS)
        }
         else{//parent process
          close(piped[1]);
          waitpid(pid, NULL, 0);
          dup2(parsed, STDOUT_FILENO);
          dup2(piped[0], STDOUT_FILENO);
              if(execvp(*str, str) == -1){
                  perror(NULL);
                  exit(EXIT_FAILURE);
              }
        }

    }

}