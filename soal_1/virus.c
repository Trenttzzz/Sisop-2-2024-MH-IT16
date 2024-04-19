#define _DEFAULT_SOURCE // untuk dt_reg
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // untuk fork dan setsid
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h> // buat pid_t
#include <time.h> // mencatat waktu

#define MAX_PATH_LENGTH 4096 //ukuran maksimal path 

void rewriteFile(char *filePath, char *target, char *replacement) {
    FILE *file = fopen(filePath, "r+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // ambil size dari file yang di scan
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Alokasi memory untuk menyimpan semua konten awal
    char *fileContent = (char *)malloc(fileSize + 1);
    if (fileContent == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return;
    }

    // baca file & masukkan ke memory
    size_t bytesRead = fread(fileContent, 1, fileSize, file);
    if (bytesRead != fileSize) {
        perror("Error reading file");
        free(fileContent);
        fclose(file);
        return;
    }

    // Null-terminate the content to use it as a string
    fileContent[fileSize] = '\0';

    // ganti string yang diminta
    char *pos = fileContent;
    while ((pos = strstr(pos, target)) != NULL) {
        memmove(pos + strlen(replacement), pos + strlen(target), strlen(pos + strlen(target)) + 1);
        memcpy(pos, replacement, strlen(replacement));
        pos += strlen(replacement);
    }

    // Move the file pointer to the beginning of the file
    rewind(file);

    // tulis kembali modified file ke file lama
    if (fwrite(fileContent, 1, fileSize, file) != fileSize) {
        perror("Error writing to file");
    }

    
    free(fileContent);
    fclose(file);
}


void recordlog(char *fileName) {
    FILE *logFile = fopen("virus.log", "a");
    if (logFile == NULL) {
        logFile = fopen("virus.log", "w");
    }

    time_t now = time(NULL);
    struct tm *localTime = localtime(&now);
    char timeString[23];
    strftime(timeString, sizeof(timeString), "[%d-%m-%Y][%H:%M:%S]", localTime); // Menggunakan %S untuk detik

    fprintf(logFile, "%s Suspicious string at %s successfully replaced!\n", timeString, fileName);
    fclose(logFile);
}

int main(int argc, char *argv[]) {
    // daemonize
    pid_t pid = fork();

    if (pid < 0) {
        perror("Error forking");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0) {
        perror("Error creating new session");
        exit(EXIT_FAILURE);
    }
    
    if (argc != 2) {
        printf("Usage: %s <folder_path>\n", argv[0]);
        return 1;
    }

    // declare folderpath dari argv
    char *folderPath = argv[1];
    struct dirent *entry;

    // loop agar program berjalan terus menerus
    while (1) {
        DIR *dir = opendir(folderPath);

        if (dir == NULL) {
            perror("Error opening directory");
            return 1;
        }

        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) {
                char filePath[MAX_PATH_LENGTH];
                snprintf(filePath, sizeof(filePath), "%s/%s", folderPath, entry->d_name);

                rewriteFile(filePath, "m4LwAr3", "[MALWARE]");
                rewriteFile(filePath, "5pYw4R3", "[SPYWARE]");
                rewriteFile(filePath, "R4nS0mWaR3", "[RANSOMWARE]");

                recordlog(entry->d_name);
            }
        }

        closedir(dir);
        sleep(15); // jeda 15 detik
    }

    return 0;
}
