#ifndef PARKING_SYSTEM_H
#define PARKING_SYSTEM_H

#include <iostream>
#include <fstream>
#include <iomanip> // Untuk put_time
#include <string>
#include "ParkingTicket.h"
#include "json.hpp"
#include "DoublyLinkedList.hpp" // <-- Menggunakan DoublyLinkedList Anda

using namespace std;
using json = nlohmann::json;

class ParkingSystem {
private:
    // MENGGUNAKAN DOUBLY LINKED LIST
    DoublyLinkedList<ParkingTicket> activeTickets; 
    DoublyLinkedList<ParkingTicket> ticketHistory; 
    
    const string historyFile = "parking_history.json";

    // --- Helper JSON ---
    void to_json(json& j, const ParkingTicket& t) {
        j = json{
            {"ticketID", t.ticketID}, {"customerID", t.customerID},
            {"platNomor", t.platNomor}, {"waktuMasuk", t.waktuMasuk},
            {"waktuKeluar", t.waktuKeluar}, {"biaya", t.biaya},
            {"status", (t.status == ACTIVE ? "ACTIVE" : "PAID")},
            {"parkingSlot", t.parkingSlot}
        };
    }

    void from_json(const json& j, ParkingTicket& t) {
        j.at("ticketID").get_to(t.ticketID);
        j.at("customerID").get_to(t.customerID);
        j.at("platNomor").get_to(t.platNomor);
        j.at("waktuMasuk").get_to(t.waktuMasuk);
        j.at("waktuKeluar").get_to(t.waktuKeluar);
        j.at("biaya").get_to(t.biaya);
        t.status = (j.at("status") == "ACTIVE" ? ACTIVE : PAID);
        if (j.find("parkingSlot") != j.end()) {
            j.at("parkingSlot").get_to(t.parkingSlot);
        } else {
            t.parkingSlot = "N/A";
        }
    }

    // Fungsi menyimpan history dengan metode rotasi (karena tidak ada iterator)
    void saveHistory() {
        json j_history = json::array();
        
        size_t historyCount = ticketHistory.size();
        for (size_t i = 0; i < historyCount; ++i) {
            // Ambil data paling depan
            ParkingTicket t = ticketHistory.front();
            ticketHistory.pop_front();

            // Masukkan ke JSON
            json j_ticket;
            to_json(j_ticket, t);
            j_history.push_back(j_ticket);

            // Kembalikan ke belakang list (Rotasi) agar urutan tetap
            ticketHistory.push_back(t);
        }

        ofstream file(historyFile);
        if (file.is_open()) {
            file << j_history.dump(4);
            file.close();
        } else {
            cerr << "Gagal menyimpan riwayat." << endl;
        }
    }

    void loadHistory() {
        ifstream file(historyFile);
        if (file.is_open()) {
            if (file.peek() == ifstream::traits_type::eof()) {
                file.close();
                return;
            }
            json j_history;
            file >> j_history;
            if (j_history.is_array()) {
                for (const auto& j_ticket : j_history) {
                    ParkingTicket ticket;
                    from_json(j_ticket, ticket);
                    ticketHistory.push_back(ticket);
                }
            }
            file.close();
            // cout << "Riwayat parkir berhasil dimuat." << endl;
        }
    }

public:
    ParkingSystem() {
        loadHistory();
    }

    // --- CREATE TICKET ---
    bool createTicket(int customerID, const string& platNomor, const string& slot) {
        // Cek duplikasi dengan teknik rotasi
        bool exists = false;
        size_t count = activeTickets.size(); // Gunakan size() sesuai file Anda

        for (size_t i = 0; i < count; ++i) {
            ParkingTicket t = activeTickets.front();
            activeTickets.pop_front();
            
            if (t.platNomor == platNomor) {
                exists = true;
            }
            
            // Kembalikan ke list
            activeTickets.push_back(t);
        }

        if (exists) {
            cout << "Error: Kendaraan " << platNomor << " sudah terparkir." << endl;
            return false;
        }

        ParkingTicket newTicket(customerID, platNomor, slot); 
        activeTickets.push_back(newTicket);
        cout << "Tiket berhasil dibuat untuk plat " << platNomor << " di slot " << slot << endl;
        cout << "ID Tiket: " << newTicket.ticketID << endl;
        return true; 
    }

    // --- CHECKOUT ---
    ParkingTicket selesaikanCheckout(uint64_t ticketID) {
        ParkingTicket ticketFound;
        bool isFound = false;

        size_t count = activeTickets.size();
        // Loop untuk mencari tiket dan menghapusnya (filter)
        for (size_t i = 0; i < count; ++i) {
            ParkingTicket t = activeTickets.front();
            activeTickets.pop_front();

            if (t.ticketID == ticketID && !isFound) {
                // KETEMU! Proses checkout
                isFound = true;
                t.hitungBiaya();

                cout << "\n--- Struk Parkir ---\n";
                cout << "ID Tiket: " << t.ticketID << endl;
                cout << "Plat Nomor: " << t.platNomor << endl;
                cout << "Slot Parkir: " << t.parkingSlot << endl;
                cout << "Total Biaya: Rp" << t.biaya << endl;
                cout << "Status: LUNAS" << endl;
                cout << "--------------------\n";

                ticketFound = t;
                
                // Simpan ke history
                ticketHistory.push_back(t);
                
                // JANGAN push_back ke activeTickets lagi (artinya dihapus dari aktif)
            } else {
                // Bukan tiket yang dicari, kembalikan ke list
                activeTickets.push_back(t);
            }
        }

        if (isFound) {
            saveHistory();
            return ticketFound;
        } else {
            cout << "Error: Tiket dengan ID " << ticketID << " tidak ditemukan." << endl;
            return ParkingTicket(); // Return kosong
        }
    }

    // --- DISPLAY ACTIVE ---
    // Hapus 'const' karena kita perlu memodifikasi list sementara (pop/push) untuk membacanya
    void displayActiveTickets() {
        cout << "\n--- Kendaraan Saat Ini di Dalam Parkir ---\n";
        if (activeTickets.empty()) {
            cout << "Tidak ada kendaraan di dalam.\n";
            return;
        }
        
        size_t count = activeTickets.size();
        for(size_t i = 0; i < count; ++i) {
            ParkingTicket t = activeTickets.front();
            activeTickets.pop_front();

            cout << "ID Tiket: " << t.ticketID 
                 << ", Plat: " << t.platNomor 
                 << ", Slot: " << t.parkingSlot << endl;

            // Kembalikan ke list
            activeTickets.push_back(t);
        }
        cout << "-------------------------------------------\n";
    }
};

#endif // PARKING_SYSTEM_H