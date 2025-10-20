#ifndef VENDOR_MANAGER_H
#define VENDOR_MANAGER_H

#include <iostream>

class VendorManager {
public:
    void displayVendorMenu() {
        std::cout << "\n===== Manajemen Vendor =====" << std::endl;
        std::cout << "1. Tambah Vendor" << std::endl;
        std::cout << "2. Tampilkan Semua Vendor" << std::endl;
        std::cout << "3. Cari Vendor" << std::endl;
        std::cout << "4. Kembali ke Menu Utama" << std::endl;
        std::cout << "==========================" << std::endl;
        std::cout << "Pilih opsi: ";
    }
};

#endif // VENDOR_MANAGER_H

