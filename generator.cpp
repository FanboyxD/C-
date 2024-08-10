#include <iostream>
#include <fstream>
#include <random>
#include <cstring>

enum FileSize {
    SMALL,
    MEDIUM,
    LARGE
};

FileSize getSize(const std::string& sizeStr) {
    if (sizeStr == "SMALL") {
        return SMALL;
    } else if (sizeStr == "MEDIUM") {
        return MEDIUM;
    } else if (sizeStr == "LARGE") {
        return LARGE;
    } else {
        throw std::invalid_argument("Tamano invalido. El tamano debe ser SMALL, MEDIUM, o LARGE.");
    }
}

size_t getFileSizeInBytes(FileSize size) {
    switch (size) {
        case SMALL:
            return 32 * 1024 * 1024; // 512 MB
        case MEDIUM:
            return 1024 * 1024 * 1024; // 1 GB
        case LARGE:
            return 2 * 32 * 1024 * 1024; // 2 GB
        default:
            throw std::invalid_argument("Unknown file size.");
    }
}

void generateFile(const std::string& outputPath, size_t fileSize) {
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to open output file.");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist;

    size_t numbersToGenerate = fileSize / sizeof(int);

    for (size_t i = 0; i < numbersToGenerate; ++i) {
    int randomNumber = dist(gen);
    if (i < 10) { // Imprime los primeros 10 números generados para verificar
        std::cout << "Generated number: " << randomNumber << std::endl;
    }
    outFile.write(reinterpret_cast<char*>(&randomNumber), sizeof(randomNumber));
}


    outFile.close();
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: generator -size <SIZE> -output <OUTPUT FILE PATH>\n";
        return 1;
    }

    std::string sizeArg;
    std::string outputPath;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-size") == 0 && i + 1 < argc) {
            sizeArg = argv[++i];
        } else if (std::strcmp(argv[i], "-output") == 0 && i + 1 < argc) {
            outputPath = argv[++i];
        }
    }

    try {
        FileSize size = getSize(sizeArg);
        size_t fileSize = getFileSizeInBytes(size);
        generateFile(outputPath, fileSize);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

/*.\generator -size SMALL -output "C:\Users\Administrator\Documents\Datos II\output.bin"
*/