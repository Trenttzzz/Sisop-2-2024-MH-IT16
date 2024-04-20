#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <syslog.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LOG_FILE "history.log"
#define BACKUP_FOLDER "library/backup/"

char *get_extension(const char *filename) {
    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

char *rename_file(const char *filename) {
    char *extension = get_extension(filename);
    char *new_name = malloc(strlen(filename) + 10); // Max new filename length
    if (strstr(filename, "r3N4mE")) {
        if (strcmp(extension, "ts") == 0)
            sprintf(new_name, "helper.ts");
        else if (strcmp(extension, "py") == 0)
            sprintf(new_name, "calculator.py");
        else if (strcmp(extension, "go") == 0)
            sprintf(new_name, "server.go");
        else
            sprintf(new_name, "renamed.file");
    } else {
        sprintf(new_name, "%s", filename);
    }
    return new_name;
}

void delete_file(const char *filename) {
    if (remove(filename) == 0) {
        syslog(LOG_INFO, "[%s][%s] - %s - Successfully deleted.", getlogin(), _TIME_, filename);
    } else {
        syslog(LOG_ERR, "[%s][%s] - %s - Failed to delete: %s", getlogin(), _TIME_, filename, strerror(errno));
    }
}

void move_to_backup(const char *filename) {
    char new_path[strlen(BACKUP_FOLDER) + strlen(filename) + 1];
    sprintf(new_path, "%s%s", BACKUP_FOLDER, filename);
    if (rename(filename, new_path) == 0) {
        syslog(LOG_INFO, "[%s][%s] - %s - Successfully moved to backup.", getlogin(), _TIME_, filename);
    } else {
        syslog(LOG_ERR, "[%s][%s] - %s - Failed to move to backup: %s", getlogin(), _TIME_, filename, strerror(errno));
    }
}

void restore_from_backup(const char *filename) {
    char backup_path[strlen(BACKUP_FOLDER) + strlen(filename) + 1];
    sprintf(backup_path, "%s%s", BACKUP_FOLDER, filename);
    if (rename(backup_path, filename) == 0) {
        syslog(LOG_INFO, "[%s][%s] - %s - Successfully restored from backup.", getlogin(), _TIME_, filename);
    } else {
        syslog(LOG_ERR, "[%s][%s] - %s - Failed to restore from backup: %s", getlogin(), _TIME_, filename, strerror(errno));
    }
}

void process_files() {
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(".")) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) {
                char *filename = entry->d_name;
                if (strstr(filename, "d3Let3")) {
                    delete_file(filename);
                } else if (strstr(filename, "r3N4mE")) {
                    char *new_name = rename_file(filename);
                    if (strcmp(filename, new_name) != 0) {
                        if (rename(filename, new_name) == 0) {
                            syslog(LOG_INFO, "[%s][%s] - %s - Successfully renamed.", getlogin(), _TIME_, filename);
                        } else {
                            syslog(LOG_ERR, "[%s][%s] - %s - Failed to rename: %s", getlogin(), _TIME_, filename, strerror(errno));
                        }
                    }
                    free(new_name);
                } else if (strstr(filename, "m0V3")) {
                    move_to_backup(filename);
                }
            }
        }
        closedir(dir);
    }
}

void signal_handler(int signum) {
    switch(signum) {
        case SIGRTMIN:
            break; // Mode default, no action needed
        case SIGUSR1:
            system("./management -m backup");
            break;
        case SIGUSR2:
            system("./management -m restore");
            break;
        default:
            break;
    }
}

void setup_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGRTMIN, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
}

void write_log(const char *message) {
    FILE *file = fopen(LOG_FILE, "a");
    if (file) {
        fprintf(file, "%s\n", message);
        fclose(file);
    }
}

void daemonize() {
    pid_t pid, sid;
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    umask(0);
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "-m") == 0) {
        if (strcmp(argv[2], "backup") == 0) {
            move_to_backup("m0V3*");
        } else if (strcmp(argv[2], "restore") == 0) {
            restore_from_backup("m0V3*");
        }
        exit(EXIT_SUCCESS);
    }

    openlog("management", LOG_PID | LOG_CONS, LOG_USER);

    setup_signal_handlers();

    daemonize();

    while (1) {
        process_files();
        write_log("Action completed.");
        sleep(1);
    }

    closelog();
    return 0;
}
