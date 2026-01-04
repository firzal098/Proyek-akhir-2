# Proyek Sistem Manajemen Parkir dan Penjualan

## Deskripsi Proyek
Proyek ini adalah sistem berbasis konsol yang dirancang untuk mengelola parkir, data pelanggan, vendor, KTP, dan transaksi penjualan produk. Sistem ini dibangun menggunakan C++ dan memanfaatkan berbagai struktur data untuk efisiensi dan pengelolaan data yang terstruktur. Fitur utama mencakup registrasi dan manajemen pelanggan, pengelolaan KTP, sistem parkir, manajemen vendor dan produk, serta antrian pesanan dengan prioritas.

## Fitur Utama
*   **Manajemen Pelanggan**: Registrasi, login, lihat profil, dan pengelolaan saldo.
*   **Manajemen KTP**: Pengisian dan pengelolaan data KTP yang terintegrasi dengan pelanggan.
*   **Manajemen Parkir**: Sistem untuk masuk dan keluar parkir, perhitungan biaya, dan riwayat parkir.
*   **Manajemen Vendor**: Penambahan, penghapusan, dan pembaruan data vendor serta produk mereka.
*   **Sistem Belanja**: Pelanggan dapat menjelajahi dan membeli produk dari berbagai vendor.
*   **Antrian Pesanan Prioritas**: Pesanan pelanggan diproses berdasarkan tingkat loyalitas mereka.
*   **Riwayat Transaksi**: Admin dapat melihat riwayat transaksi dan melakukan `undo` transaksi terakhir.
*   **Program Populasi Data**: Sebuah script terpisah untuk mengisi data awal secara otomatis.

## Struktur Data yang Digunakan

Berikut adalah daftar struktur data kunci yang diimplementasikan dan bagaimana penggunaannya dalam proyek ini secara konkret dan detail:

### 1. Doubly Linked List (DoublyLinkedList.hpp)
*   **Penggunaan**:
    *   **Penyimpanan Koleksi Umum**: Digunakan secara luas untuk menyimpan koleksi objek dinamis yang urutannya penting atau membutuhkan penyisipan/penghapusan cepat di mana saja.
    *   **`ManajerPelanggan::daftarPelanggan`**: Menyimpan semua objek `Pelanggan` yang terdaftar. Memungkinkan iterasi mudah untuk pencarian, pembaruan, dan penampilan semua pelanggan.
    *   **`Vendor::daftarProduk`**: Setiap objek `Vendor` memiliki `DoublyLinkedList<Produk>` untuk menyimpan daftar produk yang dijual oleh vendor tersebut. Ini memfasilitasi manajemen inventaris produk per vendor.
    *   **`AVLTree::valueList`**: Dalam implementasi `AVLTree`, jika ada beberapa node yang memiliki `key` yang sama (misalnya, beberapa produk dengan harga yang sama), semua objek `value` tersebut disimpan dalam `DoublyLinkedList<T>` di dalam `AVLNode` yang sama. Ini mengatasi tabrakan `key` dengan menyimpan semua nilai terkait.
    *   **`AVLTree::getInOrderList()`**: Fungsi ini mengembalikan semua elemen dalam `AVLTree` sebagai `DoublyLinkedList<T>` yang terurut secara inorder, yang sangat berguna untuk menampilkan atau memproses semua elemen secara berurutan.
    *   **`main.cpp` (Menu Navigasi)**: Digunakan untuk membuat daftar item menu yang ditampilkan di konsol, seperti `menuItems` di `menuKlien` atau `menuAdmin`, memungkinkan penambahan item menu secara dinamis.
    *   **`PenyimpanFile::pisah()`**: Fungsi utilitas ini mengembalikan `DoublyLinkedList<string>` hasil pemisahan string berdasarkan delimiter, sangat penting untuk deserialisasi data dari file.
    *   **`Customer::deserialisasi()`**: Saat memuat data pelanggan dari file, string data dipecah menjadi token-token menggunakan `DoublyLinkedList<string>` sebelum dikonversi kembali menjadi objek `Pelanggan`.

### 2. AVL Tree (AVLTree.hpp)
*   **Penggunaan**:
    *   **`KTPManager::pohonKTP`**: Sebuah `AVLTree<KTP>` digunakan untuk menyimpan semua data KTP. `key` untuk AVL tree ini dihasilkan dari NIK (Nomor Induk Kependudukan). Struktur ini memastikan:
        *   **Pencarian Cepat**: KTP dapat dicari berdasarkan NIK dengan kompleksitas waktu O(log N), krusial untuk memverifikasi atau mengambil data KTP dengan cepat.
        *   **Keseimbangan Otomatis**: Tree tetap seimbang sendiri, mencegah degenerasi menjadi list linier dan menjaga performa pencarian yang optimal bahkan dengan banyak data.
    *   **`main.cpp::pohonProduk`**: Sebuah `AVLTree<Produk>` global digunakan untuk menyimpan semua produk dari semua vendor, dengan harga produk (`double`) sebagai `key`. Ini memungkinkan:
        *   **Penjelajahan Produk Berdasarkan Harga**: Memungkinkan pengguna untuk melihat produk dalam urutan harga atau mencari produk dalam rentang harga tertentu secara efisien.
        *   **Skalabilitas**: Mengelola daftar produk yang besar tanpa penurunan performa yang signifikan.

### 3. Priority Queue (DataStructures.hpp, digunakan sebagai `PriorityQueuePesanan`)
*   **Penggunaan**:
    *   **`main.cpp::antrianPesanan`**: Implementasi priority queue digunakan untuk mengelola pesanan pelanggan. Setiap pesanan memiliki tingkat prioritas, yang ditentukan berdasarkan level loyalitas pelanggan (misalnya, pelanggan level "Gold" memiliki prioritas lebih tinggi).
        *   **Pemrosesan Terurut**: Pesanan dengan prioritas tertinggi akan selalu diproses terlebih dahulu, memastikan bahwa pelanggan VIP menerima layanan yang lebih cepat.
        *   **Alokasi Sumber Daya**: Membantu admin dalam mengelola dan memproses pesanan secara efektif berdasarkan urgensi atau nilai pelanggan.

### 4. Stack (DataStructures.hpp, digunakan sebagai `StackRiwayat`)
*   **Penggunaan**:
    *   **`main.cpp::riwayatTransaksiAdmin`**: Admin menggunakan stack untuk menyimpan riwayat transaksi pembelian yang telah berhasil diproses.
        *   **Fungsi Undo**: Memungkinkan admin untuk membatalkan (undo) transaksi terakhir yang dilakukan, dengan mudah mengembalikan perubahan saldo pelanggan dan stok produk. Ini mengikuti prinsip LIFO (Last-In, First-Out).
    *   **`Customer::riwayatBelanja`**: Setiap objek `Pelanggan` memiliki stack pribadi untuk menyimpan riwayat belanja mereka sendiri.
        *   **Pelacakan Sejarah Pembelian**: Memungkinkan pelanggan untuk melihat daftar pembelian terakhir mereka, juga mengikuti prinsip LIFO.

### 5. Binary Search Tree (BSTLoyalitasPelanggan, diimplementasikan dalam CustomerManager.h)
*   **Penggunaan**:
    *   **`ManajerPelanggan::bstLoyalitas`**: Digunakan untuk menyimpan dan mengelola poin loyalitas setiap pelanggan. ID pelanggan berfungsi sebagai `key`.
        *   **Pencarian Cepat Poin Loyalitas**: Memungkinkan sistem untuk dengan cepat mencari poin loyalitas pelanggan dan menentukan level loyalitas mereka (misalnya, "Bronze", "Silver", "Gold").
        *   **Pembaruan Efisien**: Penambahan atau pengurangan poin loyalitas dapat dilakukan dengan efisien, menjaga struktur terurut untuk akses cepat.

## Persyaratan Sistem
*   Sistem Operasi: Windows, macOS, atau Linux
*   Compiler C++: Mendukung standar C++17 (misalnya, GCC, Clang, MSVC)
*   RAM: Minimal 512 MB
*   Penyimpanan: Minimal 100 MB ruang kosong

## Cara Kompilasi dan Menjalankan Program

Untuk mengkompilasi dan menjalankan program utama:

1.  **Buka Terminal/Command Prompt** di direktori root proyek.
2.  **Kompilasi**: Gunakan perintah berikut:
    ```bash
    g++ -std=c++17 -O0 -I . main.cpp -o exe/main.exe
    ```
    *   `-std=c++17`: Menggunakan standar C++17.
    *   `-O0`: Tidak ada optimasi (untuk debugging lebih mudah).
    *   `-I .`: Menambahkan direktori saat ini sebagai direktori include untuk header.
    *   `main.cpp`: File sumber utama.
    *   `-o exe/main.exe`: Menghasilkan executable bernama `main.exe` di folder `exe`.

3.  **Jalankan**: Setelah kompilasi berhasil, jalankan executable:
    ```bash
    ./exe/main.exe
    ```

## Panduan Penggunaan

*   **Login/Registrasi**: Pada awal program, Anda akan diminta untuk login atau mendaftar sebagai pelanggan baru. Admin dapat login dengan ID `admin` dan password `admin123`.
*   **Menu Klien**: Pelanggan dapat mengelola parkir, membeli produk dari vendor, melihat data KTP, dan melihat riwayat belanja.
*   **Menu Admin**: Admin memiliki akses untuk mengelola pelanggan, vendor, KTP, memproses pesanan, melihat antrian pesanan, dan melakukan `undo` transaksi.

## Populasi Data Awal

Untuk mengisi database dengan data awal (pelanggan dan KTP) secara otomatis:

1.  **Buka Terminal/Command Prompt** di direktori root proyek.
2.  **Kompilasi script populasi data**:
    ```bash
    g++ -std=c++17 -O0 -I . populate_data.cpp -o exe/populate_data.exe
    ```
3.  **Jalankan script populasi data**:
    ```bash
    ./exe/populate_data.exe
    ```
    Script ini akan membuat atau memperbarui file `pelanggan.db` dan `ktp_data.db` dengan 20 entri pelanggan dan KTP yang saling terkait. Ini sangat berguna untuk pengujian dan demonstrasi.
