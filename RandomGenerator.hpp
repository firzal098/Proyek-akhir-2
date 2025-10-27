#pragma once // Mencegah header di-include berkali-kali

#include <string>  // Untuk std::string
#include <random>  // Untuk std::random_device, std::mt19937, std::uniform_int_distribution
#include <stdexcept> // Untuk std::invalid_argument

/**
 * @brief Namespace untuk utilitas terkait angka acak.
 */
namespace RandomUtils {

    /**
     * @brief Menghasilkan string acak alfanumerik (a-z, A-Z, 0-9).
     *
     * Fungsi ini menggunakan generator Mersenne Twister (mt19937) yang di-seed
     * sekali menggunakan std::random_device untuk performa yang efisien
     * dan kualitas acak yang baik.
     *
     * @param length Panjang string yang ingin dihasilkan. Defaultnya adalah 5.
     * @return std::string yang berisi karakter acak.
     * @throws std::invalid_argument jika panjangnya 0 atau negatif.
     */
    inline std::string generateRandomString(int length = 5) {
        
        if (length <= 0) {
            throw std::invalid_argument("Panjang string harus bilangan bulat positif.");
        }

        // Kumpulan karakter yang akan digunakan
        static const std::string CHARSET =
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789";

        // Bagian ini 'static' agar hanya diinisialisasi sekali saat fungsi
        // pertama kali dipanggil. Ini jauh lebih efisien.
        static std::random_device rd;  // Seed non-deterministik
        static std::mt19937 generator(rd()); // Generator angka (engine)
        
        // Distribusi yang seragam untuk memilih indeks dari CHARSET
        // Kita buat ini di dalam fungsi agar thread-safe jika CHARSET-nya dinamis,
        // tapi karena CHARSET statis, ini bisa juga dibuat statis.
        // Namun, membuat generator-nya statis adalah hal yang paling penting.
        std::uniform_int_distribution<int> distribution(0, CHARSET.length() - 1);

        std::string result;
        result.reserve(length); // Alokasikan memori di awal untuk efisiensi

        for (int i = 0; i < length; ++i) {
            result += CHARSET[distribution(generator)];
        }

        return result;
    }

} // namespace RandomUtils