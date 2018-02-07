#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define DELIM "|"
#define HOME "HOME"
#define USER "USER"
#define PATH "PATH"
#define COLOR_SIZE 7
#define SIZE 1024
#define FIRST 0
#define SECOND 1
#define THIRD 2
#define FOURTH 3
#define CONTINUE 0
#define STOP 1


#define EXIT "exit"
#define HELP "help"
#define	CD "cd"
#define PWD "pwd"
#define CHPMT "chpmt"
#define CHCLR "chclr"
#define PRT "prt"
#define JOBS "jobs"
#define BG "bg"
#define FG "fg"
#define KILL "kill"


#define USER_NAME "user"
#define MACHINE_NAME "machine"

#define KNRM  "\001\x1B[0m\002"
#define KRED  "\001\x1B[1;31m\002"
#define KGRN  "\001\x1B[1;32m\001"
#define KYEL  "\001\x1B[1;33m\002"
#define KBLU  "\001\x1B[1;34m\002"
#define KMAG  "\001\x1B[1;35m\002"
#define KCYN  "\001\x1B[1;36m\002"
#define KWHT  "\001\x1B[1;37m\002"
#define KBWN  "\001\x1B[0;33m\002"
#define BLON  "\001\x1B[01;1m\002"
#define BLOF  "\001\x1B[01;22m\002"


#define RED "red"
#define GREEN "green"
#define BLUE "blue"
#define YELLOW "yellow"
#define MAGENTA "magenta"
#define CYAN "cyan"
#define WHITE "white"

#define RE_OUT ">"
#define RE_IN "<"
#define RE_ERR "2>"
#define PIPE 4



struct process {
	struct process *next;
	char **argv;
	pid_t pid;
	char completed;
	char stopped;
	int status;
	int stdin, stdout, stderr;


};
typedef struct process process;

struct job {

	struct job *next;
	char command[SIZE];
	process *process_head;
	pid_t pgid;
	char notified;
} ;
typedef struct job job;




extern int user_display_toggle;
extern int machine_display_toggle;
extern int user_bold_toggle;
extern int machine_bold_toggle;
extern int sfish_fd;
extern char user_color[SIZE];
extern char machine_color[SIZE];
extern char prev_dir[SIZE];
extern job *job_head;
extern pid_t sfish_pgid;
extern int last_exit;





int parse_cmd(char *cmd, char **cmdargv);

void set_prompt(char *cursor_start);

void exit_shell();

void cd(char **cmdargv);

void print_help();

void pwd();

void chpmt(char **cmdargv);

void set_color(char *to_color, char *target);

void chclr();

void prt();

int check_for_bin(char ** cmdargv);

int is_builtin(char **cmdargv);

int build_and_excecute(char **argv);

void get_fd(char *arg, char*re_type, process *j);

int get_argvs(char *arg, char** argv, process *j);

int create_job(job *j, char *command);

void excecute_job(job *j, int foreground);

int excecute(char **argv);

void set_fd(int stdin, int stdout, int stderr);

void excecute_process(process *p, pid_t pgid, int in, int out, int err, int foreground);

void add_to_list(job *j);

void run_builtin();

int check_foreground(char*);

void init_sfish();

void jobs();

void set_job_foreground(job *j, int conntinue);

void set_job_background(job *j, int conntinue);

void wait_until_done(job *j);

int check_if_done(job *j);

void update_process(pid_t pid, int status);

process *get_process(pid_t pid);

void set_job_info(job * j);

void remove_last_job();

void remove_job(job *j);

void handler(int sig);

void child_handler(int sig);

int is_in_list(job *j);

void update_jobs();

void display_updated_job(job *j);

int check_if_stopped();

void update_all_processes();

void check_for_reap();

int is_find_by_jid(char *arg);

job *find_by_jid(char *arg);

job *find_by_pid(char *arg);

void sf_kill(char **command);

void print_help();


