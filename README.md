# Write up Modul-2 Kelompok IT-16

## Soal 1

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
