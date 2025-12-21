#ifndef TIKET_PARKIR_H
#define TIKET_PARKIR_H

#include <string>
#include <chrono>
#include <iostream>

using namespace std;

// Enum untuk status tiket
enum StatusTiket {
    AKTIF, // Tiket aktif, kendaraan ada di dalam
    DIBAYAR    // Tiket sudah dibayar, kendaraan sudah keluar
};

class TiketParkir {
public:
    string idTiket;
    string idPelanggan;
    string nomorPolisi;
    long long waktuMasuk; // cap waktu epoch
    long long waktuKeluar;
    double biaya;
    StatusTiket status;
    string slotParkir;   // misal, "A1"

    // Konstruktor default
    TiketParkir() 
        : idTiket(""), idPelanggan(""), nomorPolisi(""), 
          waktuMasuk(0), waktuKeluar(0), biaya(0.0), 
          status(AKTIF), slotParkir("") {}

    // Konstruktor dengan parameter
    TiketParkir(string idCust, string plat, string slot)
        : idPelanggan(idCust), nomorPolisi(plat), slotParkir(slot),
          waktuKeluar(0), biaya(0.0), status(AKTIF)
    {
        // Atur waktu masuk generik
        waktuMasuk = chrono::duration_cast<chrono::seconds>(
            chrono::system_clock::now().time_since_epoch()
        ).count();
    }
};

#endif // TIKET_PARKIR_H