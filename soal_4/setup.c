#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_APPS 50

// Array menyimpan PID dari aplikasi
pid_t app_pids[MAX_APPS];
int app_count = 0;

// Fungsi membuka aplikasi
void openApps(char *app, int num) {
    pid_t pid;

    for (int i = 0; i < num; i++) {
        pid = fork();

        if (pid < 0) {
            // Fork gagal
            perror("Fork gagal");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Proses anak
            execlp(app, app, NULL);
            // exec hanya kembali jika terjadi kesalahan
            perror("Exec gagal");
            exit(EXIT_FAILURE);
        } else {
            // Proses induk
            // Menyimpan PID dari aplikasi yang dibuka
            app_pids[app_count++] = pid;
        }
    }
}

// Fungsi membaca dari file.conf
void openAppsFromFile(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error membuka file.\n");
        return;
    }

    char app[50];
    int num;
    while (fscanf(file, "%s %d", app, &num) != EOF) {
        openApps(app, num);
    }

    fclose(file);
}

// Fungsi menutup semua aplikasi
void closeAllApps() {
    printf("Menutup semua aplikasi...\n");
    // Mengirim sinyal SIGKILL ke semua aplikasi yang dibuka
    for (int i = 0; i < app_count; i++) {
        kill(app_pids[i], SIGKILL);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Penggunaan: %s [-o app1 num1 app2 num2 ...] | [-f namafile] | [-k]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-o") == 0) {
        int count = (argc - 2) / 2;
        for (int i = 0; i < count; i++) {
            char *app = argv[2 + i * 2];
            int num = atoi(argv[3 + i * 2]);
            openApps(app, num);
        }
    } else if (strcmp(argv[1], "-f") == 0) {
        if (argc != 3) {
            printf("Penggunaan: %s -f namafile\n", argv[0]);
            return 1;
        }
        openAppsFromFile(argv[2]);
    } else if (strcmp(argv[1], "-k") == 0) {
        closeAllApps();
    } else {
        printf("Opsi tidak valid.\n");
        return 1;
    }

    return 0;
}
