/**
 * @file main.cpp
 * @brief Titik masuk utama dan pengendali alur aplikasi.
 *
 * File ini berisi fungsi `main` yang menjalankan loop aplikasi utama,
 * serta semua fungsi yang menampilkan menu untuk admin dan pelanggan (klien).
 * File ini mengorkestrasi interaksi antara berbagai manajer (Pelanggan, Parkir, Vendor).
 */

#include "CustomerManager.h"
#include "ParkingManager.h"
#include "VendorManager.h" 
#include "Tampilan.hpp"       // <-- Include library Tampilan
#include "AVLTree.hpp"        // <-- Include AVL Tree
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>

#include <algorithm>

using namespace std;

//--- Global Objects ---
PriorityQueuePesanan antrianPesanan;
StackRiwayat riwayatTransaksiAdmin;
AVLTree<Produk> pohonProduk; // <-- AVL Tree global untuk semua produk
// --------------------


// --- Deklarasi Fungsi ---
void menuBeriSaldo(ManajerPelanggan& manajerPelanggan);
void menuToko(Pelanggan* pelanggan, ManajerVendor& manajerVendor, ManajerPelanggan& manajerPelanggan);
string tanganiLogin(ManajerPelanggan& manajerPelanggan, Pelanggan** pelangganKeluar);
void menuKlien(Pelanggan* pelangganMasuk, ManajerParkir& manajerParkir, ManajerVendor& manajerVendor, ManajerPelanggan& manajerPelanggan);
void menuAdmin(ManajerPelanggan& manajerPelanggan, ManajerParkir& manajerParkir, ManajerVendor& manajerVendor); 
void prosesPesanan(ManajerPelanggan& manajerPelanggan, ManajerVendor& manajerVendor);
void undoTransaksiTerakhir(ManajerPelanggan& manajerPelanggan, ManajerVendor& manajerVendor);
void tampilkanDetailPesanan(const Pesanan& p, ManajerPelanggan& manajerPelanggan);
void menuJelajahiProduk(Pelanggan* pelanggan, ManajerPelanggan& manajerPelanggan);


// --- Implementasi Fungsi ---

/**
 * @brief Menampilkan semua produk dari semua vendor, diurutkan berdasarkan harga.
 */
void menuJelajahiProduk(Pelanggan* pelanggan, ManajerPelanggan& manajerPelanggan) {
    Tampilan::printHeader("Jelajahi Semua Produk (Urut Harga)");
    DoublyLinkedList<Produk> semuaProduk = pohonProduk.getInOrderList();

    if (semuaProduk.empty()) {
        Tampilan::printMessage("Saat ini tidak ada produk yang tersedia.");
        Tampilan::pause();
        return;
    }

    int halaman = 0;
    const int itemPerHalaman = 5;

    while (true) {
        Tampilan::clearScreen();
        Tampilan::printHeader("Jelajahi Semua Produk (Halaman " + to_string(halaman + 1) + ")");
        cout << "Saldo Anda: " << Tampilan::GREEN << "Rp" << fixed << setprecision(2) << pelanggan->getSaldo() << Tampilan::RESET << endl << endl;

        int startIndex = halaman * itemPerHalaman;
        int endIndex = std::min(startIndex + itemPerHalaman, static_cast<int>(semuaProduk.size()));

        if (startIndex >= semuaProduk.size() && !semuaProduk.empty()) {
            Tampilan::printMessage("Tidak ada produk lagi untuk ditampilkan.");
            halaman--; 
            Tampilan::pause();
            continue;
        }
        
        cout << Tampilan::CYAN << "--- Menampilkan produk " << startIndex + 1 << " - " << endIndex << " dari " << semuaProduk.size() << " ---\n" << Tampilan::RESET;
        for (int i = startIndex; i < endIndex; ++i) {
            Produk p = semuaProduk[i];
            cout << Tampilan::BOLD << (i + 1) << ". " << p.nama << Tampilan::RESET
                 << " dari " << Tampilan::YELLOW << p.namaVendor << Tampilan::RESET
                 << Tampilan::DIM << " (Stok: " << p.stok << ")" << Tampilan::RESET
                 << Tampilan::GREEN << " - Rp" << p.harga << Tampilan::RESET << endl;
        }

        cout << "\n";
        DoublyLinkedList<string> menuNavigasi;
        if (halaman > 0) menuNavigasi.push_back("Halaman Sebelumnya (P)");
        if (endIndex < semuaProduk.size()) menuNavigasi.push_back("Halaman Berikutnya (N)");
        menuNavigasi.push_back("Kembali (Q)");

        // Tampilkan menu navigasi kustom
        cout << Tampilan::BOLD << "NAVIGASI:" << Tampilan::RESET << endl;
        for(auto it = menuNavigasi.begin(); it != menuNavigasi.end(); ++it){
            cout << " - " << *it << endl;
        }
        
        string pilihan = Tampilan::getString("\nKetik nomor produk untuk membeli, atau pilih navigasi (P/N/Q)");

        // Cek apakah input adalah navigasi
        if (pilihan == "p" || pilihan == "P") {
            if (halaman > 0) halaman--;
            continue;
        } else if (pilihan == "n" || pilihan == "N") {
            if (endIndex < semuaProduk.size()) halaman++;
            continue;
        } else if (pilihan == "q" || pilihan == "Q") {
            break;
        }

        // Jika bukan navigasi, coba proses sebagai nomor produk
        try {
            int noProduk = stoi(pilihan);
            int indexProduk = noProduk - 1;

            if (indexProduk >= startIndex && indexProduk < endIndex) {
                // Produk valid, lanjutkan proses pembelian
                Produk& targetProduk = semuaProduk[indexProduk];
                
                Tampilan::clearScreen();
                Tampilan::printHeader("Antrikan Pesanan");
                cout << "Anda akan membeli: " << Tampilan::BOLD << targetProduk.nama << Tampilan::RESET << " dari " << Tampilan::YELLOW << targetProduk.namaVendor << Tampilan::RESET << endl;
                cout << "Harga Satuan: Rp" << targetProduk.harga << endl;
                cout << "Stok Tersedia: " << targetProduk.stok << endl;
                
                int jumlahBeli = Tampilan::getInt("\nMasukkan jumlah yang ingin dibeli");

                if(jumlahBeli <= 0){
                    Tampilan::printError("Jumlah beli harus lebih dari 0.");
                    Tampilan::pause();
                    continue;
                }

                double totalHarga = targetProduk.harga * jumlahBeli;
                if (pelanggan->getSaldo() < totalHarga) {
                    Tampilan::printError("Transaksi gagal: Saldo tidak mencukupi.");
                    Tampilan::pause();
                    continue;
                }
                if (targetProduk.stok < jumlahBeli) {
                    Tampilan::printError("Transaksi gagal: Stok tidak mencukupi.");
                    Tampilan::pause();
                    continue;
                }

                // Logika untuk enqueue pesanan
                string level = manajerPelanggan.getLevelPelanggan(pelanggan->getId());
                int prioritas = BSTLoyalitasPelanggan::getPrioritasFromLevel(level);

                Pesanan pesananBaru;
                pesananBaru.idPelanggan = pelanggan->getId();
                pesananBaru.idVendor = targetProduk.idVendor;
                pesananBaru.namaProduk = targetProduk.nama;
                pesananBaru.jumlah = jumlahBeli;
                pesananBaru.totalHarga = totalHarga;
                pesananBaru.prioritas = prioritas;
                
                antrianPesanan.enqueue(pesananBaru);
                
                Tampilan::printMessage("Pesanan Anda telah berhasil ditambahkan ke antrian!", Tampilan::GREEN);
                Tampilan::printMessage("Admin akan segera memprosesnya.");
                Tampilan::pause();

            } else {
                Tampilan::printError("Nomor produk tidak valid di halaman ini.");
                Tampilan::pause();
            }
        } catch (const std::invalid_argument& e) {
            Tampilan::printError("Input tidak valid. Silakan coba lagi.");
            Tampilan::pause();
        } catch (const std::out_of_range& e) {
            Tampilan::printError("Input numerik terlalu besar. Silakan coba lagi.");
            Tampilan::pause();
        }
    }
}


/**
 * @brief Menampilkan detail sebuah objek Pesanan secara terformat.
 */
void tampilkanDetailPesanan(const Pesanan& p, ManajerPelanggan& manajerPelanggan) {
    Pelanggan* pelanggan = manajerPelanggan.cariPelanggan(p.idPelanggan);
    string namaPelanggan = pelanggan ? pelanggan->getNama() : "Tidak Dikenal";

    cout << Tampilan::BLUE << "----------------------------------------" << Tampilan::RESET << endl;
    cout << Tampilan::BOLD << "ID Pelanggan   : " << Tampilan::RESET << p.idPelanggan << " (" << namaPelanggan << ")" << endl;
    cout << Tampilan::BOLD << "Produk         : " << Tampilan::RESET << p.namaProduk << endl;
    cout << Tampilan::BOLD << "Jumlah         : " << Tampilan::RESET << p.jumlah << endl;
    cout << Tampilan::BOLD << "Total Harga    : " << Tampilan::GREEN << "Rp" << p.totalHarga << Tampilan::RESET << endl;
    cout << Tampilan::BOLD << "Prioritas      : " << Tampilan::RESET << p.prioritas << endl;
    cout << Tampilan::BLUE << "----------------------------------------" << Tampilan::RESET << endl;
}

/**
 * @brief Memproses pesanan dengan prioritas tertinggi dari antrian.
 */
void prosesPesanan(ManajerPelanggan& manajerPelanggan, ManajerVendor& manajerVendor) {
    Tampilan::printHeader("Proses Pesanan");
    if (antrianPesanan.isEmpty()) {
        Tampilan::printMessage("Tidak ada pesanan untuk diproses.");
        return;
    }

    try {
        Pesanan pesanan = antrianPesanan.dequeue();

        Pelanggan* pelanggan = manajerPelanggan.cariPelanggan(pesanan.idPelanggan);
        Vendor* vendor = manajerVendor.getVendorById(stoi(pesanan.idVendor));

        if (!pelanggan || !vendor) {
            Tampilan::printError("Gagal proses: Pelanggan atau Vendor tidak valid untuk pesanan ini.");
            // Mungkin kembalikan pesanan ke antrian atau log error
            return;
        }

        Produk* produk = nullptr;
        for (auto& p : vendor->getDaftarProduk()) {
            if (p.nama == pesanan.namaProduk) {
                produk = &p;
                break;
            }
        }

        if (!produk) {
            Tampilan::printError("Gagal proses: Produk tidak lagi tersedia.");
            return;
        }

        // Ulangi validasi untuk kondisi race condition
        if (pelanggan->getSaldo() < pesanan.totalHarga || produk->stok < pesanan.jumlah) {
            Tampilan::printError("Gagal proses: Saldo atau stok tidak lagi mencukupi saat akan diproses.");
            // Bisa jadi ada transaksi lain yang mengubah saldo/stok
            return;
        }

        // --- EKSEKUSI TRANSAKSI ---
        pelanggan->setSaldo(pelanggan->getSaldo() - pesanan.totalHarga);
        produk->stok -= pesanan.jumlah;
        
        // Tambah poin loyalitas (misal 1 poin per 1000 rupiah)
        int poinDidapat = static_cast<int>(pesanan.totalHarga / 1000);
        manajerPelanggan.tambahPoin(pelanggan->getId(), poinDidapat);

        // Simpan perubahan ke file
        manajerPelanggan.simpanData();
        manajerVendor.simpan();

        // Masukkan ke riwayat
        riwayatTransaksiAdmin.push(pesanan);
        pelanggan->riwayatBelanja.push(pesanan);

        Tampilan::printMessage("Pesanan berhasil diproses!", Tampilan::GREEN);
        cout << "Detail: " << pesanan.jumlah << "x " << pesanan.namaProduk << " untuk " << pelanggan->getNama() << endl;
        cout << "Poin loyalitas ditambahkan: " << poinDidapat << endl;

    } catch (const runtime_error& e) {
        Tampilan::printError(e.what());
    }
}

/**
 * @brief Membatalkan transaksi terakhir yang diproses oleh admin.
 */
void undoTransaksiTerakhir(ManajerPelanggan& manajerPelanggan, ManajerVendor& manajerVendor) {
    Tampilan::printHeader("Undo Transaksi Terakhir");
    if (riwayatTransaksiAdmin.isEmpty()) {
        Tampilan::printMessage("Tidak ada transaksi untuk di-undo.");
        return;
    }

    try {
        Pesanan pesanan = riwayatTransaksiAdmin.pop();

        Pelanggan* pelanggan = manajerPelanggan.cariPelanggan(pesanan.idPelanggan);
        Vendor* vendor = manajerVendor.getVendorById(stoi(pesanan.idVendor));

        // Jika pelanggan/vendor dihapus setelah transaksi, bisa terjadi error
        if (!pelanggan || !vendor) {
            Tampilan::printError("Gagal undo: Pelanggan atau Vendor dari transaksi ini sudah tidak ada.");
            // Kembalikan pesanan ke stack? Atau log sebagai irreversible?
            // Untuk sekarang, kita hentikan proses undo.
            riwayatTransaksiAdmin.push(pesanan); // Push kembali karena gagal
            return;
        }

        Produk* produk = nullptr;
        for (auto& p : vendor->getDaftarProduk()) {
            if (p.nama == pesanan.namaProduk) {
                produk = &p;
                break;
            }
        }
        
        // Jika produk dihapus dari vendor, ini juga masalah
        if(!produk){
             Tampilan::printError("Gagal undo: Produk dari transaksi ini sudah tidak ada.");
             riwayatTransaksiAdmin.push(pesanan);
             return;
        }

        // --- REVERT TRANSAKSI ---
        pelanggan->setSaldo(pelanggan->getSaldo() + pesanan.totalHarga);
        produk->stok += pesanan.jumlah;

        // Kurangi poin loyalitas yang sudah diberikan
        int poinDibatalkan = static_cast<int>(pesanan.totalHarga / 1000);
        manajerPelanggan.tambahPoin(pelanggan->getId(), -poinDibatalkan);

        // Simpan perubahan
        manajerPelanggan.simpanData();
        manajerVendor.simpan();

        // Optional: Hapus dari riwayat belanja pelanggan?
        // Untuk saat ini, biarkan sebagai bukti transaksi yang dibatalkan.
        
        Tampilan::printMessage("Transaksi terakhir berhasil di-undo!", Tampilan::GREEN);
        cout << "Refund Rp" << pesanan.totalHarga << " ke " << pelanggan->getNama() << "." << endl;
        cout << "Stok " << pesanan.namaProduk << " telah dikembalikan." << endl;

    } catch (const runtime_error& e) {
        Tampilan::printError(e.what());
    }
}


/**
 * @brief Menampilkan menu untuk admin agar dapat menambahkan saldo ke akun pelanggan.
 * @param manajerPelanggan Referensi ke manajer pelanggan untuk mengakses data pelanggan.
 */
void menuBeriSaldo(ManajerPelanggan& manajerPelanggan) {
    Tampilan::printHeader("Beri Saldo");
    
    cout << "\n--- Daftar Pelanggan ---\n";
    manajerPelanggan.tampilkanSemuaPelanggan();

    string idPelanggan = Tampilan::getString("\nMasukkan ID Pelanggan yang akan diberi saldo");

    Pelanggan* pelanggan = manajerPelanggan.cariPelanggan(idPelanggan);
    if (!pelanggan) {
        Tampilan::printError("Pelanggan dengan ID " + idPelanggan + " tidak ditemukan.");
        Tampilan::pause();
        return;
    }

    cout << Tampilan::BOLD << Tampilan::YELLOW << "-> Masukkan jumlah saldo yang akan ditambahkan: " << Tampilan::RESET;
    long double jumlahSaldo;
    cin >> jumlahSaldo;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');


    if (jumlahSaldo <= 0) {
        Tampilan::printError("Jumlah saldo harus positif.");
        Tampilan::pause();
        return;
    }

    pelanggan->setSaldo(pelanggan->getSaldo() + jumlahSaldo);
    manajerPelanggan.simpanData();

    Tampilan::printMessage("Saldo berhasil ditambahkan!", Tampilan::GREEN);
    cout << "Saldo baru untuk " << pelanggan->getNama() << " adalah Rp" << fixed << setprecision(2) << pelanggan->getSaldo() << endl;
    Tampilan::pause();
}

/**
 * @brief Menampilkan menu belanja (toko) untuk pelanggan.
 */
void menuToko(Pelanggan* pelanggan, ManajerVendor& manajerVendor, ManajerPelanggan& manajerPelanggan) {
    Tampilan::printHeader("Toko Vendor");
    cout << "Saldo Anda: " << Tampilan::GREEN << "Rp" << fixed << setprecision(2) << pelanggan->getSaldo() << Tampilan::RESET << endl;

    cout << "\n--- Daftar Vendor Tersedia ---\n";
    manajerVendor.tampilkanSemua();
    
    // Asumsi pengguna memasukkan ID vendor yang valid dari daftar
    int idVendor = Tampilan::getInt("Pilih ID Vendor");

    Vendor* targetVendor = manajerVendor.getVendorById(idVendor);

    if (!targetVendor) {
        Tampilan::printError("Vendor tidak ditemukan.");
        Tampilan::pause();
        return;
    }

    Tampilan::printHeader("Produk Vendor: " + targetVendor->getNama());
    targetVendor->tampilkanProduk();

    string namaProduk = Tampilan::getString("Masukkan nama produk yang ingin dibeli");
    int jumlahBeli = Tampilan::getInt("Masukkan jumlah yang ingin dibeli");

    Produk* targetProduk = nullptr;
    DoublyLinkedList<Produk>& daftarProduk = targetVendor->getDaftarProduk();
    for (auto it = daftarProduk.begin(); it != daftarProduk.end(); ++it) {
        if (it->nama == namaProduk) {
            targetProduk = &(*it);
            break;
        }
    }

    if (!targetProduk) {
        Tampilan::printError("Produk tidak ditemukan.");
        Tampilan::pause();
        return;
    }

    double totalHarga = targetProduk->harga * jumlahBeli;
    if (pelanggan->getSaldo() < totalHarga) {
        Tampilan::printError("Transaksi gagal: Saldo tidak mencukupi.");
        Tampilan::pause();
        return;
    }
    if (targetProduk->stok < jumlahBeli) {
        Tampilan::printError("Transaksi gagal: Stok tidak mencukupi.");
        Tampilan::pause();
        return;
    }

    // --- LOGIKA BARU: MASUKKAN KE PRIORITY QUEUE ---
    string level = manajerPelanggan.getLevelPelanggan(pelanggan->getId());
    int prioritas = BSTLoyalitasPelanggan::getPrioritasFromLevel(level);

    Pesanan pesananBaru;
    pesananBaru.idPelanggan = pelanggan->getId();
    pesananBaru.idVendor = to_string(targetVendor->getId());
    pesananBaru.namaProduk = targetProduk->nama;
    pesananBaru.jumlah = jumlahBeli;
    pesananBaru.totalHarga = totalHarga;
    pesananBaru.prioritas = prioritas;
    
    antrianPesanan.enqueue(pesananBaru);
    // Transaksi tidak dieksekusi di sini. Saldo dan stok tidak diubah.
    
    Tampilan::pause();
}

/**
 * @brief Menangani proses login dan registrasi pengguna.
 */
string tanganiLogin(ManajerPelanggan& manajerPelanggan, Pelanggan** pelangganKeluar) {
    Tampilan::printHeader("Selamat Datang");
    DoublyLinkedList<string> menuItems;
    menuItems.push_back("Login");
    menuItems.push_back("Register");
    menuItems.push_back("Keluar");
    Tampilan::printMenu(menuItems);
    int pilihan = Tampilan::getChoice();

    if (pilihan == 1) {
        Tampilan::printHeader("Login");
        string id = Tampilan::getString("ID");
        string password = Tampilan::getString("Password");
        
        if (id == "admin" && password == "admin123") {
            Tampilan::printMessage("Login admin berhasil!", Tampilan::GREEN);
            Tampilan::pause();
            return "admin";
        }

        Pelanggan* ditemukan = manajerPelanggan.cariPelanggan(id);
        if (ditemukan && ditemukan->verifikasiPassword(password)) {
            *pelangganKeluar = ditemukan; 
            Tampilan::printMessage("Login berhasil! Selamat datang, " + ditemukan->getNama(), Tampilan::GREEN);
            Tampilan::pause();
            return "pelanggan";
        } else {
            Tampilan::printError("Login gagal! ID atau password salah.");
            Tampilan::pause();
            return "gagal";
        }
    } else if (pilihan == 2) {
        manajerPelanggan.tambahPelanggan();
        return "terdaftar";
    } else if (pilihan == 3) {
        return "keluar";
    } else {
        Tampilan::printError("Pilihan tidak valid.");
        Tampilan::pause();
        return "tidak valid";
    }
}

/**
 * @brief Menampilkan menu utama untuk pelanggan yang telah login.
 */
void menuKlien(Pelanggan* pelangganMasuk, ManajerParkir& manajerParkir, ManajerVendor& manajerVendor, ManajerPelanggan& manajerPelanggan) {
    if (!pelangganMasuk) return;

    while (true) {
        Tampilan::clearScreen();
        cout << Tampilan::CYAN << "Profil Anda:" << Tampilan::RESET << endl;
        pelangganMasuk->tampilkanInfo(); 
        
        DoublyLinkedList<string> menuItems;
        menuItems.push_back("Masuk Menu Parkir");
        menuItems.push_back("Jelajahi Semua Produk (Baru!)");
        menuItems.push_back("Jelajahi per-Vendor (Toko)");
        menuItems.push_back("Logout");
        Tampilan::printMenu(menuItems);
        int pilihan = Tampilan::getChoice();

        switch (pilihan) {
            case 1:
                manajerParkir.tampilkanMenu(pelangganMasuk, manajerPelanggan);
                break;
            case 2:
                menuJelajahiProduk(pelangganMasuk, manajerPelanggan);
                break;
            case 3:
                menuToko(pelangganMasuk, manajerVendor, manajerPelanggan);
                break;
            case 4:
                return;
            default:
                Tampilan::printError("Pilihan tidak valid.");
                Tampilan::pause();
        }
    }
}

/**
 * @brief Menampilkan menu utama untuk admin.
 */
void menuAdmin(ManajerPelanggan& manajerPelanggan, ManajerParkir& manajerParkir, ManajerVendor& manajerVendor) {
    while (true) {
        Tampilan::printHeader("Menu Admin");
        DoublyLinkedList<string> menuItems;
        menuItems.push_back("Lihat Semua Pelanggan");
        menuItems.push_back("Manajemen Parkir");
        menuItems.push_back("Manajemen Vendor");
        menuItems.push_back("Beri Saldo ke Pelanggan");
        menuItems.push_back("Lihat Antrian Pesanan (Priority Queue)");
        menuItems.push_back("Proses Pesanan Berikutnya (Priority Queue)");
        menuItems.push_back("Undo Pembelanjaan Terakhir (Stack)");
        menuItems.push_back("Logout");
        Tampilan::printMenu(menuItems);
        int pilihan = Tampilan::getChoice();

        switch (pilihan) {
            case 1:
                Tampilan::printHeader("Daftar Pelanggan");
                manajerPelanggan.tampilkanSemuaPelanggan();
                Tampilan::pause();
                break;
            case 2:
                manajerParkir.tampilkanMenu(nullptr, manajerPelanggan);
                break;
            case 3:
               manajerVendor.jalankan();
               break;
            case 4:
                menuBeriSaldo(manajerPelanggan);
                break;
            case 5:
                Tampilan::printHeader("Antrian Pesanan (Priority Queue)");
                antrianPesanan.tampilkan();
                Tampilan::pause();
                break;
            case 6:
                {
                    if (antrianPesanan.isEmpty()) {
                        Tampilan::printMessage("Tidak ada pesanan untuk diproses.");
                    } else {
                        Tampilan::printMessage("Pesanan berikutnya yang akan diproses:", Tampilan::YELLOW);
                        Pesanan pesanan = antrianPesanan.peek();
                        tampilkanDetailPesanan(pesanan, manajerPelanggan);

                        string konfirmasi = Tampilan::getString("Proses pesanan ini? (Y/N)");
                        if (konfirmasi == "Y" || konfirmasi == "y") {
                            prosesPesanan(manajerPelanggan, manajerVendor);
                        } else {
                            Tampilan::printMessage("Proses dibatalkan.");
                        }
                    }
                    Tampilan::pause();
                }
                break;
            case 7:
                {
                    if (riwayatTransaksiAdmin.isEmpty()) {
                        Tampilan::printMessage("Tidak ada transaksi untuk di-undo.");
                    } else {
                        Tampilan::printMessage("Transaksi terakhir yang akan di-undo:", Tampilan::YELLOW);
                        Pesanan pesanan = riwayatTransaksiAdmin.peekData();
                        tampilkanDetailPesanan(pesanan, manajerPelanggan);

                        string konfirmasi = Tampilan::getString("Batalkan transaksi ini? (Y/N)");
                        if (konfirmasi == "Y" || konfirmasi == "y") {
                            undoTransaksiTerakhir(manajerPelanggan, manajerVendor);
                        } else {
                            Tampilan::printMessage("Pembatalan dibatalkan.");
                        }
                    }
                    Tampilan::pause();
                }
                break;
            case 8:
                return;
            default:
                Tampilan::printError("Pilihan tidak valid.");
                Tampilan::pause();
        }
    }
}

/**
 * @brief Fungsi main, titik awal dari eksekusi program.
 */
int main() {
    ManajerPelanggan manajerPelanggan("pelanggan.db"); 
    ManajerParkir manajerParkir;
    ManajerVendor manajerVendor; 

    // --- Populasi AVL Tree ---
    DoublyLinkedList<Vendor>& semuaVendor = manajerVendor.getDaftarVendor();
    for(auto it = semuaVendor.begin(); it != semuaVendor.end(); ++it) {
        Vendor& vendor = *it;
        DoublyLinkedList<Produk>& produkVendor = vendor.getDaftarProduk();
        for(auto prodIt = produkVendor.begin(); prodIt != produkVendor.end(); ++prodIt) {
            Produk p = *prodIt;
            p.idVendor = to_string(vendor.getId());
            p.namaVendor = vendor.getNama();
            pohonProduk.insert(p.harga, p);
        }
    }
    // -------------------------

    while (true) {
        Pelanggan* pelangganMasuk = nullptr; 
        string hasilLogin = tanganiLogin(manajerPelanggan, &pelangganMasuk);

        if (hasilLogin == "admin") {
            menuAdmin(manajerPelanggan, manajerParkir, manajerVendor);
        } else if (hasilLogin == "pelanggan") {
            menuKlien(pelangganMasuk, manajerParkir, manajerVendor, manajerPelanggan);
        } else if (hasilLogin == "keluar") {
            Tampilan::clearScreen();
            Tampilan::printMessage("Terima kasih telah menggunakan sistem. Sampai jumpa!", Tampilan::MAGENTA);
            Tampilan::pause();
            break;
        }
    }

    return 0;
}
