#ifndef MANAJER_VENDOR_H
#define MANAJER_VENDOR_H

#include "vendor.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <limits>
#include <sstream>
#include <fstream> // Required for direct file I/O
#include "DoublyLinkedList.hpp"
#include "PenyimpanFile.hpp"
#include "Tampilan.hpp"
#include "Huffman.hpp" // Include Huffman compressor

using namespace std;

class ManajerVendor {
private:
    DoublyLinkedList<Vendor> daftarVendor;
    string uncompressedFileName = "vendors.db";
    string compressedFileName = "vendors_compressed.db";
    HuffmanCompressor huffman; // Huffman compressor member
    const std::string COMPRESSION_HEADER = "[HUFFMAN_COMPRESSED]\n";
    const std::string CODES_DELIMITER = "---CODES_END---\n";

    void tambahVendor() {
        Tampilan::printHeader("Tambah Vendor");
        
        cout << Tampilan::BOLD << Tampilan::YELLOW << "-> ID Vendor (Angka): " << Tampilan::RESET;
        int id;
        cin >> id;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        for (auto& v : daftarVendor) {
            if (v.getId() == id) {
                Tampilan::printError("ID Vendor sudah digunakan!");
                Tampilan::pause();
                return;
            }
        }

        string nama = Tampilan::getString("Nama Vendor");
        string produk = Tampilan::getString("Produk Utama");
        string kat = Tampilan::getString("Kategori");

        daftarVendor.push_back(Vendor(id, nama, produk, kat));
        simpan();
        Tampilan::printMessage("Vendor berhasil disimpan!", Tampilan::GREEN);
        Tampilan::pause();
    }

    void cariVendor() {
        Tampilan::printHeader("Cari Vendor");
        string key = Tampilan::getString("Masukkan kata kunci (Nama/Kategori)");
        string lowerKey = keHurufKecil(key);
        
        cout << "\n" << Tampilan::CYAN << "--- Hasil Pencarian ---\n" << Tampilan::RESET;
        
        bool found = false;
        for (auto& v : daftarVendor) {
            if (keHurufKecil(v.getNama()).find(lowerKey) != string::npos || 
                keHurufKecil(v.getKategori()).find(lowerKey) != string::npos) {
                v.info();
                found = true;
            }
        }
        if (!found) {
            Tampilan::printMessage("Tidak ditemukan vendor dengan kata kunci tersebut.");
        }
    }

    void menuKelolaProduk() {
        Tampilan::printHeader("Kelola Produk");
        tampilkanSemua();
        cout << Tampilan::BOLD << Tampilan::YELLOW << "-> Masukkan ID Vendor yang ingin dikelola: " << Tampilan::RESET;
        int id;
        cin >> id;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        Vendor* target = nullptr;
        for (auto it = daftarVendor.begin(); it != daftarVendor.end(); ++it) {
            if (it->getId() == id) {
                target = &(*it);
                break;
            }
        }

        if (!target) {
            Tampilan::printError("Vendor tidak ditemukan.");
            Tampilan::pause();
            return;
        }

        int subPilih;
        do {
            Tampilan::printHeader("Kelola Produk: " + target->getNama());
            DoublyLinkedList<string> menuItems;
            menuItems.push_back("Tambah Produk");
            menuItems.push_back("Lihat Daftar Produk");
            menuItems.push_back("Kembali");
            Tampilan::printMenu(menuItems);
            subPilih = Tampilan::getChoice();

            if (subPilih == 1) {
                target->tambahProduk();
                simpan(); 
                Tampilan::printMessage("Produk berhasil ditambahkan.", Tampilan::GREEN);
                Tampilan::pause();
            } else if (subPilih == 2) {
                Tampilan::printHeader("Daftar Produk: " + target->getNama());
                target->tampilkanProduk();
                Tampilan::pause();
            }
        } while (subPilih != 0);
    }

public:
    ManajerVendor() {
        // Try to load from compressed file first
        ifstream compFile(compressedFileName);
        if (compFile.is_open()) {
            loadFromCompressedFile(compFile);
            compFile.close();
            return;
        }

        // If compressed file doesn't exist, try loading from uncompressed file
        ifstream uncompFile(uncompressedFileName);
        if (uncompFile.is_open()) {
            loadFromUncompressedFile(uncompFile);
            uncompFile.close();
            // After loading from uncompressed, save to compressed file
            simpan();
            return;
        }

        // If neither file exists, proceed with empty daftarVendor
    }

    void loadFromCompressedFile(ifstream& file) {
        string line;
        getline(file, line); // Read first line (should be header)

        if (line == COMPRESSION_HEADER.substr(0, COMPRESSION_HEADER.length() - 1)) {
            // File is compressed
            string serializedCodes = "";
            string codeLine;
            while (getline(file, codeLine) && codeLine != CODES_DELIMITER.substr(0, CODES_DELIMITER.length() - 1)) {
                serializedCodes += codeLine;
            }
            huffman.deserializeCodes(serializedCodes);

            string compressedDataLine;
            // Read the rest of the file as compressed lines
            while (getline(file, compressedDataLine)) {
                if (!compressedDataLine.empty()) {
                    try {
                        string decompressedVendorData = huffman.decompress(compressedDataLine);
                        daftarVendor.push_back(Vendor::deserialisasi(decompressedVendorData));
                    } catch (const runtime_error& e) {
                        cerr << "[ManajerVendor] Error during decompression/deserialization: " << e.what() << endl;
                    }
                }
            }
        }
    }

    void loadFromUncompressedFile(ifstream& file) {
        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                try {
                    daftarVendor.push_back(Vendor::deserialisasi(line));
                } catch (const runtime_error& e) {
                    cerr << "[ManajerVendor] Warning: Skipped malformed line in uncompressed file: " << e.what() << endl;
                }
            }
        }
    }

    void simpan() {
        ofstream file(compressedFileName);
        if (!file.is_open()) {
            cerr << "[ManajerVendor] Error: Could not open " << compressedFileName << " for writing." << endl;
            return;
        }

        if (daftarVendor.empty()) {
            // If no vendors, just write the header and an empty code block
            file << COMPRESSION_HEADER;
            file << huffman.serializeCodes(); // This will be empty
            file << CODES_DELIMITER;
            file.close();
            return;
        }

        // Collect all uncompressed vendor data first to build one Huffman tree
        string allUncompressedData = "";
        for (auto it = daftarVendor.begin(); it != daftarVendor.end(); ++it) {
            allUncompressedData += Vendor::serialisasi(*it);
            allUncompressedData += '\n'; // Add newline as separator
        }
        
        // Remove trailing newline if any
        if (!allUncompressedData.empty() && allUncompressedData.back() == '\n') {
            allUncompressedData.pop_back();
        }

        // Build the Huffman tree once from all data
        huffman.compress(allUncompressedData); // This builds the tree and codes.

        // Write to compressed file with header and codes
        file << COMPRESSION_HEADER;
        file << huffman.serializeCodes();
        file << CODES_DELIMITER;

        // Now, use the generated codes to compress each line individually.
        for (auto it = daftarVendor.begin(); it != daftarVendor.end(); ++it) {
            string uncompressedVendorLine = Vendor::serialisasi(*it);
            string compressedVendorLine = "";
            for(char ch : uncompressedVendorLine) {
                compressedVendorLine += huffman.getCode(ch); // Use the already generated codes
            }
            file << compressedVendorLine << '\n';
        }

        file.close();
    }

    DoublyLinkedList<Vendor>& getDaftarVendor() {
        return daftarVendor;
    }

    Vendor* getVendorById(int id) {
        for (auto it = daftarVendor.begin(); it != daftarVendor.end(); ++it) {
            if (it->getId() == id) {
                return &(*it);
            }
        }
        return nullptr;
    }

    void tampilkanSemua() {
        cout << "\n" << Tampilan::CYAN << "--- Daftar Vendor ---\n" << Tampilan::RESET;
        if (daftarVendor.empty()) {
            Tampilan::printMessage("Data vendor kosong.");
            return;
        }
        for (auto& v : daftarVendor) {
            v.info();
        }
    }

    void jalankan() {
        int pilih;
        do {
            Tampilan::printHeader("Manajemen Vendor");
            DoublyLinkedList<string> menuItems;
            menuItems.push_back("Tambah Vendor Baru");
            menuItems.push_back("Tampilkan Semua Vendor");
            menuItems.push_back("Cari Vendor (Nama/Kategori)");
            menuItems.push_back("Kelola Produk Vendor");
            menuItems.push_back("Kembali ke Menu Utama");
            Tampilan::printMenu(menuItems);
            pilih = Tampilan::getChoice();

            switch (pilih) {
                case 1: tambahVendor(); break;
                case 2: 
                    Tampilan::printHeader("Semua Vendor");
                    tampilkanSemua(); 
                    Tampilan::pause(); 
                    break;
                case 3: 
                    cariVendor(); 
                    Tampilan::pause(); 
                    break;
                case 4: menuKelolaProduk(); break;
                case 0: break;
                default: 
                    Tampilan::printError("Pilihan tidak valid.");
                    Tampilan::pause();
            }
        } while (pilih != 0);
    }
};

#endif // MANAJER_VENDOR_H
