Simple Web Interface

Fitur yang didukung:
- Register / Login (admin: `admin` / `admin123`)
- Admin: lihat pelanggan, beri saldo, tambah vendor, tambah produk
- Pelanggan: lihat profil & saldo, beli produk, check-in/check-out parkir, lihat denah sederhana

## Struktur Data pada Modul Vendor

### Doubly Linked List
Struktur data **Doubly Linked List** digunakan secara ekstensif untuk mengelola koleksi data yang dinamis.
1.  **Daftar Produk (`daftarProduk`)**:
    -   Setiap objek `Vendor` memiliki atribut `daftarProduk` yang bertipe `DoublyLinkedList<Produk>`.
    -   **Penjelasan**: Struktur ini dipilih karena memungkinkan vendor untuk memiliki jumlah produk yang dinamis (bertambah/berkurang) tanpa batasan ukuran tetap seperti array. Linked list juga efisien untuk operasi penyisipan data baru.
2.  **Pemrosesan String (Tokenizing)**:
    -   Dalam fungsi `deserialisasi` (baik di kelas `Produk` maupun `Vendor`), `DoublyLinkedList<string>` digunakan sebagai wadah sementara untuk menyimpan token string hasil pemisahan data (parsing) dari format file teks sebelum dikonversi menjadi objek.

### AVL Tree
Struktur data **AVL Tree** digunakan untuk optimasi pencarian dan pengurutan.
1.  **Pencarian dan Sorting Produk (`pohonProduk` di `main.cpp`)**:
    -   **Penjelasan**: AVL Tree digunakan secara global untuk mengindeks semua produk dari berbagai vendor dengan harga sebagai *key*. Ini memungkinkan fitur "Jelajahi Semua Produk" untuk menampilkan produk secara terurut berdasarkan harga (ascending) dan memfasilitasi pencarian produk yang efisien.

### Huffman Coding
Struktur data **Huffman Coding** digunakan untuk efisiensi penyimpanan data (Kompresi).
1.  **Kompresi Database Vendor**:
    -   **Penjelasan**: Algoritma Huffman digunakan untuk memampatkan (compress) data string vendor sebelum disimpan ke dalam file penyimpanan. Dengan memetakan karakter yang sering muncul ke kode biner yang lebih pendek, ukuran file database dapat diminimalkan, yang mempercepat operasi I/O (Input/Output) file.

## Struktur Data pada Modul Customer & Transaksi

### Doubly Linked List
Digunakan sebagai wadah penyimpanan data dinamis yang fleksibel.
1.  **Daftar Pelanggan**: Menyimpan seluruh objek pelanggan yang terdaftar.
2.  **Menu Navigasi**: Menyimpan item-item menu string pada antarmuka pengguna (`menuItems`), memungkinkan navigasi menu yang dinamis.

### Stack (Tumpukan)
Menggunakan prinsip LIFO (*Last In, First Out*) untuk manajemen riwayat.
1.  **Riwayat Transaksi Admin (`riwayatTransaksiAdmin`)**:
    -   **Fungsi**: Menyimpan jejak transaksi yang baru saja diproses.
    -   **Fitur**: Memungkinkan fitur **Undo (Batalkan)**. Admin dapat membatalkan transaksi terakhir (misal: salah input), mengembalikan saldo pelanggan, dan stok produk secara otomatis.

### Priority Queue (Antrian Berprioritas)
Menggunakan prinsip antrian di mana elemen dengan prioritas tertinggi diproses lebih dulu.
1.  **Antrian Pesanan (`antrianPesanan`)**:
    -   **Fungsi**: Menampung pesanan yang masuk dari pelanggan sebelum diproses oleh admin.
    -   **Logika**: Pesanan tidak diproses berdasarkan urutan kedatangan (FIFO murni), melainkan berdasarkan **Level Loyalitas Pelanggan** (Gold, Silver, Bronze). Pelanggan dengan status loyalitas lebih tinggi (Gold) akan ditempatkan di depan antrian untuk diproses lebih cepat.

### AVL Tree (Adelson-Velsky and Landis Tree)
Pohon pencarian biner yang menyeimbangkan diri sendiri (self-balancing) untuk performa pencarian O(log n).
1.  **Manajemen Data KTP (`KTPManager`)**:
    -   **Fungsi**: Mengindeks data KTP pelanggan.
    -   **Penjelasan**: Digunakan untuk menyimpan data KTP sehingga pencarian berdasarkan NIK atau Nama dapat dilakukan dengan sangat cepat, bahkan jika jumlah data penduduk/pelanggan sangat besar. Ini mencegah kelambatan yang terjadi jika menggunakan pencarian linear biasa.

## Struktur Data pada Modul Parkir

### Stack (Tumpukan)
Digunakan untuk manajemen riwayat parkir.
1.  **Riwayat Kendaraan Keluar**:
    -   **Fungsi**: Menyimpan data tiket dari kendaraan yang baru saja melakukan *checkout*.
    -   **Penjelasan**: Memudahkan admin untuk memantau arus keluar kendaraan terakhir (LIFO - *Last In First Out*).

### Queue (Antrian)
Digunakan untuk manajemen arus masuk.
1.  **Antrian Kendaraan Masuk**:
    -   **Fungsi**: Menampung kendaraan yang akan masuk ke area parkir.
    -   **Penjelasan**: Menggunakan prinsip FIFO (*First In, First Out*) agar kendaraan dilayani sesuai urutan kedatangan.

### Binary Search Tree (BST)
Digunakan untuk pencarian cepat data parkir aktif.
1.  **Indeks Nomor Polisi**:
    -   **Fungsi**: Menyimpan referensi tiket parkir yang masih aktif (kendaraan di dalam) diurutkan berdasarkan Nomor Polisi.
    -   **Penjelasan**: Memungkinkan fitur "Cari Kendaraan" atau "Cek Status Parkir" bekerja dengan efisien (O(log n)) tanpa harus memindai seluruh slot parkir satu per satu.
