# Write up Modul-2 Kelompok IT-16

## Soal 1

## Soal 2

### Langkah - Langkah 
1. Langkah awal yaitu saya membuat berkas baru yang bernama **admin.c** yang dimana berkas ini nantinya akan dijalankan
   menggunakan perintah `gcc -o admin admin.c`

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

## Soal 3

## Soal 4
