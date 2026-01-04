#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include <string>
#include <iostream>
#include "DoublyLinkedList.hpp" // Assuming this is available
#include "PenyimpanFile.hpp" // For the pisah function

// Custom Pair struct since std::map/std::pair are not allowed directly for storing codes
template <typename K, typename V>
struct CustomPair {
    K key;
    V value;

    CustomPair(K k, V v) : key(k), value(v) {}
};

// Huffman Node structure
struct HuffmanNode {
    char data; // Character for leaf nodes
    int freq; // Frequency of the character
    HuffmanNode *left, *right; // Left and right child

    // Constructor for leaf nodes
    HuffmanNode(char data, int freq) {
        left = right = nullptr;
        this->data = data;
        this->freq = freq;
    }

    // Constructor for internal nodes
    HuffmanNode(HuffmanNode* left, HuffmanNode* right) {
        this->left = left;
        this->right = right;
        this->freq = left->freq + right->freq; // Sum frequencies of children
        this->data = '\0'; // Internal nodes don't have a character
    }

    // For deleting the tree
    ~HuffmanNode() {
        // Recursive deletion handled by parent's destructor.
        // Prevent double deletion if a node is shared, though in Huffman build, they are not.
        // It's generally better to have a dedicated tree traversal for deletion.
        // For simplicity here, assuming a single ownership model where 'delete root' suffices.
        delete left;
        left = nullptr; // Set to nullptr after deletion
        delete right;
        right = nullptr; // Set to nullptr after deletion
    }

    // Comparison for sorting in DoublyLinkedList (mimicking priority queue)
    bool operator>(const HuffmanNode& other) const {
        return freq > other.freq;
    }
    bool operator<(const HuffmanNode& other) const {
        return freq < other.freq;
    }
};

// Custom DoublyLinkedList-based "Min-Priority Queue" for Huffman Nodes
// Cannot directly sort DoublyLinkedList in-place, so insertion needs to be sorted.
class HuffmanMinPQ {
private:
    DoublyLinkedList<HuffmanNode*> list;

public:
    void push(HuffmanNode* node) {
        if (list.empty() || (**list.begin() > *node)) { 
            list.push_front(node);
            return;
        }
        
        auto it = list.begin();
        while (it != list.end() && (*(*it) < *node)) {
            ++it;
        }
        // Insert 'node' before 'it'
        // DoublyLinkedList::insert before iterator functionality might be missing
        // If not available, need to iterate and re-create nodes or modify DoublyLinkedList
        // Assuming insert(iterator, value) is implemented or simulating it.
        // Given that it's a "custom" list, let's assume it has an insert functionality.
        // If not, a linear scan and re-creation of the list would be required.
        // For this example, I'll assume an insert method exists or I'll add one.
        
        // As DoublyLinkedList in this project does not seem to have `insert(iterator, value)`
        // directly, I will simulate it by finding the position and then manually
        // manipulating pointers or re-creating a sublist.
        // This is a common pain point for implementing custom data structures.
        // For now, I'll use a simpler approach for a sorted insertion to avoid modifying DoublyLinkedList.hpp

        // Simulating sorted insertion without modifying DoublyLinkedList
        DoublyLinkedList<HuffmanNode*> newList;
        bool inserted = false;
        for (auto current_node_it = list.begin(); current_node_it != list.end(); ++current_node_it) {
            if (!inserted && (**current_node_it > *node)) { // Corrected dereferencing
                newList.push_back(node);
                inserted = true;
            }
            newList.push_back(*current_node_it);
        }
        if (!inserted) {
            newList.push_back(node);
        }
        list = newList;
    }

    HuffmanNode* pop() {
        if (list.empty()) return nullptr;
        HuffmanNode* node = list.front();
        list.pop_front();
        return node;
    }

    HuffmanNode* top() {
        if (list.empty()) return nullptr;
        return list.front();
    }

    bool empty() const {
        return list.empty();
    }

    size_t size() const {
        return list.size();
    }
    
    // Custom destructor to prevent memory leaks from nodes not popped
    ~HuffmanMinPQ() {
        // Nodes taken by pop() are owned by the HuffmanCompressor tree.
        // Remaining nodes (if any, should not happen in a correctly built tree) should be deleted.
        // For simplicity, assuming all nodes are properly consumed into the Huffman tree.
        // If a node is pushed and not used, it would be a leak.
        // Best practice would be to ensure nodes pushed here are deleted if not part of the final tree.
    }
};


class HuffmanCompressor {
private:
    HuffmanNode* root;
    DoublyLinkedList<CustomPair<char, std::string>> huffmanCodes; // To store char -> code mapping
    DoublyLinkedList<CustomPair<char, int>> charFrequencies; // To store char -> frequency mapping

    void buildCharFrequencies(const std::string& text) {
        charFrequencies.clear();

        int counts[256] = {0}; 
        for (char ch : text) {
            counts[static_cast<unsigned char>(ch)]++;
        }

        for (int i = 0; i < 256; ++i) {
            if (counts[i] > 0) {
                charFrequencies.push_back(CustomPair<char, int>(static_cast<char>(i), counts[i]));
            }
        }
    }


    void buildHuffmanTree() {
        // Clear previous tree if any
        if (root) {
            delete root; 
            root = nullptr;
        }

        HuffmanMinPQ pq;

        // If only one unique character, handle it as a special case
        if (charFrequencies.size() == 1) {
            auto it = charFrequencies.begin();
            root = new HuffmanNode(it->key, it->value);
            return;
        }
        
        for (auto it = charFrequencies.begin(); it != charFrequencies.end(); ++it) {
            pq.push(new HuffmanNode(it->key, it->value));
        }

        while (pq.size() > 1) {
            HuffmanNode* left = pq.pop();
            HuffmanNode* right = pq.pop();
            HuffmanNode* parent = new HuffmanNode(left, right);
            pq.push(parent);
        }

        root = pq.pop();
    }

    void generateCodes(HuffmanNode* node, std::string currentCode) {
        if (!node) return;

        // If it's a leaf node, store the Huffman code
        if (!node->left && !node->right) {
            huffmanCodes.push_back(CustomPair<char, std::string>(node->data, currentCode));
        }

        generateCodes(node->left, currentCode + "0");
        generateCodes(node->right, currentCode + "1");
    }

public: // Made public
    std::string getCode(char ch) {
        for (auto it = huffmanCodes.begin(); it != huffmanCodes.end(); ++it) {
            if (it->key == ch) {
                return it->value;
            }
        }
        return ""; // Should not happen for valid characters
    }

public:
    HuffmanCompressor() : root(nullptr) {}

    // Destructor to clean up Huffman tree
    ~HuffmanCompressor() {
        delete root;
    }

    // Compresses the input text
    std::string compress(const std::string& text) {
        if (text.empty()) return "";

        buildCharFrequencies(text);
        if (charFrequencies.empty()) return ""; // No characters to compress

        buildHuffmanTree();
        
        huffmanCodes.clear(); 
        if (root->left == nullptr && root->right == nullptr) { // Handle single character case
            huffmanCodes.push_back(CustomPair<char, std::string>(root->data, "0"));
        } else {
            generateCodes(root, "");
        }
        
        std::string encodedText = "";
        for (char ch : text) {
            encodedText += getCode(ch);
        }

        return encodedText;
    }

    // Decompresses the input text using the current Huffman tree
    std::string decompress(const std::string& encodedText) {
        if (encodedText.empty() || !root) return "";

        std::string decodedText = "";
        HuffmanNode* current = root;

        for (char bit : encodedText) {
            if (bit == '0') {
                current = current->left;
            } else if (bit == '1') {
                current = current->right;
            } else {
                throw std::runtime_error("Malformed Huffman encoded data (invalid bit)");
            }

            if (!current) {
                throw std::runtime_error("Malformed Huffman encoded data (path leads to null)");
            }

            // If a leaf node is reached, append character to decoded string
            if (!current->left && !current->right) {
                decodedText += current->data;
                current = root; // Reset to root for next character
            }
        }
        // Check if last character was fully decoded
        if (current != root && (current->left || current->right)) {
             throw std::runtime_error("Malformed Huffman encoded data (incomplete last character)");
        }
        
        return decodedText;
    }

    // Serialize Huffman codes for storage
    // Format: char1:code1;char2:code2;...
    std::string serializeCodes() {
        std::string serialized = "";
        bool first = true;
        for (auto it = huffmanCodes.begin(); it != huffmanCodes.end(); ++it) {
            if (!first) serialized += ";";
            // Use numeric character code for the key to avoid delimiter collisions
            serialized += std::to_string(static_cast<unsigned char>(it->key)) + ":" + it->value;
            first = false;
        }
        return serialized;
    }

    // Deserialize Huffman codes from storage and rebuild decoding tree
    void deserializeCodes(const std::string& serializedCodes) {
        // Clear previous tree and codes
        if (root) { delete root; root = nullptr; }
        huffmanCodes.clear();
        charFrequencies.clear(); // Not strictly needed for decoding tree but good practice

        if (serializedCodes.empty()) return;

        // Remove any stray carriage returns that may come from CRLF files
        std::string cleaned = serializedCodes;
        cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '\r'), cleaned.end());

        // Reconstruct huffmanCodes list. Format now is: <ascii_number>:<code>;<ascii_number>:<code>;...
        DoublyLinkedList<std::string> pairs = PenyimpanFile::pisah(cleaned, ';'); 
        for (auto it_pair = pairs.begin(); it_pair != pairs.end(); ++it_pair) {
            std::string pairStr = *it_pair;
            if (pairStr.empty()) continue;

            size_t colonPos = pairStr.find(':');
            if (colonPos == std::string::npos) continue;

            std::string keyToken = pairStr.substr(0, colonPos);
            std::string code = pairStr.substr(colonPos + 1);

            // Trim whitespace from keyToken and code
            keyToken.erase(std::remove_if(keyToken.begin(), keyToken.end(), ::isspace), keyToken.end());
            code.erase(std::remove_if(code.begin(), code.end(), ::isspace), code.end());

            if (keyToken.empty() || code.empty()) continue;

            // Parse numeric key
            int keyVal = 0;
            try {
                keyVal = std::stoi(keyToken);
            } catch (...) {
                continue; // skip malformed entries
            }

            char ch = static_cast<char>(keyVal);
            huffmanCodes.push_back(CustomPair<char, std::string>(ch, code));
        }

        // Build the decoding tree from the deserialized codes
        // Create an empty root. For each code (char:code_string), traverse the tree,
        // adding new nodes for '0' and '1' as needed, and setting the leaf node's char.
        if (huffmanCodes.empty()) return; // No codes to build tree from

        root = new HuffmanNode('\0', 0); // Root for decoding tree

        for (auto it = huffmanCodes.begin(); it != huffmanCodes.end(); ++it) {
            HuffmanNode* current = root;
            for (char bit : it->value) {
                if (bit == '0') {
                    if (!current->left) current->left = new HuffmanNode('\0', 0);
                    current = current->left;
                } else if (bit == '1') {
                    if (!current->right) current->right = new HuffmanNode('\0', 0);
                    current = current->right;
                } else {
                    // This case indicates malformed code string (e.g., neither '0' nor '1')
                    throw std::runtime_error("Malformed Huffman code string during tree reconstruction");
                }
            }
            current->data = it->key; // Set character at leaf
        }
    }
};

#endif // HUFFMAN_HPP