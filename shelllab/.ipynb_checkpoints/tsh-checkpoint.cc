e// 
// tsh - A tiny shell program with job control
// 
// Bryce DesBrisay - 107692119
// David Skrenta - 108458842
//

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string>

#include "globals.h"
#include "jobs.h"
#include "helper-routines.h"

//
// Needed global variable definitions
//

static char prompt[] = "tsh> ";
int verbose = 0;

//
// You need to implement the functions eval, builtin_cmd, do_bgfg,
// waitfg, sigchld_handler, sigstp_handler, sigint_handler
//
// The code below provides the "prototypes" for those functions
// so that earlier code can refer to them. You need to fill in the
// function bodies below.
// 

void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

//
// main - The shell's main routine 
//
int main(int argc, char **argv) 
{
  int emit_prompt = 1; // emit prompt (default)

  //
  // Redirect stderr to stdout (so that driver will get all output
  // on the pipe connected to stdout)
  //
  dup2(1, 2);

  /* Parse the command line */
  char c;
  while ((c = getopt(argc, argv, "hvp")) != EOF) {
    switch (c) {
    case 'h':             // print help message
      usage();
      break;
    case 'v':             // emit additional diagnostic info
      verbose = 1;
      break;
    case 'p':             // don't print a prompt
      emit_prompt = 0;  // handy for automatic testing
      break;
    default:
      usage();
    }
  }

  //
  // Install the signal handlers
  //

  //
  // These are the ones you will need to implement
  //
  Signal(SIGINT,  sigint_handler);   // ctrl-c
  Signal(SIGTSTP, sigtstp_handler);  // ctrl-z
  Signal(SIGCHLD, sigchld_handler);  // Terminated or stopped child

  //
  // This one provides a clean way to kill the shell
  //
  Signal(SIGQUIT, sigquit_handler); 

  //
  // Initialize the job list
  //
  initjobs(jobs);

  //
  // Execute the shell's read/eval loop
  //
  for(;;) {
    //
    // Read command line
    //
    if (emit_prompt) {
      printf("%s", prompt);
      fflush(stdout);
    }

    char cmdline[MAXLINE];

    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)) {
      app_error("fgets error");
    }
    //
    // End of file? (did user type ctrl-d?)
    //
    if (feof(stdin)) {
      fflush(stdout);
      exit(0);
    }

    //
    // Evaluate command line
    //
    eval(cmdline);
    fflush(stdout);
    fflush(stdout);
  } 

  exit(0); //control never reaches here
}
  
/////////////////////////////////////////////////////////////////////////////
//
// eval - Evaluate the command line that the user has just typed in
// 
// If the user has requested a built-in command (quit, jobs, bg or fg)
// then execute it immediately. Otherwise, fork a child process and
// run the job in the context of the child. If the job is running in
// the foreground, wait for it to terminate and then return.  Note:
// each child process must have a unique process group ID so that our
// background children don't receive SIGINT (SIGTSTP) from the kernel
// when we type ctrl-c (ctrl-z) at the keyboard.
//
void eval(char *cmdline) 
{
  /* Parse command line */
  //
  // The 'argv' vector is filled in by the parseline
  // routine below. It provides the arguments needed
  // for the execve() routine, which you'll need to
  // use below to launch a process.
  //
  char *argv[MAXARGS];
  pid_t pid;  // process id 

  //
  // The 'bg' variable is TRUE if the job should run
  // in background mode or FALSE if it should run in FG
  //
  int bg = parseline(cmdline, argv); 
  if (argv[0] == NULL)  
    return;   /* ignore empty lines */
  
  // mask to block signal while job is being added 
  // so it's not deleted before it's added
  sigset_t mask; 
  sigemptyset(&mask);
  sigaddset(&mask, SIGCHLD);

  // handle built in commands by calling function
  if (!builtin_cmd(argv)) {
    // forking and execing a child process
    // handle non built in commands

    // parent will block SIGCHLD signal temporarily
    sigprocmask(SIG_BLOCK, &mask, 0);
    
    // if pid = 0 then it's a child
    if ((pid = fork()) <= 0) {
      // unblocks the child signal and sets a pid for it
      sigprocmask(SIG_UNBLOCK, &mask, 0); 
      setpgid(0,0);

      // validates command
      if (execve(argv[0], argv, environ) < 0) { 
        printf("%s: Command not found. \n", argv[0]);
        exit(0);
      }
    }
    
    // parent will wait for foreground job to terminate 
    // if in foreground
    if (!bg) {
      // add job
      addjob(jobs, pid, FG, cmdline); 
      // unblock the child signal
      sigprocmask(SIG_UNBLOCK, &mask, 0); 
      // wait for foreground to finish
      waitfg(pid); 

    }
    // if in background
    else {
      // add job
      addjob(jobs, pid, BG, cmdline);  
      // unblock the child signal
      sigprocmask(SIG_UNBLOCK, &mask, 0); 
      // print out job info
      printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
    }
  }

  return;
}


/////////////////////////////////////////////////////////////////////////////
//
// builtin_cmd - If the user has typed a built-in command then execute
// it immediately. The command name would be in argv[0] and
// is a C string. We've cast this to a C++ string type to simplify
// string comparisons; however, the do_bgfg routine will need 
// to use the argv array as well to look for a job number.
//
int builtin_cmd(char **argv) 
{
  string cmd(argv[0]);
  
  // quit
  if (cmd == "quit") exit(0); 

  // set to background or foreground
  else if (cmd == "bg" || cmd == "fg") { 
    do_bgfg(argv);
    return 1;
  }

  // list jobs
  else if (cmd == "jobs") { 
    listjobs(jobs);
    return 1;
  }

  // ignore bg process with no arg
  else if (cmd == "&") return 1;

  // not a built in command
  else return 0; 
}

/////////////////////////////////////////////////////////////////////////////
//
// do_bgfg - Execute the builtin bg and fg commands
//
void do_bgfg(char **argv) 
{
  struct job_t *jobp=NULL;
    
  /* Ignore command if no argument */
  if (argv[1] == NULL) {
    printf("%s command requires PID or %%jobid argument\n", argv[0]);
    return;
  }
    
  /* Parse the required PID or %JID arg */
  if (isdigit(argv[1][0])) {
    pid_t pid = atoi(argv[1]);
    if (!(jobp = getjobpid(jobs, pid))) {
      printf("(%d): No such process\n", pid);
      return;
    }
  }
  else if (argv[1][0] == '%') {
    int jid = atoi(&argv[1][1]);
    if (!(jobp = getjobjid(jobs, jid))) {
      printf("%s: No such job\n", argv[1]);
      return;
    }
  }	    
  else {
    printf("%s: argument must be a PID or %%jobid\n", argv[0]);
    return;
  }

  //
  // You need to complete rest. At this point,
  // the variable 'jobp' is the job pointer
  // for the job ID specified as an argument.
  //
  // Your actions will depend on the specified command
  // so we've converted argv[0] to a string (cmd) for
  // your benefit.
  //
  string cmd(argv[0]);

  // if cmd line is background process
  if (cmd == "bg") {
    // change job state to background
    jobp->state = BG;
    // tells system to run it again with new state
    kill(-(jobp->pid), SIGCONT);
    // print job info
    printf("[%d] (%d) %s", (jobp->jid), (jobp->pid), (jobp->cmdline));
  }
  // if cmd line is foreground process
  else if (cmd == "fg") {    
    // change job state to foreground
    jobp->state = FG;
    // tells system to run it again with new state
    kill(-(jobp->pid), SIGCONT);
    // wait for foreground job
    waitfg(jobp->pid);
  }

  return;
}

/////////////////////////////////////////////////////////////////////////////
//
// waitfg - Block until process pid is no longer the foreground process
//
void waitfg(pid_t pid)
{
  // sleep until pid is done
  while(pid == fgpid(jobs)){
    sleep(1);
  }
  return;
}

/////////////////////////////////////////////////////////////////////////////
//
// Signal handlers
//


/////////////////////////////////////////////////////////////////////////////
//
// sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
//     a child job terminates (becomes a zombie), or stops because it
//     received a SIGSTOP or SIGTSTP signal. The handler reaps all
//     available zombie children, but doesn't wait for any other
//     currently running children to terminate.  
//
void sigchld_handler(int sig) 
{
  pid_t pid;
  int status = -1;
	
  // waitpid(pid_t pid, int *status, int options) funciton info
  // if pid > 0 : thne the wait set is the singleton child process whose process ID is equal to the given pid
  // if pid = -1 : then the wait set consists of all of the parent's child process
  // WNOHANG | WUNTRACED option :
  // Return immediately, with a return value of 0, if none of the children in the wait set has stopped or terminated,
  // or with a return value equal to the PID of one of the stopped or terminated children.
    
  while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
	// the delete job signal for sigchld
    // WIFEXITED returns true if the child terminated normally
	if (WIFEXITED(status)) { 
        // delete child from jobs list
        deletejob(jobs, pid);
	}
    // WIFSIGNALED returns true if the child process terminated because of a signal that was not caught
    else if (WIFSIGNALED(status)) { 
      // print out info
			printf("Job [%d] (%d) terminated by signal %d \n",pid2jid(pid),pid,WTERMSIG(status));
      // delete child from jobs list
			deletejob(jobs, pid);
		}
    // WIFSTOPPED returns true if the child that caused the return is currently stopped
    else if (WIFSTOPPED(status)) {
      // print out info
			printf("Job [%d] (%d) stopped by signal %d \n",pid2jid(pid),pid,WSTOPSIG(status));
      // get jobs and change it's state
			job_t* temp = getjobpid(jobs, pid);
			temp->state = ST;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// sigint_handler - The kernel sends a SIGINT to the shell whenver the
//    user types ctrl-c at the keyboard.  Catch it and send it along
//    to the foreground job.  
//
void sigint_handler(int sig) 
{
  pid_t pid = fgpid(jobs);

  // kill pid if it exists in foreground
  if (pid > 0) kill(-pid, SIGINT);

  return;
}

/////////////////////////////////////////////////////////////////////////////
//
// sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
//     the user types ctrl-z at the keyboard. Catch it and suspend the
//     foreground job by sending it a SIGTSTP.  
//
void sigtstp_handler(int sig) 
{
  pid_t pid = fgpid(jobs);

  // kill pid if it exists in foreground
  if (pid > 0) kill(-pid, SIGTSTP);

  return;
}

/*********************
 * End signal handlers
 *********************/