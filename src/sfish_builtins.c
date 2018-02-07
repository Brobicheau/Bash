#include "sfish.h"

void cd(char **cmdargv){

      char cwd[SIZE];
      char old_dir[SIZE];
      char child_dir[SIZE];
      int ch;  

    if((getcwd(cwd, SIZE)) == NULL){
        exit(EXIT_FAILURE);
        /*PRINT ERROR MESSAGE AND STUFF*/
    }

    if(cmdargv[SECOND] == 0){
        if((ch = chdir(getenv(HOME))) != 0){
            fprintf(stderr, "Could not navtigate to home direcotry\n");
        }
        else{
            strcpy(prev_dir, cwd);
        }
    }
    else if(strcmp(cmdargv[SECOND], "-") == 0){
        if(prev_dir != NULL){
            if((ch = chdir(prev_dir)) != 0){
                fprintf(stderr, "Error when navigating to previous directory\n");
            }
            else {
                strcpy(prev_dir, cwd);
            }
        }
        else {
            fprintf(stderr, "No previous directory\n");
        }
    }
    else {
        /*WHAT DOES A SINGLE DOT DO?*/
        memset(child_dir, 0, SIZE);
        strcpy(child_dir, "/");
        strcat(child_dir, cmdargv[SECOND]);
        strcpy(old_dir, cwd);
        strcat(cwd, child_dir);

        if((ch = chdir(cwd)) != 0){
            fprintf(stderr, "%s is not a valid directory\n", cwd);
        }
        else {
            strcpy(prev_dir, old_dir);
        }
    }

}

void pwd() {

    char cwd[SIZE];

    if((getcwd(cwd, SIZE)) == NULL){
        exit(EXIT_FAILURE);
        /*PRINT ERROR MESSAGE AND STUFF*/
    }

    fprintf(stdout, "%s\n", cwd);


}


void chpmt(char** cmdargv) {


    if(strcmp(cmdargv[SECOND], USER_NAME) == 0){
        if(strcmp(cmdargv[THIRD], "1")==0) {
           user_display_toggle = 1;
        }
        else if(strcmp(cmdargv[THIRD], "0")==0) {
           user_display_toggle = 0;

        }
        else {
            fprintf(stderr, "%s: command not found", cmdargv[THIRD] );
        }
    }
    else if (strcmp(cmdargv[SECOND], MACHINE_NAME) == 0){

        if(strcmp(cmdargv[THIRD], "1")== 0){
            machine_display_toggle = 1;
        }
        else if (strcmp(cmdargv[THIRD], "0")==0){
            machine_display_toggle = 0;

        }
        else {
            fprintf(stderr, "%s: command not found", cmdargv[THIRD] );
        }
    }
    else {
        fprintf(stderr, "%s: command not found", cmdargv[SECOND] );
    }
}


void set_color(char *to_color, char *target) {

    char *colors[COLOR_SIZE] = {RED, BLUE, GREEN, YELLOW, WHITE, MAGENTA, CYAN};
    char *color_set[COLOR_SIZE] = {KRED, KBLU, KGRN, KYEL, KWHT, KMAG, KCYN};
    char temp[SIZE] = "";
    int i;
    int flag;

    if(strcmp(target, USER_NAME) == 0){

        if(user_bold_toggle == 1){
            strcat(temp, BLON);
        }        

        flag = 0;
        i = 0;
        while(i < COLOR_SIZE && flag == 0){

            if(strcmp(user_color, colors[i]) == 0 ){
                strcat(temp, color_set[i]);
                strcat(temp, to_color);
                strcpy(to_color, temp);
                strcat(to_color, KNRM);
                flag = 1;
            }
            i++;
        }



        if(machine_bold_toggle == 1){
            strcat(to_color, BLOF);
        }              

    }     
    else if( strcmp(target, MACHINE_NAME) == 0){

            if(machine_bold_toggle == 1){
                strcat(temp, BLON);
            }      

        i = 0;
        flag =0;    
        while(i < COLOR_SIZE && flag == 0){

            if(strcmp(machine_color, colors[i]) == 0 ){
                strcat(temp, color_set[i]);
                strcat(temp, to_color);
                strcpy(to_color, temp);
                strcat(to_color, KNRM);
                flag = 1;
            }
            i++;
        }
 

        if(machine_bold_toggle == 1){
            strcat(to_color, BLOF);
        }                  
    }

}


void chclr(char ** cmdargv) {

    char *settings_choice = cmdargv[SECOND];
    char *color_choice = cmdargv[THIRD];
    char *bold_choice = cmdargv[FOURTH];
    char *colors[COLOR_SIZE] = {RED, BLUE, GREEN, YELLOW, WHITE, MAGENTA, CYAN};
    int i;
    int flag;

    if(strcmp(settings_choice, USER_NAME) == 0){

        if(strcmp(bold_choice, "1") == 0) { user_bold_toggle = 1;}
        else if(strcmp(bold_choice, "0") == 0) {user_bold_toggle = 0;}
        else { fprintf(stderr, "%s: command not found \n", bold_choice);}    

        i = 0;
        flag = 0;
        while(i < COLOR_SIZE && flag == 0){

            if(strcmp(color_choice, colors[i]) == 0){
                strcpy(user_color, colors[i]);
                flag = 1;
            }
            i++;
        }
        if(flag == 0){
            fprintf(stderr, "%s: command not found \n", color_choice);
        }
                

    }
    else if (strcmp(settings_choice, MACHINE_NAME) == 0){

        if(strcmp(bold_choice, "1") == 0) { machine_bold_toggle = 1;}
        else if(strcmp(bold_choice, "0") == 0) {machine_bold_toggle = 0;}
        else { fprintf(stderr, "%s: command not found \n", bold_choice);}            

        i = 0;
        flag = 0;
        while(i < COLOR_SIZE && flag == 0){

            if(strcmp(color_choice, colors[i]) == 0){
                strcpy(machine_color, colors[i]);
                flag = 1;
            }
            i++;
        }
        if(flag == 0){
            fprintf(stderr, "%s: command not found \n", color_choice);
        }     
    }                

}


void run_builtin(char **argv){

        if(argv[FIRST] == NULL){
            fprintf(stderr, "Invalid Input. type \"help\" for assistance\n");

        }
        else if (strcmp(argv[FIRST], EXIT)== 0){
            exit(EXIT_SUCCESS);
        }

        else if (strcmp(argv[FIRST], HELP)== 0){
            print_help();
        }

        else if (strcmp(argv[FIRST], PWD) == 0){
            pwd();
        }
        else if (strcmp(argv[FIRST], PRT) == 0){
            prt();
        }
        else if(strcmp(argv[FIRST], JOBS) == 0){
            jobs();
        }


}


int is_builtin(char **cmdargv){

            job *j = NULL;

        if(cmdargv[FIRST] == NULL){
            fprintf(stderr, "Invalid Input. type \"help\" for assistance\n");
            return 0;

        }
        else if (strcmp(cmdargv[FIRST], EXIT)== 0){
            exit(EXIT_SUCCESS);
            return 0;
        }

        else if (strcmp(cmdargv[FIRST], HELP)== 0){
            return 1;
        }

        else if (strcmp(cmdargv[FIRST],CD) == 0){
            cd(cmdargv);
            return 0;
        
        }
        else if (strcmp(cmdargv[FIRST], PWD) == 0){
            //pwd();
            return 1;
        }
        else if (strcmp(cmdargv[FIRST], PRT) == 0){
            //prt();
            return 1;
        }
        else if (strcmp(cmdargv[FIRST], CHPMT) == 0){
            chpmt(cmdargv);
            return 0;
        }
        else if (strcmp(cmdargv[FIRST], CHCLR) == 0){
           chclr(cmdargv);
            return 0;
        }
        else if(strcmp(cmdargv[FIRST], JOBS) == 0){
            //jobs();
            return 2;
        }
     
        else if (strcmp(cmdargv[FIRST], FG) == 0){
            if(is_find_by_jid(cmdargv[SECOND])){
               // printf("\n\nFINDING BY JID\n\n");
                j = find_by_jid(cmdargv[SECOND]);

            }
            else {
                j =find_by_pid(cmdargv[SECOND]);
            }

            if(j != NULL){

                set_job_foreground(j, 1);
            }
            return 0;
        }
        else if (strcmp(cmdargv[FIRST], BG) == 0){
            if(is_find_by_jid(cmdargv[SECOND])){
                j = find_by_jid(cmdargv[SECOND]);
            }
            else {
                j = find_by_pid(cmdargv[SECOND]);
            }      
            if(j != NULL)      
                set_job_background(j, 0);
            return 0;
        }        
        else if(strcmp(cmdargv[FIRST], KILL) == 0){
            sf_kill(cmdargv);
            return 0;
        }
        else 
            return -1;

}


int is_find_by_jid(char *arg){

    size_t length = strlen(arg);
    int i;
    int flag = 0;
    char temp[SIZE];
    memset(temp, 0 , SIZE);



    for(i = 0; i < length; i++){
        if(arg[i] == '%'){
            arg[i] = arg[i+1];
            arg[i+1] = '\0';
            flag =1;
        }

    }
    if(flag){
        return 1;
    }
    return 0;


}

job *find_by_pid(char *arg){

    job *j = malloc(sizeof(job));
    long int pid;
    pid = strtol(arg, NULL, 10);
    j = job_head;


    if(j != NULL){
        if(j->pgid == pid){
            return j;
        }
    }
    return NULL;

}

job *find_by_jid(char *arg){

    job *j = malloc(sizeof(job));
    int count = 1;
    long int jid = strtol(arg, NULL, 10);


    j = job_head;

    while( count != jid && j != NULL){
        j = j->next;
        count++;
    }
    if (j == NULL){
        return NULL;
    }

    return j;

}

void sf_kill(char **command){

    int signum;
    int i = 0;
    while(command[i] != NULL){
        i++;
    }
    job *j = NULL;

    if(i == 2){
        if(is_find_by_jid(command[SECOND])){
            j = find_by_jid(command[SECOND]);
            killpg(j->pgid, SIGTERM);
            remove_job(j);

        }
        else {
            j = find_by_pid(command[SECOND]);
            killpg(j->pgid, SIGTERM);
            remove_job(j);
        }

    }
    else if (i == 3){

        if(is_find_by_jid(command[THIRD])){
            j = find_by_jid(command[THIRD]);
            signum = strtol(command[SECOND], NULL, 10);
            killpg(j->pgid, signum);
            if(signum == 3 
                || signum == 2
                || signum == 9
                || signum == 11) {
                remove_job(j);
            }

        }
        else {
            j = find_by_pid(command[THIRD]);
            signum = strtol(command[SECOND], NULL, 10);            
            killpg(j->pgid, signum);
            if(signum == 3 
                || signum == 2
                || signum == 9
                || signum == 11) {
                remove_job(j);
            }

        }
    }

}
void print_help(){
    printf("\thelp\n");
    printf("\texit\n");
    printf("\tcd [new directory]\n");
    printf("\tpwd\n");
    printf("\tprt\n");
    printf("\tchpmt SETTINGS TOGGLE\n");
    printf("\tchclr SETTINGS COLOR BOLD\n");
    printf("\tjobs\n");
    printf("\tfg PID|JID\n");
    printf("\tbg PID|JID\n");
    printf("\tkill [signal] PID|JID\n");
    printf("\tdisown[PID|JID]\n");

}

void prt() {

    if(last_exit < 0){
        printf("\nno status to report\n");
    }
    else {
        printf("\nexit status of last process run: %i\n", last_exit);
    }

} 