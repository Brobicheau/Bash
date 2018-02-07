#include "sfish.h"

int user_display_toggle = 1;
int machine_display_toggle = 1;
int user_bold_toggle = 0;
int redirect_flag;
int machine_bold_toggle = 0;
int sfish_fd;
pid_t sfish_pgid;
char user_color[SIZE] = WHITE;
char machine_color[SIZE] = WHITE;
char prev_dir[SIZE];
job *job_head = NULL;
int last_exit = -1;





int main(int argc, char** argv) {
 .
    rl_catch_signals = 0;
    char *cmd;
    char prompt[SIZE];
    int foreground;
    job *new_job;


    job_head = calloc(1,sizeof(job));
    set_prompt(prompt);

   
    init_sfish();

    while((cmd = readline(prompt)) != NULL ) {
            signal(SIGCHLD, SIG_IGN);



        if(strcmp(cmd, "") != 0 ){
            if(job_head == NULL){
                job_head = calloc(1,sizeof(job));
                job_head->next = NULL;
                new_job = job_head;
            }
            else{
                new_job = calloc(1,sizeof(job));
                add_to_list(new_job);
                
                new_job->next = NULL;
            }
            strcpy(new_job->command, cmd);
            foreground = check_foreground(cmd);
            if(create_job(new_job, cmd)){
                excecute_job(new_job, foreground);
            }
        }




        new_job = job_head;
        check_for_reap();
        while(new_job != NULL){
            if(check_if_done(new_job)){
                remove_job(new_job);
            }
            new_job = new_job->next;
        }
        
        set_prompt(prompt);
        signal(SIGCHLD, SIG_DFL);
        signal(SIGCHLD, child_handler);



    }

    //Don't forget to free allocated memory, and close file descriptors.
    free(cmd);
    free(job_head);
   

    printf(KNRM);

    return EXIT_SUCCESS;
}


void handler(int sig) {


    if(sig == SIGINT)
        kill(getpid(), SIGINT);
    else if (sig == SIGTSTP)
        kill(getpid(),SIGTSTP);
}


int build_and_excecute(char **argv) {


    char *path;
    char edit_path[SIZE];
    char *test_paths[SIZE];
    char *test;
    char *tok_trk = NULL;
    struct stat buf;
    int i = 0;
    int j = 0;
    int st;
    test = malloc(sizeof(char*));
    char *new_arg = malloc(sizeof(char*));
    memset(test_paths, 0 ,SIZE);

    i = 0;
    path = getenv(PATH);
    strcpy(edit_path, path);
    test_paths[i] = strtok_r(edit_path, ":", &tok_trk);
    while(test_paths[i] != NULL){
        i++;
        test_paths[i] = strtok_r(NULL, ":", &tok_trk);
    }
    j = 0;
    while(j < i){
        strcpy(test, test_paths[j]);
        strcat(test, "/");
        strcat(test, argv[0]);
        if((st = stat(test, &buf)) == 0) {
            strcpy(new_arg, test);
            argv[0] = new_arg;
            return 0;
        }
        j++;
    }
    return -1;
}




void jobs(){

    job *j = job_head;
    char * note;
    int count = 1;

    while(j != NULL){
        if(j->pgid != 0){
            if(j->notified){
                note = "Stopped";
            }
            else {
                note = "Running";
            }
            printf("[%i]", count);
            printf(" %s", note);
            printf(" %i", j->pgid);
            printf(" %s", j->command);
            printf("\n");

     
        }
        count++;
        j = j->next;        
    }

}

void init_sfish() {


    sigset_t mask, prev;
    sigfillset(&mask);
    sigdelset(&mask, SIGCHLD);

    sigprocmask(SIG_BLOCK, &mask, &prev);


    sfish_fd = STDOUT_FILENO;
    sfish_pgid = getpgid(sfish_pgid);
    tcsetpgrp(sfish_fd, sfish_pgid);

}

void set_job_foreground(job *j, int is_continue){

    tcsetpgrp(sfish_fd, j->pgid);

    if(is_continue){
        kill(j->pgid, SIGCONT);
    }

    wait_until_done(j);

    tcsetpgrp(sfish_fd, sfish_pgid);

}

void set_job_background(job *j, int is_continue){

    if(is_continue){
        kill(j->pgid, SIGCONT);
    }


}

int is_in_list(job *j){

    job *temp_j;
    if(job_head != NULL){
        temp_j = job_head;
        while (temp_j != NULL){
            if(temp_j == j){
                return 1;
            }
            else {
                temp_j = temp_j->next;
            }
        }   
        return 0;
    }
    return 0;
}



