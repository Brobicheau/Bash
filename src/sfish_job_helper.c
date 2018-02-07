#include "sfish.h"



void excecute_job(job *j, int foreground){

    process *current_process = malloc(sizeof(process));
    int new_pipe[2];
    int in;
    int out;
    pid_t pid;


    current_process = j->process_head;
    in = current_process->stdin;


/*if(current_process->next != NULL) {
            if(pipe(new_pipe) < 0){
                //print error
            }
            out = new_pipe[1];
        }
        else {
            out = current_process->stdout;
        }
    excecute_process(current_process, j->pgid, in, out, current_process->stderr, foreground);*/

    while(current_process != NULL){

        if(current_process->next != NULL) {
            if(pipe(new_pipe) < 0){
                //print error
            }
            out = new_pipe[1];
        }
        else {
            out = current_process->stdout;
        }

        if((pid = fork()) < 0){
            //print error
            exit(EXIT_FAILURE);
        }
        else if(pid == 0){

            excecute_process(current_process, j->pgid, in, out, current_process->stderr, foreground);

        }
        else {
            //parent process stuff

            current_process->pid = pid;
            if (j->pgid == 0){
                j->pgid = pid;
                setpgid(pid, j->pgid);

            }
           // close(in);
            //close(out);
            //close(current_process->stderr);

            
        }

        if(in != current_process->stdin){
            close(in);
        }
        if(out != current_process->stdout){
            close(out);
        }
        in = new_pipe[0]; 
        current_process = current_process->next;

    }
    //close(new_pipe[0]);
    //close(new_pipe[1]);


    if(foreground){
        set_job_foreground(j, CONTINUE);
    }
    else if (!foreground){
        set_job_background(j, CONTINUE);
    }

    //free(current_process);
    //DEAL WITH JOB STUFF HERE
}

void excecute_process(process *p, pid_t pgid, int in, int out, int err, int foreground){

    pid_t pid;
    sigset_t mask, prev;
    sigfillset(&mask);
    signal(SIGTTOU, SIG_IGN);
    //sigdelset(&mask, SIGCHLD);
    //sigdelset(&mask, SIGTTOU);


    pid = getpid();

    if(pgid == 0){
        pgid = pid;
    }
    setpgid(pid, pgid);
    if(foreground){
      tcsetpgrp(sfish_fd, pgid);
    }
    sigprocmask(SIG_UNBLOCK, &mask, &prev);
    set_fd(in, out, err);
    if(is_builtin(p->argv) >0){
        run_builtin(p->argv);
        exit(1);
    }
    else{
        execv(p->argv[0], p->argv);
        exit(1);
    }
}

int excecute(char** argv){

    struct stat buf;
    int st;

    if((st = stat(argv[0], &buf)) == 0){
        return 0;
    }
    else {
        //print error
        return -1;
    }



}


void get_fd(char *arg, char *re_type, process *j){

    char out_path[SIZE];
    char cwd[SIZE];
    int fd;
    getcwd(cwd, SIZE);

    if(strcmp(re_type, RE_OUT) == 0){
        strcpy(out_path, cwd);
        strcat(out_path, "/");
        strcat(out_path, arg);
        fd = open(out_path, O_RDWR | O_CREAT, 0644);
        j->stdout = fd;
    }
    else if (strcmp(re_type, RE_IN) == 0){
        strcpy(out_path, cwd);
        strcat(out_path, "/");
        strcat(out_path, arg);        
        fd = open(out_path, O_RDONLY, 0);
        j->stdin = fd;
    }
    else if (strcmp(re_type, RE_ERR) == 0){
        strcpy(out_path, cwd);
        strcat(out_path, "/");
        strcat(out_path, arg);
        fd = open(out_path, O_RDWR | O_CREAT, 0644);
        j->stderr = fd;
    }
}


void set_fd(int stdin, int stdout, int stderr){

    if(stdin != STDIN_FILENO){
        dup2(stdin, 0);
        close(stdin);
    }
    if(stdout != STDOUT_FILENO){
        dup2(stdout, 1);
        close(stdout);
    }
    if(stderr != STDERR_FILENO){
        dup2(stderr, 2);
        close(stderr);
    }
}

void add_to_list(job *j){

    job *current_j;

    current_j = job_head;

    while(current_j->next != NULL){
        current_j = current_j->next;
    }
    current_j->next = j;
}




int create_job(job *j, char *command){


    char *cmd = malloc(SIZE);
    memset(cmd, 0, SIZE);
    int c;
    int i = 0;
    char* cmdargv[SIZE];
    for(i = 0; i < SIZE; i++){
        cmdargv[i] = malloc(64);
    }
    int cmdargc;    
    process *current_tail = {0};



    strcpy(cmd, command);
    cmdargc = parse_cmd(cmd, cmdargv);
    process *p[cmdargc];
    for(i = 0; i < cmdargc; i++){
        p[i] = calloc(1,sizeof(process));
    }


    for(i = 0; i < cmdargc; i++){

        p[i]->argv =(char**) malloc(128);
        
        p[i]->stdin = STDIN_FILENO;
        p[i]->stdout = STDOUT_FILENO;
        p[i]->stderr = STDERR_FILENO;
        if((c = get_argvs(cmdargv[i], p[i]->argv, p[i])) == 2){
            return 0;
        }
        else if (c  == -1){
            remove_last_job();
            return 0;
        }




        if(j->process_head == NULL) {
            j->process_head = p[i];
            current_tail = p[i];
        }
        else {
            current_tail->next = p[i];
            current_tail = p[i];
        }
    }


    return 1;

}

void check_for_reap(){
    int status;
    int pid;

    /*WRITE FUNCTION TO REOVE JOB FROM LSIT RIGHT AWAY*/
    /*PROBLEM IS HAPPENING WHEN 


    Program received signal SIGSEGV, Segmentation fault.
    0x0000000000403c71 in remove_job (j=0x637d20) at src/sfish_job_helper.c:396
    396         while(test_j->next != j || test_j->next != NULL){
*/
    pid = waitpid(-1, &status, WNOHANG | WUNTRACED);
    while(pid > 0){
        update_process(pid, status);
        pid = waitpid(-1, &status, WNOHANG | WUNTRACED);  
    }
}

void wait_until_done(job *j){

    int status;
    pid_t pid;

    /*PROBLEMS PROBABLY HAPPENING HERE*/
    pid = waitpid(-j->pgid, &status, WUNTRACED);
        update_process(pid, status);

    while(errno != ECHILD && !check_if_done(j)){

        update_process(pid, status);
        pid = waitpid(-j->pgid, &status, WUNTRACED);
    }
    last_exit = WEXITSTATUS(status);
    remove_job(j);
  
}


void update_process(pid_t pid, int status){

    process *p = get_process(pid);


    if(p != NULL){
            p->status = status;

        if( WIFSTOPPED(status)){
            p->stopped = 1;
        }
        else{
            p->completed = 1;
            last_exit = WEXITSTATUS(status);
            if(WIFSIGNALED(status)){
               // fprintf(stderr,"process %i: Stopped by signal\n", pid);
            }
        }
    }


}


int check_if_done(job *j){

    process *p = j->process_head;


    if(p != NULL ){

        while(p != NULL){
            if(!p->completed){
                return 0;
            }
            p = p->next;
        }
        return 1;
    }
    else {
       // fprintf(stderr, "job with no processes\n");
        return -1;
    }
}


process *get_process(pid_t pid){

    job *j;
    process *p;

    j = job_head;

    while(j != NULL){
        p = j->process_head;
        while(p != NULL){
            if(p->pid == pid){
                return p;
            }
            p = p->next;
        }
        j = j->next;
    }
    return NULL;
}

void set_job_info(job *j){




}
void remove_last_job() {

    job *j;
    job *j_prev;
    j = job_head;

    while(j->next != NULL){

        j_prev = j;
        j = j->next;
    }

    if(j == job_head){
        job_head = NULL;
    }
    else {
        j_prev->next = NULL;
    }

    free(j);
}

void remove_job(job *j){

    job *test_j;
    test_j = job_head;

    if(j == job_head) {
        if(job_head->next == NULL)
            job_head = NULL;
        else
            job_head = job_head->next;
    }
    else if ( job_head == NULL){

    }
    else {
        while(test_j->next != j && test_j->next != NULL){
            test_j = test_j->next;
        }

        if(j->next != NULL){
            test_j->next = j->next;
        }
        else{
            test_j->next = NULL;
        }
    }
}



void child_handler (int sig){
    signal(SIGCHLD, SIG_IGN); 
    check_for_reap();       
    signal(SIGCHLD, SIG_DFL);
}


void update_all_processes(){

     pid_t pid;
     int status; 

     while((pid = waitpid(-1, &status, WNOHANG)) != 0);
     if(pid != -1 && pid != 0)
        update_process(pid, status);
}


void update_jobs() {

    update_all_processes();

    job *j = job_head;

    while( j != NULL){
        printf("in while\n");

        if(check_if_done(j)){
            printf("removing job");
            display_updated_job(j);
            //killpg(j->pgid, 9);
            remove_job(j);
        }
        else  if(check_if_stopped(j)){
            display_updated_job(j);
        }

        j = j->next;
    }
}

void display_updated_job(job *j){


        fprintf(stdout, " %i", j->pgid);
        fprintf(stdout, " %s", j->command);
        fprintf(stdout, "\n");


}

int check_if_stopped() {
    return 0;
}

