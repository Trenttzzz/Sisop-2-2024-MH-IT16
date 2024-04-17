# Write up Modul-2 Kelompok IT-16

## Soal 1

## Soal 2

### Langkah - Langkah 
1. Langkah awal yaitu saya membuat berkas baru yang bernama **admin.c** yang dimana berkas ini nantinya akan dijalankan menggunakan perintah `gcc -o admin admin.c`

2. Kemudian untuk membuat fitur yang dimana dapat menampilkan seluruh kegiatan yang dilakukan oleh user maka saya menggunakan fungsi sebagai berikut :
   ```c
    void display_user_activities(char *username) {
    char filename[100];
    char line[100];  // Anda dapat menyesuaikan ukuran buffer sesuai dengan kebutuhan
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
sasasas

## Soal 3

## Soal 4
