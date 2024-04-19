# Write up Modul-2 Kelompok IT-16

## Soal 1

### Langkah - Langkah
1. awalnya saya input library yang dibutuhkan:
    ```c
    #define _DEFAULT_SOURCE // untuk dt_reg
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h> // untuk fork dan setsid
    #include <dirent.h>  
    #include <sys/stat.h>
    #include <sys/types.h> // untuk pid_t
    #include <time.h> // mencatat waktu
    ```
    diatas saya menggunakan sekitar 8 library yang di butuhkan sesuai yang di jelaskan pada comment diatas. lalu saya menggunakan `#define` untuk define max lenght nya
    ```c
    #define MAX_PATH_LENGTH 4096 // panjang maksimal path 
    ```

2. lalu untuk fungsi yang saya gunakan adalah:
    ```c
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
    ```
    fungsi diatas untuk menulis ulang file yang terdapat kata **suspicios** dengan cara membuka file dalam direktori tersebut, lalu cek size dari file nya, kemudian alokasi memori dan baca isi file tersebut dan masukkan kedalam memory, lalu ganti string sesuai dengan ketentuan yang diminta pada soal seperti berikut:
        
    1. String **m4LwAr3** direplace dengan string **[MALWARE]**
    2. String **5pYw4R3** direplace dengan string **[SPYWARE]**
    3. String **R4nS0mWaR3** direplace dengan string **[RANSOMWARE]**

    lalu saya membuat variable untuk record log nya:
    ```c
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
    ```
    jadi fungsi diatas akan membuat file bernama `virus.log` lalu mencatat waktu saat variable tersebut dijalankan, kemudian print text nya pada `virus.log` saat menemukan **suspicious word** di dalam file yang di scan.

3. Terakhir saya membuat main function nya yang akan menjalankan file tersebut secara **daemon**

    ```c
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
    ```
    ketika saya menjalankan kode diatas akan menjadi seperti ini:
    (gambar terminal)

    lalu berikut adalah isi dari log file yang terbentuk:
    ```log
    [19-04-2024][12:48:14] Suspicious string at test1.txt successfully replaced!
    [19-04-2024][12:48:14] Suspicious string at test2.txt successfully replaced!
    ```
    berikut adalah isi file sebelum diubah text **suspicious** nya

    ![file_1_before](https://github.com/Trenttzzz/Sisop-2-2024-MH-IT16/assets/141043792/c362c67b-13dc-4049-95ae-be60a680c93a)

   ![file_2_before](https://github.com/Trenttzzz/Sisop-2-2024-MH-IT16/assets/141043792/2ad69509-4721-4767-ad1b-f6e6391a9c40)


    lalu berikut adalah isi file sesudah file dijalankan

    ![file_1_after](https://github.com/Trenttzzz/Sisop-2-2024-MH-IT16/assets/141043792/301bc440-205c-471f-b799-b0b019f858e5)

   ![file_2_after](https://github.com/Trenttzzz/Sisop-2-2024-MH-IT16/assets/141043792/c903280b-5c4a-46d2-866b-de45ccab2f7c)



    untuk membuktikan program tersebut berjalan pada background kita bisa cek pid nya dengan `pgrep -fl <filename>`

    ![pgrep_fl](https://github.com/Trenttzzz/Sisop-2-2024-MH-IT16/assets/141043792/d4160189-169e-46e7-84ed-2e6dd728b241)


    lalu cek dengan `ps aux | grep <pid>`
    
    ![ps_aux](https://github.com/Trenttzzz/Sisop-2-2024-MH-IT16/assets/141043792/44d5d0bb-9a8e-45ff-b19a-ec140f361b65)



## Soal 2

## Soal 3

### Langkah - Langkah 
1. Langkah awal yaitu saya membuat berkas baru yang bernama **admin.c** yang dimana berkas ini nantinya akan dijalankan
   menggunakan perintah `gcc -o admin admin.c`

   Berikut adalah contoh bentuk file yang telah dibuat :

   ![image](https://github.com/Trenttzzz/Sisop-2-2024-MH-IT16/assets/152785029/52435cbf-08a2-4d1f-8887-f9408a4c72b2)


2. Kemudian untuk membuat fitur yang dimana dapat menampilkan seluruh kegiatan yang dilakukan oleh user maka saya
   menggunakan fungsi sebagai berikut :
   ```c
    void display_user_activities(char *username) {
    char filename[100];
    char line[100];  
    FILE *log_file;

    // Membuat nama file log dari nama pengguna
    strcpy(filename, username);
    strcat(filename, LOG_FILE_EXTENSION);

    // Membuka file log untuk dibaca
    log_file = fopen(filename, "r");
    if (log_file == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    // Membaca setiap baris log dari file dan menampilkannya
    while (fgets(line, sizeof(line), log_file) != NULL) {
        // Menampilkan baris log ke layar atau keluaran yang sesuai
        printf("%s", line);
    }

    // Menutup file log setelah selesai membacanya
    fclose(log_file);
   ```

3. Pada langkah ini saya akan membuat program berjalan secara daemon dan berjalan terus menerus yang dimana adalah
   menggunakan
   `./admin -m farand` untuk menjalankan fitur
   `./admin -s farand` untuk mematikan fitur
   **Fitur tersebut terdapat pada fungsi utama dan juga loop utama untuk daemon**

   Kemudian semua kegiatan user ini akan tercatat dengan format
   ```[dd:mm:yyyy]-[hh:mm:ss]-pid_kegiatan-nama_kegiatan_GAGAL/JALAN```

   Maka saya menggunakan fungsi sebagai berikut:
   ```c
   void log_activity(char *username, pid_t pid, char *activity, int success) {
    time_t now; 
    struct tm *local_time;
    char log_entry[100];
    char filename[100];

    time(&now); // Untuk mendapatkan waktu saat ini
    local_time = localtime(&now);

    // Untuk membuat format waktu [dd:mm:yyyy]-[hh:mm:ss]
    strftime(log_entry, sizeof(log_entry), "[%d:%m:%Y]-[%H:%M:%S]", local_time);
    // Menambahkan informasi PID dan kegiatan ke dalam entri log
    sprintf(log_entry + strlen(log_entry), "-%d-%s_%s\n", pid, activity, success ? "JALAN" : "GAGAL");

    strcpy(filename, username);
    strcat(filename, LOG_FILE_EXTENSION);

    FILE *log_file = fopen(filename, "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    // Menulis entri log ke dalam file
    fprintf(log_file, "%s", log_entry);
    fclose(log_file);
   }
   ```

   Berikut adalah contoh output ketika saya menjalankan `admin -m user`. Dan maka aktivitas login akan tersimpan pada file
   baru bernama *`user.log`* . Kemudian monitoring tersebut akan berhenti ketika saya menjankan `admin -s user` pada tab
   terminal yang baru. Jika saya tidak menjalankan `admin -s user` maka otomatis program akan me-monitoring kegiatan user
   setiap detik hingga saya mematikan fitur tersebut. Dan aktivitas tersebut akan menampilkan seperti template pada soal
   yaitu  `[dd:mm:yyyy]-[hh:mm:ss]-pid_kegiatan-nama_kegiatan_GAGAL/JALAN` 

   ![image](https://github.com/Trenttzzz/Sisop-2-2024-MH-IT16/assets/152785029/8e977bc7-d7e9-4b35-a544-08c17497670f)

4. Program akan bisa menggagalkan kegiatan user setiap detik dan fitur ini juga bisa dimatikan
   `./admin -c farand` untuk menjalankan fitur
   `./admin -a farand` untuk mematikan fitur

   Berikut adalah yang terjadi ketika saya menjalankan `./admin -c farand`

   ![image](https://github.com/Trenttzzz/Sisop-2-2024-MH-IT16/assets/152785029/8b275f0a-af5d-439e-b4c5-51fcc00bc7ec)

   Maka dengan fitur tersebut kegiatan user juga akan digagalkan setiap detiknya kemudian agar kegiatan user dapat berjalan
   tanpa gagal maka kita akan menjalankan perintah `./admin -a farand` maka dengan begitu kegiatan user akan berjalan
   normal lagi seperti berikut :

   ![image](https://github.com/Trenttzzz/Sisop-2-2024-MH-IT16/assets/152785029/408d1c85-6d8f-41f0-9c4a-b2979ad8855e)

   Dengan begitu kegiatan user akan berjalan seperti awla dan berjalan setiap detiknya.

5. Kemudian ketika kegiatan user digagalkan maka program akan melog dan menset log tersebut sebagai **GAGAL**. Dan jika di
   log menggunakan fitur poin ke 3, log akan ditulis dengan **JALAN**. Berikut adalah contoh perbedaan ketika kegiatan user
   GAGAL maupun JALAN :

   ![image](https://github.com/Trenttzzz/Sisop-2-2024-MH-IT16/assets/152785029/198b805a-705e-4c32-bf39-2ced178286ca)


## Soal 4
#### Langkah - Langkah

1. Saya menggunakan 4 fungsi utama yang dimana untuk menjalankan aplikasi, kemudian membaca file.conf yang berisikan
   aplikasi yang akan dijalankan kemudian fungsi untuk menutup aplikasi yang telah dibuka kemudian fungsi utama.

   **Membuka Aplikasi**
   ```c
   void openApps(char *app, int num) {
    pid_t pid;

    for (int i = 0; i < num; i++) {
        pid = fork();

        if (pid < 0) {
            perror("Fork gagal");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            execlp(app, app, NULL);
            perror("Exec gagal");
            exit(EXIT_FAILURE);
        } else {
            // Proses utama
            app_pids[app_count++] = pid;
        }
    }
   }
   ```
   **Membaca file.conf**
   ```c
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
   ```
   **Menutup aplikasi**
   ```c
   void closeAllApps() {
    printf("Menutup semua aplikasi...\n");
    // Mengirim sinyal SIGKILL ke semua aplikasi yang dibuka
    for (int i = 0; i < app_count; i++) {
        kill(app_pids[i], SIGKILL);
    }
   }
   ```

2. Dengan begitu kita bisa mulai menjalankan perintah yang pertama yaitu `./setup -o firefox 2 wireshark 2` yang dimana
   perintah tersebut untuk meberi perintah membuka 2 tab firefox dan 2 tab wireshark. Berikut adalah contoh ketika 2 tab
   firefox dan 2 tab wireshark terbuka: 

   ![image](https://github.com/Trenttzzz/Sisop-2-2024-MH-IT16/assets/152785029/cf11b6f7-a640-417d-ba1f-174c473c33de)

   ![image](https://github.com/Trenttzzz/Sisop-2-2024-MH-IT16/assets/152785029/fcbbe7d5-b676-4863-b17c-9b8936c77ee7)

3. Kemudian langkah selanjutnya adalah membuat *`file.conf`* agar kita bisa menjalankan perintah untuk membuka aplikasi
   melalui konfigurasi. *`file.conf`* itu sendiri berisikan _firefox 2 wireshark 3_ yang berarti ketika *`file.conf`*
   dijalankan maka akan membuka 2 tab firefox dan 3 tab wireshark.

   ![image](https://github.com/Trenttzzz/Sisop-2-2024-MH-IT16/assets/152785029/b27a2e77-c317-4c06-869b-c813b3cb1ecb)

4. Maka dengan fungsi `void closeAllApps()` yang ada ada program saya, menggunakan perintah  `setup-k` akan mematikan semua
   aplikasi yang dijalankan dengan program tersebut. Kemudian begitu juga dengan perintah `setup -k file.conf` maka program
   juga dapat mematikan aplikasi yang dijalankan sesuai dengan file konfigurasinya





   
   
   
