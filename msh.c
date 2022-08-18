#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                            // so we need to define what delimits our tokens.
                            // In this case  white space
                            // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11   // Mav shell only supports ten arguments

int main()
{
    int i;
    char *history[15];         // history array to save all the user command
    int  history_index = 0;
    int h_i = 0;
    int pid_history[15];
    int pid_history_index = 0; // pids array to save all the child ID (pids)
    int  p_h_i = 0;
    int history_rerun;         // int to contoral if the user enter valid number
                              //  in to history array if yes (valid number)exexute the rest of
                              // the code  if no print error massage.


    char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );


    for( i = 0; i < 15; i ++ )
    {
        history[i] = (char*) malloc( MAX_COMMAND_SIZE );
        memset( history[i], 0, MAX_COMMAND_SIZE );
    }


    while(1)
    {
        history_rerun = 1;

        // Print out the msh prompt
        printf ("msh> ");

        // Read the command from the commandline.  The
        // maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait here until the user
        // inputs something since fgets returns NULL when there
        // is no input
        while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );


        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];

        int   token_count = 0;
        if (strlen(cmd_str)-1 != 0)
        {

                // Pointer to point to the token
                // parsed by strsep
                char *argument_ptr;

                char *working_str  = strdup( cmd_str );

                // we are going to move the working_str pointer so
                // keep track of its original value so we can deallocate
                // the correct amount at the end
                char *working_root = working_str;

                // if the first character is !
                // copy from  history array into working_str if doing the ! command
                int s=0,r=0;

                if (cmd_str[0] ==  '!' && isdigit(cmd_str[1]))
                {


                    for (i = 1; i < strlen(working_str); i++)
                    {
                        if(isdigit(cmd_str[i]))   //  check if after ! is integer then
                                                // covert it from string to integer
                        {
                            s++;
                            char ch = cmd_str[i];
                            int num = atoi(&ch);
                            r = (r*10) + num;
                        }
                    }
                    if ((r > 0) && (r <= history_index))    //check if r in the valid range of
                                                            //the history array
                    {
                        strncpy(working_str,history[r-1], MAX_COMMAND_SIZE );
                        for ( i = s+1; i < strlen(cmd_str); i++)
                        {
                            strcat(working_str, &cmd_str[i]);
                        }
                        working_str = strtok(working_str,"\n");
                        printf("%s \n",working_str);
                    }
                    else {
                        printf("Command not in history \n");
                        history_rerun = 0;
                    }
                }
            if (history_rerun == 1)
            {

                if (history_index < 14)  // to fill the history array from 0 - 13
                {
                    strncpy(history[history_index], working_str, MAX_COMMAND_SIZE );
                    history[history_index] = strtok(history[history_index],"\n");
                    history_index++;

                }
                else if ((history_index == 14)&& (h_i ==0))  // to fill the history array
                                                            // in index 14
                {
                    strncpy(history[history_index], working_str, MAX_COMMAND_SIZE );
                    history[history_index] = strtok(history[history_index],"\n");
                    h_i = 1;
                }
                else
                {
                    // if the history array is full then shift all the command
                    // back one and store the new command in index 14
                    for (i = 0 ; i < 14; i++)
                    {
                        strncpy(history[i], history[i+1], MAX_COMMAND_SIZE );
                    }
                    strncpy(history[history_index], working_str, MAX_COMMAND_SIZE );
                    history[history_index] = strtok(history[history_index],"\n");
                }


                // Tokenize the input stringswith whitespace used as the delimiter
                while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
                        (token_count<MAX_NUM_ARGUMENTS))
                {
                    token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
                    if( strlen( token[token_count] ) == 0 )
                    {
                        token[token_count] = NULL;
                    }
                    token_count++;
                }
                //  shell functionality

                if (strcmp(token[0],"exit") == 0 || strcmp(token[0],"quit" ) == 0)
                {
                    exit( 0 );
                }
                else if(strcmp(token[0],"cd") == 0)  // change directory
                {
                    chdir(token[1]);
                }
                else if (strcmp(token[0],"history") == 0) // print history  array
                {
                    // if the history array index less than  13
                    int h = history_index-1;
                    if(h_i == 1)
                    {
                        // if the history index ==14
                        h = history_index;
                    }

                    for (i = 0; i <= h ; i++)
                    {
                    printf("[%d] %s\n", i+1, history[i]);
                    }
                }
                else if (strcmp(token[0],"showpids") == 0)
                {
                    // if the pid array index less than  13
                    int p = pid_history_index -1;
                    if(p_h_i == 1)
                    {
                        // if the pid index ==14
                        p = pid_history_index;
                    }
                    for (i = 0; i <= p ; i++)
                    {
                    printf("[%d] %d\n", i+1, pid_history[i]);
                    }
                }
                else
                {
                    pid_t pid = fork();

                    // save the pid in to pid history array
                    if (pid_history_index > 14 )
                    {
                        pid_history_index = 0;
                        p_h_i = 1;
                    }

                    if (pid_history_index < 14)
                    {
                        pid_history[pid_history_index] = pid;
                        pid_history_index++;

                    }
                    else if ((history_index == 14)&& (p_h_i == 0))
                    {

                        pid_history[pid_history_index] = pid;
                        p_h_i = 1;
                    }
                    else
                    {

                        for (i = 0 ; i < 14; i++)
                            {
                                pid_history[i] = pid_history[i+1];
                            }
                        pid_history[pid_history_index] = pid;
                    }

                    if( pid == 0 )
                    {
                        int ret = execvp( token[0],&token[0]);

                        if( ret == -1 )
                        {
                            printf("%s: Command not found.\n", token[0]);
                        }

                        exit( EXIT_SUCCESS );
                    }
                    else
                    {
                        int status;
                        waitpid( pid, &status, 0 ); // wait for the child to exit
                        fflush(NULL);
                    }
                }
            }

         free( working_root );

        }


    }


    for (i = 0; i < history_index ; i++) //free history array
    {
         free(history[i]);
    }

    return 0;
}
