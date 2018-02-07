#include "sfish.h"



int get_argvs(char *arg, char** argv, process *j){

    char current_arg[SIZE];
    char *token;
    char *next_token;
    char *tok_trk;
    int i;
    int argc;
    int bt;


    i = 0;
    strcpy(current_arg, arg);
    token = strtok_r(current_arg, " ", &tok_trk);

    while(token != NULL){

         if (strcmp(token, ">") == 0
        || strcmp(token, "<") == 0
        || strcmp(token, "2>") == 0){
            next_token = strtok_r(NULL, " ", &tok_trk);
            if(next_token !=NULL){
                get_fd(next_token, token, j);
                strcpy(token, next_token);
            }
            else{
                errno = 1;
                perror("Missing Argument\n");
            }
            i++;
        }
        else {
            argv[i] = malloc(strlen(token));
            strcpy(argv[i], token);
            argc++;
        }

        token = strtok_r(NULL, " ", &tok_trk);

        i++;
    }


    if((bt = is_builtin(argv)) < 0){
        if(check_for_bin(argv) == -1){
            fprintf(stderr, "%s: could not find command\n", argv[0]);
            return -1;
        }
    }
    else if(bt == 0){
        return 2;
    }
    else if (bt > 0){
        return 1;
    }

    return 0;
}


int parse_cmd(char *cmd, char **cmdargv) {

    char *current_arg;
    char *tok_trk;
    int i = 0;
    char cmd_tokenizer[SIZE];

    strcpy(cmd_tokenizer, cmd);
    current_arg = strtok_r(cmd_tokenizer, DELIM, &tok_trk);

    while( current_arg!= NULL){

        cmdargv[i] = current_arg;
        i++;
        current_arg = strtok_r(NULL, DELIM, &tok_trk);
    }

    return i;

}

void set_prompt(char * cursor_start){

    char cursor_end[SIZE];
    char user_name[SIZE];
    char cwd[SIZE];
    char host_name[SIZE];
    int dp;

    memset(cursor_start,0, SIZE);

    if((getcwd(cwd, SIZE)) == NULL){
        fprintf(stderr, "Could not get current working directory\n");
        exit(EXIT_FAILURE);
        /*PRINT ERROR MESSAGE AND STUFF*/
    }      

    if(user_display_toggle == 1 || machine_display_toggle == 1 ){
        strcpy(cursor_start,  "sfish-" );
    }
    else 
        strcpy(cursor_start,  "sfish" );


    if(user_display_toggle == 1){

        strcpy(user_name, getenv(USER)); 
        set_color(user_name, USER_NAME);
        strcat(cursor_start, user_name);
    }

    if(machine_display_toggle == 1){
        if((dp = gethostname(host_name, SIZE)) == 0){

            if(user_display_toggle == 1)
                strcat(cursor_start, "@");

            set_color(host_name, MACHINE_NAME);
            strcat(cursor_start, host_name);

        }
        else {

        }
    }

    if(strcmp(cwd, getenv(HOME)) == 0){
        strcpy(cwd, "~");
    }
    strcpy(cursor_end ,"> ");
    strcat(cursor_start, ":[");
    strcat(cursor_start, cwd);
    strcat(cursor_start, "]");    
    strcat(cursor_start, cursor_end);
    strcat(cursor_start, KNRM);

}

int check_foreground(char *cmd){

  size_t length;
  int i;
  char *temp = malloc(SIZE);
  length = strlen(cmd);
  if((cmd[length-1] == '&')){
    for(i = 0; i < length -2; i++){
        temp[i] = cmd[i];
    }
    strcpy(cmd, temp);
    return 0;
  }  
  else {
    return 1;
  }

} 


int check_for_bin(char ** argv) {


    int i;
    int flag;
    size_t length;
    char arg[SIZE];
    strcpy(arg,argv[FIRST]);
    length = strlen(*argv);


        i = 0;
        flag = 0;
        while(i < length && flag == 0 ){
            if(arg[i] == '/'){
                flag = 1;
            }
            i++;
        }

        if(flag == 1){
            return excecute(argv);
        }
        else  {
            return build_and_excecute(argv);
        }
}
