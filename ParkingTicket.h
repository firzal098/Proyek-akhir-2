#ifndef PARKING_TICKET_H
#define PARKING_TICKET_H

#include <string>
#include <chrono>
#include <cstdint>

// Tambahkan using namespace std
using namespace std;

// Enum untuk status pembayaran tiket
enum TicketStatus {
    ACTIVE, // Tiket aktif, kendaraan masih di dalam
    PAID    // Tiket sudah dibayar
};

class ParkingTicket {
public:
    uint64_t ticketID;
    int customerID;
    string platNomor; // Menggunakan string secara langsung
    long long waktuMasuk; // Simpan sebagai timestamp (epoch time)
    long long waktuKeluar;
    double biaya;
    TicketStatus status;

    // Default constructor
    ParkingTicket() : ticketID(0), customerID(-1), waktuMasuk(0), waktuKeluar(0), biaya(0.0), status(ACTIVE) {}

    // Constructor untuk membuat tiket baru saat kendaraan masuk
    ParkingTicket(int custID, const string& plat)
        : customerID(custID), platNomor(plat), waktuKeluar(0), biaya(0.0), status(ACTIVE) {
        
        // Generate a unique ticket ID based on current timestamp
        ticketID = chrono::duration_cast<chrono::nanoseconds>(
            chrono::system_clock::now().time_since_epoch()
        ).count();

        // Catat waktu masuk
        waktuMasuk = chrono::duration_cast<chrono::seconds>(
            chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    // Fungsi untuk menghitung biaya saat kendaraan keluar
    void hitungBiaya() {
        if (status == ACTIVE) {
            // Catat waktu keluar
            waktuKeluar = chrono::duration_cast<chrono::seconds>(
                chrono::system_clock::now().time_since_epoch()
            ).count();

            // Hitung durasi dalam jam (dibulatkan ke atas)
            long long durasiDetik = waktuKeluar - waktuMasuk;
            long long durasiJam = (durasiDetik + 3599) / 3600; // Pembulatan ke atas

            // Tarif sederhana: Rp 3000 per jam
            const double TARIF_PER_JAM = 3000.0;
            biaya = durasiJam * TARIF_PER_JAM;
            status = PAID;
        }
    }
};

#endif // PARKING_TICKET_H

