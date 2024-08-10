#include <iostream> 
#include <fstream>    
#include <random>     
#include <cstring>   

// Enumeración para definir tamaños de archivo
enum FileSize {
    SMALL,   
    MEDIUM,  
    LARGE    
};

// Función para obtener el tamaño de archivo basado en una cadena de entrada
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

// Función para convertir el enum FileSize a bytes
size_t getFileSizeInBytes(FileSize size) {
    switch (size) {
        case SMALL:
            return 512 * 1024 * 1024ull; // 512 MB
        case MEDIUM:
            return 1024 * 1024 * 1024ull; // 1 GB
        case LARGE:
            return 2 * 1024 * 1024 * 1024ull; // 2 GB
        default:
            throw std::invalid_argument("Unknown file size.");
    }
}

// Función para generar un archivo binario con números aleatorios
void generateFile(const std::string& outputPath, size_t fileSize) {
    std::ofstream outFile(outputPath, std::ios::binary); // Abre el archivo en modo binario
    if (!outFile) {
        throw std::runtime_error("Failed to open output file.");
    }

    std::random_device rd;              // Dispositivo para generar semillas aleatorias
    std::mt19937 gen(rd());             // Generador de números aleatorios Mersenne Twister
    std::uniform_int_distribution<int> dist; // Distribución uniforme para enteros

    size_t numbersToGenerate = fileSize / sizeof(int); // Calcula cuántos números enteros caben en el archivo

    for (size_t i = 0; i < numbersToGenerate; ++i) {
        int randomNumber = dist(gen);  // Genera un número aleatorio
        
        outFile.write(reinterpret_cast<char*>(&randomNumber), sizeof(randomNumber)); // Escribe el número en el archivo
    }

    outFile.close(); // Cierra el archivo
}

// Función principal para manejar argumentos y ejecutar la generación de archivo
int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: generator -size <SIZE> -output <OUTPUT FILE PATH>\n";
        return 1;
    }

    std::string sizeArg;   // Cadena para almacenar el argumento de tamaño
    std::string outputPath; // Cadena para almacenar la ruta del archivo de salida

    // Procesa los argumentos de la línea de comandos
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-size") == 0 && i + 1 < argc) {
            sizeArg = argv[++i];
        } else if (std::strcmp(argv[i], "-output") == 0 && i + 1 < argc) {
            outputPath = argv[++i];
        }
    }

    try {
        FileSize size = getSize(sizeArg); // Obtiene el tamaño del archivo
        size_t fileSize = getFileSizeInBytes(size); // Convierte el tamaño a bytes
        generateFile(outputPath, fileSize); // Genera el archivo binario
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n"; // Maneja errores
        return 1;
    }

    return 0;
}

/* Ejemplo de uso:
   .\generator -size SMALL -output "OUTPUT FILE PATH\numeros.bin"
*/
