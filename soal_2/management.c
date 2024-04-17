#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define MAX_USERS 100
#define MAX_COMMAND_LENGTH 20
#define LOG_FILE_EXTENSION ".log"

typedef struct {
    char username[50];
    int monitoring;  // Status pemantauan aktif
    int control;     // Status pengendalian aktif
} User;

void log_activity(char *username, pid_t pid, char *activity, int success) {
    time_t now;
    struct tm *local_time;
    char log_entry[100];
    char filename[100];

    time(&now);
    local_time = localtime(&now);

    strftime(log_entry, sizeof(log_entry), "[%d:%m:%Y]-[%H:%M:%S]", local_time);
    sprintf(log_entry + strlen(log_entry), "-%d-%s_%s\n", pid, activity, success ? "JALAN" : "GAGAL");

    strcpy(filename, username);
    strcat(filename, LOG_FILE_EXTENSION);

    FILE *log_file = fopen(filename, "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    fprintf(log_file, "%s", log_entry);
    fclose(log_file);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Penggunaan: %s -m <user> | -s <user> | -c <user> | -a <user>\n", argv[0]);
        return EXIT_FAILURE;
    }

    User users[MAX_USERS];
    int num_users = 0;

    // Analisis argumen baris perintah
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "-s") == 0 ||
            strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "-a") == 0) {
            strcpy(users[num_users].username, argv[i + 1]);
            users[num_users].monitoring = 0;
            users[num_users].control = 0;
            num_users++;
        } else {
            printf("Perintah tidak valid: %s\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < num_users; i++) {
        if (strcmp(argv[1], "-m") == 0 && strcmp(argv[2], users[i].username) == 0) {
            users[i].monitoring = 1;
            printf("Memantau pengguna: %s\n", users[i].username);
        } else if (strcmp(argv[1], "-s") == 0 && strcmp(argv[2], users[i].username) == 0) {
            users[i].monitoring = 0;
            printf("Menghentikan pemantauan untuk pengguna: %s\n", users[i].username);
        } else if (strcmp(argv[1], "-c") == 0 && strcmp(argv[2], users[i].username) == 0) {
            users[i].control = 1;
            printf("Mengendalikan pengguna: %s\n", users[i].username);
        } else if (strcmp(argv[1], "-a") == 0 && strcmp(argv[2], users[i].username) == 0) {
            users[i].control = 0;
            printf("Menghentikan pengendalian untuk pengguna: %s\n", users[i].username);
        }
    }

    // Loop utama untuk program daemon
    while (1) {
        for (int i = 0; i < num_users; i++) {
            if (users[i].monitoring) {
                // Memantau aktivitas pengguna
                // Contoh: Mencatat aktivitas pengguna di sini
                log_activity(users[i].username, getpid(), "nama_kegiatan", 1);
            }

            if (users[i].control) {
                // Implementasi logika pengendalian di sini
                // Contoh: Menghalangi aktivitas pengguna
                log_activity(users[i].username, getpid(), "nama_kegiatan_terblokir", 0);
            }
        }
        sleep(1); // Memeriksa setiap detik
    }

    return EXIT_SUCCESS;
}
