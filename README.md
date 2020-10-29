# Command-Shell
Implemented Command shell using OS system calls to execute built-in Linux commands

Command Shell supports following things ->
  1) Shell run an infinite loop (which will only exit with the ‘exit’ command) and interactively process user commands. The shell print a prompt that indicate the current working directory followed by ‘$’ character
  
  2) Shell should support ‘cd’ command
  
  3) An incorrect command format prints an error message ‘Shell: Incorrect command’ 
  
  4) Shell supports multiple command execution either sequentially or in parallel. The commands separated by ‘&&’ is executed in parallel and the commands separated by ‘##’ is executed sequentially
  
  5) Shell supports redirect STDOUT for '>' symbol
