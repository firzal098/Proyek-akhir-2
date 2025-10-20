#ifndef PARKING_MANAGER_H
#define PARKING_MANAGER_H

#include <iostream>

class ParkingManager {
public:
    void displayParkingMenu() {
        std::cout << "\n===== Manajemen Parkir =====" << std::endl;
        std::cout << "1. Masuk Parkir" << std::endl;
        std::cout << "2. Keluar Parkir" << std::endl;
        std::cout << "3. Tampilkan Status Parkir" << std::endl;
        std::cout << "4. Kembali ke Menu Utama" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "Pilih opsi: ";
    }
};

#endif // PARKING_MANAGER_H

