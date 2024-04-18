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

// Untuk menyimpan informasi pengguna
typedef struct {
    char username[50]; 
    int monitoring;     
    int control;        
} User;

// Fungsi untuk mencatat aktivitas ke file log
void log_activity(char *username, pid_t pid, char *activity, int success) {
    time_t now;             // Waktu saat ini
    struct tm *local_time;  
    char log_entry[100];    
    char filename[100];     

    // Mendapatkan waktu saat ini
    time(&now);
    local_time = localtime(&now);

    // Format waktu dalam entri log
    strftime(log_entry, sizeof(log_entry), "[%d:%m:%Y]-[%H:%M:%S]", local_time);
    // Menambahkan informasi PID dan kegiatan ke dalam entri log
    sprintf(log_entry + strlen(log_entry), "-%d-%s_%s\n", pid, activity, success ? "JALAN" : "GAGAL");

    // Membuat nama file log dari nama pengguna
    strcpy(filename, username);
    strcat(filename, LOG_FILE_EXTENSION);

    // Membuka file log untuk ditambahkan entri
    FILE *log_file = fopen(filename, "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    // Menulis entri log ke dalam file
    fprintf(log_file, "%s", log_entry);
    fclose(log_file);
}

int main(int argc, char *argv[]) {
    // Memastikan jumlah argumen yang diberikan sesuai
    if (argc < 3) {
        printf("Usage: %s -m <user> | -s <user> | -c <user> | -a <user>\n", argv[0]);
        return EXIT_FAILURE;
    }

    User users[MAX_USERS];  // Array untuk menyimpan informasi pengguna
    int num_users = 0;      

    // Memproses argumen baris perintah
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "-s") == 0 ||
            strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "-a") == 0) {
            // Menyimpan nama pengguna yang terdaftar
            strcpy(users[num_users].username, argv[i + 1]);
            users[num_users].monitoring = 0;
            users[num_users].control = 0;
            num_users++;  // Menambah jumlah pengguna yang terdaftar
        } else {
            printf("Invalid command: %s\n", argv[i]); 
            return EXIT_FAILURE;
        }
    }

    // Memproses opsi perintah dan nama pengguna yang diberikan
    for (int i = 0; i < num_users; i++) {
        if (strcmp(argv[1], "-m") == 0 && strcmp(argv[2], users[i].username) == 0) {
            users[i].monitoring = 1;  // Mengaktifkan pemantauan 
            printf("Monitoring user: %s\n", users[i].username);  // Pesan konfirmasi
        } else if (strcmp(argv[1], "-s") == 0 && strcmp(argv[2], users[i].username) == 0) {
            users[i].monitoring = 0;  // Menonaktifkan pemantauan
            printf("Stopping monitoring for user: %s\n", users[i].username);  // Pesan konfirmasi
        } else if (strcmp(argv[1], "-c") == 0 && strcmp(argv[2], users[i].username) == 0) {
            users[i].control = 1;  // Mengaktifkan pengendalian
            printf("Controlling user: %s\n", users[i].username);  // Pesan konfirmasi
        } else if (strcmp(argv[1], "-a") == 0 && strcmp(argv[2], users[i].username) == 0) {
            users[i].control = 0;  // Menonaktifkan pengendalian
            printf("Stopping control for user: %s\n", users[i].username);  // Pesan konfirmasi
        }
    }

    // Loop utama program daemon
    while (1) {
        for (int i = 0; i < num_users; i++) {
            if (users[i].monitoring) {
                // Memantau aktivitas pengguna
                log_activity(users[i].username, getpid(), "nama_kegiatan", 1);
            }

            if (users[i].control) {
                // Melaksanakan logika pengendalian di sini
                log_activity(users[i].username, getpid(), "nama_kegiatan_terblokir", 0);
            }
        }
        sleep(1);  // Memeriksa setiap detik
    }

    return EXIT_SUCCESS;
}
