/* pqsh.c: Process Queue Shell */

#include "pqsh/macros.h"
#include "pqsh/options.h"
#include "pqsh/scheduler.h"
#include "pqsh/signal.h"

#include <errno.h>
#include <string.h>
#include <sys/time.h>

/* Global Variables */

Scheduler PQShellScheduler = {
    .policy    = FIFO_POLICY,
    .cores     = 1,
    .timeout   = 250000,
};


/* Help Message */

void help() {
    printf("Commands:\n");
    printf("  add    command    Add command to waiting queue.\n");
    printf("  status [queue]    Display status of specified queue (default is all).\n");
    printf("  help              Display help message.\n");
    printf("  exit|quit         Exit shell.\n");
}

/* Main Execution */

int main(int argc, char *argv[]) {
    Scheduler *s = &PQShellScheduler;

    /* TODO: Parse command line options */
    if (!parse_command_line_options(argc, argv, s))
        return EXIT_SUCCESS;

    /* TODO: Register signal handlers */
    signal_register(SIGALRM, 0, sigalrm_handler);
    
    /* TODO: Start timer interrupt */
    struct itimerval interval = {
        .it_interval = { .tv_sec = 0, .tv_usec = s->timeout },
        .it_value    = { .tv_sec = 0, .tv_usec = s->timeout },
    };
    if (setitimer(ITIMER_REAL, &interval, NULL) < 0) 
    {
        fprintf(stderr, "fail to set timer\n");
        exit(1);
    }

    /* TODO: Process shell comands */
    while (!feof(stdin)) {
        char command[BUFSIZ]  = "";
        char argument[BUFSIZ] = "";

        printf("\nPQSH> ");

        while (!fgets(command, BUFSIZ, stdin) && !feof(stdin));

        chomp(command);
        
        /* TODO: Handle add and status commands */
        if (streq(command, "help")) {
            help();
        } else if (streq(command, "exit") || streq(command, "quit")) {
            // clear resource
            Process * p;
            
            while (p = queue_pop(&s->running))
            {
                kill(p->pid, SIGKILL);
                Process * temp = p->next;
                free(p);
                p = temp;
            }

            while (p = queue_pop(&s->waiting))
            {
                kill(SIGKILL, p->pid);
                Process * temp = p->next;
                free(p);
                p = temp;
            }


            while (p = queue_pop(&s->finished))
            {
                Process * temp = p->next;
                free(p);
                p = temp;
            }

            break;
        } else if (strlen(command)) {
            char * delim = strchr(command, ' ');
            char * args = NULL;
            if (delim)
            {
                *delim = '\0';
                args = delim + 1;
            }
            if (streq(command, "add"))
                scheduler_add(s, stdout, args);
            else if (streq(command, "status"))
            {
                int queue;
                if (!args)
                    queue = 7;
                else if (streq(args, "running"))
                    queue = RUNNING;
                else if (streq(args, "waiting"))
                    queue = WAITING;
                else if (streq(args, "finished"))
                    queue = FINISHED;
                else
                {
                    fprintf(stderr, "Unknown queue: %s\n", args);
                    continue;
                }

                scheduler_status(s, stdout, queue);
            }
            else
                printf("Unknown command: %s\n", command);
        }
    }

    return EXIT_SUCCESS;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
