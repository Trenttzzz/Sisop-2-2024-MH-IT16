#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <curl/curl.h>
#include <zip.h>

#define HISTORY_LOG "soal_2/history.log"

enum Mode {
    DEFAULT,
    BACKUP,
    RESTORE
};

enum Mode current_mode = DEFAULT;

void log_action(const char *username, const char *filename, const char *action) {
    time_t now;
    struct tm *local_time;
    char time_str[9]; // HH:MM:SS
    FILE *log_file;

    time(&now);
    local_time = localtime(&now);
    strftime(time_str, sizeof(time_str), "%H:%M:%S", local_time);

    log_file = fopen(HISTORY_LOG, "a");
    if (log_file != NULL) {
        fprintf(log_file, "[%s][%s] - %s - %s\n", username, time_str, filename, action);
        fclose(log_file);
    }
}

void handle_signal(int sig) {
    if (sig == SIGUSR1) {
        current_mode = BACKUP;
    } else if (sig == SIGUSR2) {
        current_mode = RESTORE;
    }
}

void download_and_extract(const char *url, const char *output_dir) {
    // Download file
    CURL *curl = curl_easy_init();
    FILE *fp = fopen("temp.zip", "wb");
    if (curl && fp) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    } else {
        printf("Failed to initialize curl or open file.\n");
        return;
    }

    // Extract file
    struct zip *archive = zip_open("temp.zip", 0, NULL);
    if (!archive) {
        printf("Failed to open zip file.\n");
        return;
    }

    int num_entries = zip_get_num_entries(archive, 0);
    if (num_entries < 0) {
        printf("Failed to get number of entries in zip file.\n");
        zip_close(archive);
        return;
    }

    for (int i = 0; i < num_entries; ++i) {
        struct zip_stat file_info;
        if (zip_stat_index(archive, i, 0, &file_info) != 0) {
            printf("Failed to get file info for entry %d.\n", i);
            continue;
        }

        char *filename = malloc(strlen(file_info.name) + 1);
        if (!filename) {
            printf("Memory allocation failed.\n");
            break;
        }

        zip_file_t *file = zip_fopen_index(archive, i, 0);
        if (!file) {
            printf("Failed to open file %d.\n", i);
            free(filename);
            continue;
        }

        if (zip_fread(file, filename, file_info.size) < 0) {
            printf("Failed to read file %d.\n", i);
            zip_fclose(file);
            free(filename);
            continue;
        }

        filename[file_info.size] = '\0';

        // Dekripsi nama file ke-7 hingga terakhir menggunakan algoritma ROT19
        if (i >= 6) {
            for (int j = 0; j < strlen(filename); ++j) {
                if ((filename[j] >= 'A' && filename[j] <= 'Z') || (filename[j] >= 'a' && filename[j] <= 'z')) {
                    if ((filename[j] >= 'A' && filename[j] <= 'Z')) {
                        filename[j] = ((filename[j] - 'A' + 7) % 26) + 'A';
                    } else {
                        filename[j] = ((filename[j] - 'a' + 7) % 26) + 'a';
                    }
                }
            }
        }

        // Hapus file yang mengandung kode "d3Let3"
        if (strstr(filename, "d3Let3") != NULL) {
            remove(filename);
            printf("File %s dihapus.\n", filename);
            continue;
        }

        // Proses file yang mengandung kode "r3N4mE"
        if (strstr(filename, "r3N4mE") != NULL) {
            char *extension = strrchr(filename, '.');
            if (extension != NULL) {
                // Jika ekstensi file adalah ".ts"
                if (strcmp(extension, ".ts") == 0) {
                    rename(filename, "helper.ts");
                    printf("File %s direname menjadi helper.ts.\n", filename);
                }
                // Jika ekstensi file adalah ".py"
                else if (strcmp(extension, ".py") == 0) {
                    rename(filename, "calculator.py");
                    printf("File %s direname menjadi calculator.py.\n", filename);
                }
                // Jika ekstensi file adalah ".go"
                else if (strcmp(extension, ".go") == 0) {
                    rename(filename, "server.go");
                    printf("File %s direname menjadi server.go.\n", filename);
                }
                // Jika ekstensi file tidak sesuai
                else {
                    rename(filename, "renamed.file");
                    printf("File %s direname menjadi renamed.file.\n", filename);
                }
            }
        }
    }

    zip_close(archive);
}

void run_default_mode() {
    printf("Running default mode.\n");
}

void run_backup_mode() {
    printf("Running backup mode.\n");
    // Download and extract files
    download_and_extract("https://drive.google.com/uc?id=1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup", "backup_directory");
}

void run_restore_mode() {
    printf("Running restore mode.\n");
    // Download and extract files
    download_and_extract("https://drive.google.com/uc?id=1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup", "restore_directory");
}

int main(int argc, char *argv[]) {
    // Set up signal handlers
    signal(SIGUSR1, handle_signal);
    signal(SIGUSR2, handle_signal);

    // Check if there are additional arguments
    if (argc > 1) {
        if (strcmp(argv[1], "-m") == 0 && argc > 2) {
            // Change mode based on argument
            if (strcmp(argv[2], "backup") == 0) {
                kill(getpid(), SIGUSR1);
            } else if (strcmp(argv[2], "restore") == 0) {
                kill(getpid(), SIGUSR2);
            }
        }
    }

    // Run the appropriate mode
    if (current_mode == BACKUP) {
        run_backup_mode();
    } else if (current_mode == RESTORE) {
        run_restore_mode();
    } else {
        run_default_mode();
    }

    return 0;
}
