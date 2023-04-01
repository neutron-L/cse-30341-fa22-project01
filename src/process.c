/* process.c: PQSH Process */

#include "pqsh/macros.h"
#include "pqsh/process.h"
#include "pqsh/timestamp.h"

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

/**
 * Create new process structure given command.
 * @param   command     String with command to execute.
 * @return  Pointer to new process structure
 **/
Process *process_create(const char *command) {
    /* TODO: Implement */
    Process * pcb = (Process *)malloc(sizeof(Process));
    pcb->next = NULL;

    strcpy(pcb->command, command);

    pcb->pid = 0;
    pcb->arrival_time = timestamp();
    pcb->start_time = 0.0;
    pcb->end_time = 0.0;

    return pcb;
}

/**
 * Start process by forking and executing the command.
 * @param   p           Pointer to Process structure.
 * @return  Whether or not starting the process was successful
 **/
bool process_start(Process *p) {
    /* TODO: Implement */
    pid_t pid = fork();
    if (pid == 0)
    {
        // parse command
        int argc = 0;
        char buf[BUFSIZ];
        char * argv[MAX_ARGUMENTS];
        
        strcpy(buf, p->command);
        char * delim;
        char * pb = buf;

        while (*pb && *pb == ' ')
            ++pb;
        
        while ((delim = strchr(pb, ' ')))
        {
            *delim = '\0';
            argv[argc++] = buf;
            pb = delim + 1;

            while (*pb && *pb == ' ')
                ++pb;    
        }
        argv[argc] = pb;

        execve(argv[0], argv, NULL);

        return true;
    }
    else if (pid > 0)
    {
        p->pid = pid;
        p->start_time = timestamp();
        
        return true;
    }
    else
        return false;
}

/**
 * Pause process by sending it the appropriate signal.
 * @param   p           Pointer to Process structure.
 * @return  Whether or not sending the signal was successful.
 **/
bool process_pause(Process *p) {
    /* TODO: Implement */
    return kill(p->pid, SIGSTOP) == 0;
}

/**
 * Resume process by sending it the appropriate signal.
 * @param   p           Pointer to Process structure.
 * @return  Whether or not sending the signal was successful.
 **/
bool process_resume(Process *p) {
    /* TODO: Implement */
    return kill(p->pid, SIGCONT) == 0;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
