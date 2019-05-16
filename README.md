# C-Shell

This code simulations a shell in your terminal. 
1). Supports any shell command and up to 10 parameters at once.
2). Typing ctrl-c or ctrl-z shall not stop or kill your shell. ctrl-z
shall suspend the currently running process. ctrl-c shall stop the currently running
process. 
3). Can list last 15 pids and 15 history commands.
4). has bg command which suspends a process to a background.
5). I used execvp which automatically searches for the path and executes in that order.
