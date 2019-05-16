// The MIT License (MIT)
//
// Copyright (c) 2016, 2017 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

/*
  Name: Randy Bui
  ID: 1001338008
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n" // We want to split our command line up into tokens
                           // so we need to define what delimits our tokens.
                           // In this case  white space                        
                           // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10 // Mav shell only supports ten arguments

static void handle_signal(int sig)
{

  /*
   Determine which of the two signals were caught
  */
  switch (sig)
  {
  case SIGINT:
    break;

  case SIGTSTP:
    break;

  default:
    printf("Unable to determine the signal\n");
    break;
  }
}

int main()
{
  //array made to store first 50 commands
  char h[50][MAX_COMMAND_SIZE];

  //history counter
  int hc = 0;

  //array made to store first 15 pids
  int pid_array[15];

  //pid counter
  int pid_counter = 0;

  //cmd_str takes in string size of 255
  char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);

  struct sigaction act;

  /*
    Zero out the sigaction struct
  */
  memset(&act, '\0', sizeof(act));

  /*
    Set the handler to use the function handle_signal()
  */
  act.sa_handler = &handle_signal;

  /*
    Install the handler for SIGINT and SIGTSTP and check the
    return value.
  */
  if (sigaction(SIGINT, &act, NULL) < 0)
  {
    perror("sigaction: ");
    return 1;
  }

  if (sigaction(SIGTSTP, &act, NULL) < 0)
  {
    perror("sigaction: ");
    return 1;
  }

  while (1)
  {

    // Print out the msh prompt
    printf("msh> ");

    //signal(SIGINT,&handle_signal);
    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin))
      ;

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];
    int token_count = 0;
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;

    char *working_str = strdup(cmd_str);

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while (((arg_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
           (token_count < MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup(arg_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0)
      {
        token[token_count] = NULL;
      }
      token_count++;
    }
    if (token[0] == '\0')
    {
      continue;
    }
    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality
    while (1)
    {
      if (token[0][0] == '!')
      {

        //history target is the n from !n which is a int
        int history_target = atoi(&token[0][1]);

        //condition to prevent !0 from crashing program by watching the
        //history_target and hc count
        if (history_target >= 0 && history_target <= 15 && history_target < hc && hc > 0)
        {
          //clearing out token array with null character
          memset(&token, '\0', sizeof(token));
          //h is an array with the n of !n as index value and
          //is duplicate string by allocating memory
          //and using a pointer so it can be stored in history_copy
          char *history_copy = strdup(h[history_target]);

          //resets token count before tokenizing the !n command because
          //token_count is the same one used for
          //tokenizing cmd_str into token array
          token_count = 0;
          //tokenizing the history_copy into token array using
          //whitespace as a delimiter
          while (((arg_ptr = strsep(&history_copy, WHITESPACE)) != NULL) &&
                 (token_count < MAX_NUM_ARGUMENTS))
          {
            token[token_count] = strndup(arg_ptr, MAX_COMMAND_SIZE);
            if (strlen(token[token_count]) == 0)
            {
              token[token_count] = NULL;
            }
            token_count++;
          }

          //after you history_copy is tokenized into
          //the token you want to continue to execute
          //code that executes command in token array
          continue;
        }

        else
        {
          //if !n is not valid or meet if constraints then it will execute this
          printf("Command not found \n");
          //storing user input into history array and incrementing the hc
          //which is history counter is incremented
          strcpy(h[hc], cmd_str);
          hc++;
          if (hc == 50)
          {
            hc = 0;
          }
          break;
        }
        break;
      }
      //reads first token to see if listpids if true then
      //all pids from array is printed
      //after for loop is completed
      //store string "listpids" into
      //history array
      else if (strcmp(token[0], "listpids") == 0)
      {
        int i;
        for (i = 0; i < pid_counter; i++)
        {
          printf("%d: %d\n", i, pid_array[i]);
        }
        strcpy(h[hc], cmd_str);
        hc++;
        if (hc == 50)
        {
          hc = 0;
        }
        break;
      }
      //checking if first token is either quit or exit
      //if true then exit shell
      else if (strcmp(token[0], "quit") == 0 || strcmp(token[0], "exit") == 0)
      {
        exit(0);
      }
      //check if first token is cd
      //change directory to token[1]
      //which is the user destination
      //after cd command string "cd"
      //will be stored in history array
      else if (strcmp(token[0], "cd") == 0)
      {
        chdir(token[1]);
        strcpy(h[hc], cmd_str);
        hc++;
        if (hc == 50)
        {
          hc = 0;
        }
        break;
        //fflush(NULL);
      }
      //check if first token is history
      //if it is then this statement
      //will print out everything in
      //the history array up to the
      //50th index
      else if (strcmp(token[0], "history") == 0)
      {
        int i;
        for (i = 0; i < hc; i++)
        {
          printf("%d: %s", i, h[i]);
        }
        strcpy(h[hc], cmd_str);
        hc++;
        if (hc == 50)
        {
          hc = 0;
        }
        break;
      }
      //check if first token is bg(background)
      //if true then send a signal to last process
      //in pid array that was suspended and run
      //the process in the background
      //after sending signal bg will
      //be store into history array
      else if (strcmp(token[0], "bg") == 0)
      {
        kill(pid_array[pid_counter - 1], SIGCONT);
        strcpy(h[hc], cmd_str);
        hc++;
        if (hc == 50)
        {
          hc = 0;
        }
        break;
      }
      else
      {
        pid_t pid = fork();
        int status;

        if (pid == -1)
        {
          // When fork() returns -1, an error happened.
          perror("fork failed: ");
          exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
          // When fork() returns 0, we are in the child process.
          //execvp will take first argument as file name and then
          //second argument is the array of points that is represented
          //as a set arguments for the first argument
          //p in execvp is the PATH environment variable used to find
          //file name in path argument
          //if no path is specified then the default is recommended which is
          // usr/bin
          execvp(token[0], token);
          printf("%s: command not found \n", token[0]);
          //fflush(NULL);
          exit(EXIT_SUCCESS);
        }
        //if pid is greater than 0 that means
        //that pid id is the child process id
        //and is stored into pid array
        //pid array stores 15 pids
        else if (pid > 0)
        {
          if (pid_counter > 15)
          {
            pid_counter = 0;
          }
          pid_array[pid_counter] = pid;
          pid_counter++;
          // When fork() returns a positive number, we are in the parent
          // process and the return value is the PID of the newly created
          // child process.

          //waitpid makes parent process wait until
          //child process is finished executing everytime
          //because the order that fork runs in is
          //random and sometimes child may run
          //or parent but waitpid ensures that the child
          //will run everytime before parent
          waitpid(pid, &status, 0);
          //fflush( NULL );

          //copy execvp command into the history array
          strcpy(h[hc], cmd_str);
          hc++;
          if (hc == 50)
          {
            hc = 0;
          }
          break;
        }
      }
    }
    free(working_root);
  }
  return 0;
}
