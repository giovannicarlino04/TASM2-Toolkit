#include <iostream>
#include <fstream>
#include <vector>
#include <zlib.h> // Per decompressione zlib
#include <cstring> // Per memcpy
#include <cstdint>

// Funzione per leggere un file in un vector
std::vector<unsigned char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Impossibile aprire il file: " + filename);
    }
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

// Funzione per scrivere un vector su un file
void writeFile(const std::string& filename, const std::vector<unsigned char>& data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Impossibile aprire il file per scrittura: " + filename);
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

// Wrapper per decompressione zlib
std::vector<unsigned char> decompressBlock(const unsigned char* compressedData, size_t compressedSize, size_t expectedDecompressedSize) {
    std::vector<unsigned char> decompressedData(expectedDecompressedSize);
    z_stream strm = {};
    strm.next_in = const_cast<unsigned char*>(compressedData);
    strm.avail_in = compressedSize;
    strm.next_out = decompressedData.data();
    strm.avail_out = expectedDecompressedSize;

    if (inflateInit(&strm) != Z_OK) {
        throw std::runtime_error("Errore durante l'inizializzazione di zlib");
    }

    int result = inflate(&strm, Z_FINISH);
    if (result != Z_STREAM_END) {
        inflateEnd(&strm);
        throw std::runtime_error("Errore durante la decompressione del blocco");
    }

    inflateEnd(&strm);
    decompressedData.resize(strm.total_out); // Adatta alla dimensione effettiva decompressa
    return decompressedData;
}

// Logica principale per decomprimere
void decompressPkz(const std::string& inputFilename) {
    auto data = readFile(inputFilename);

    // Verifica identificativo
    const uint32_t IDENT_BIG = 0xb0b1bEbA;
    const uint32_t IDENT_LITTLE = 0xbabeb1b0;
    uint32_t ident = *reinterpret_cast<uint32_t*>(data.data());

    bool isBigEndian = false;
    if (ident == IDENT_BIG) {
        isBigEndian = true;
    } else if (ident != IDENT_LITTLE) {
        throw std::runtime_error("Identificativo del file non valido");
    }

    size_t offset = 4; // Inizio lettura dopo l'identificativo

    // Lettura dell'offset dati compressi
    uint32_t dataOffset = *reinterpret_cast<uint32_t*>(data.data() + offset);
    offset += 4;

    if (isBigEndian) {
        dataOffset = __builtin_bswap32(dataOffset);
    }

    // Salvataggio header
    std::string headerFilename = inputFilename + ".header";
    writeFile(headerFilename, {data.begin(), data.begin() + dataOffset});

    // Lettura dimensione blocchi
    uint32_t blockSize = *reinterpret_cast<uint32_t*>(data.data() + offset);
    offset += 4;
    if (isBigEndian) {
        blockSize = __builtin_bswap32(blockSize);
    }

    // Calcolo del numero di blocchi
    size_t loops = (data.size() - dataOffset) / blockSize;

    // Prepara output decompressi
    std::vector<unsigned char> decompressedData;

    // Decompressione di ciascun blocco
    size_t currentOffset = dataOffset;
    for (size_t i = 0; i < loops; ++i) {
        size_t compressedSize = blockSize;
        if (currentOffset + blockSize > data.size()) {
            compressedSize = data.size() - currentOffset;
        }

        auto decompressedBlock = decompressBlock(data.data() + currentOffset, compressedSize, blockSize * 10);
        decompressedData.insert(decompressedData.end(), decompressedBlock.begin(), decompressedBlock.end());

        currentOffset += blockSize;
    }

    // Salva i dati decompressi
    std::string decompressedFilename = inputFilename + ".decomp";
    writeFile(decompressedFilename, decompressedData);

    std::cout << "Dati decompressi salvati in: " << decompressedFilename << std::endl;
    std::cout << "Header salvato in: " << headerFilename << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <file_input.pkz>" << std::endl;
        return 1;
    }

    try {
        decompressPkz(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Errore: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
