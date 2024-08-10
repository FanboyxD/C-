#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <cstdlib>

using namespace std;
using namespace std::chrono;

// Clase que maneja la paginación
class PagedArray {
private:
    static const int PAGE_SIZE = 1024; // Tamaño de página en número de enteros
    static const int NUM_PAGES = 4; // Número máximo de páginas en memoria

    // Estructura que representa una página de enteros
    struct Page {
        int data[PAGE_SIZE]; // Datos de la página
        int pageNumber; // Número de la página en el archivo
        bool dirty; // Indicador de si la página ha sido modificada
    };

    vector<Page> pages; // Almacena las páginas actualmente en memoria
    unordered_map<int, int> pageTable; // Mapa de número de página a índice en `pages`
    fstream file; // Archivo binario

    int pageFaults = 0; // Contador de fallos de página
    int pageHits = 0; // Contador de aciertos de página

    void loadPage(int pageNumber); // Carga una página en memoria
    Page loadPageFromFile(int pageNumber); // Carga una página desde el archivo
    void savePage(Page& page); // Guarda una página en el archivo si ha sido modificada

public:
    PagedArray(const string& filePath); // Constructor que abre el archivo binario
    ~PagedArray(); // Destructor que guarda las páginas modificadas y cierra el archivo

    int& operator[](int index); // Sobrecarga del operador [] para acceso similar a un arreglo

    int getPageFaults() const; // Retorna el número de fallos de página
    int getPageHits() const; // Retorna el número de aciertos de página

    PagedArray& operator=(const PagedArray&) = delete;
};

// Implementación del método `loadPage`
void PagedArray::loadPage(int pageNumber) {
    // Verificar si la página ya está cargada
    if (pageTable.find(pageNumber) != pageTable.end()) {
        pageHits++; // Incrementar el contador de aciertos de página
        return; // La página ya está en memoria
    }

    pageFaults++; // Incrementar el contador de fallos de página

    // Si todas las páginas están ocupadas, reemplazar una al azar
    if (pages.size() == NUM_PAGES) {
        int pageToReplace = rand() % NUM_PAGES; // Seleccionar una página al azar
        if (pages[pageToReplace].dirty) {
            savePage(pages[pageToReplace]); // Guardar la página si ha sido modificada
        }
        pageTable.erase(pages[pageToReplace].pageNumber); // Eliminar la página reemplazada del mapa
        pages[pageToReplace] = loadPageFromFile(pageNumber); // Cargar la nueva página
        pageTable[pageNumber] = pageToReplace; // Actualizar el mapa de páginas
    } else {
        // Si hay espacio, agregar una nueva página
        Page newPage = loadPageFromFile(pageNumber);
        pages.push_back(newPage);
        pageTable[pageNumber] = pages.size() - 1; // Guardar el índice de la nueva página
    }
}

// Cargar una página desde el archivo
PagedArray::Page PagedArray::loadPageFromFile(int pageNumber) {
    Page page;
    page.pageNumber = pageNumber;
    page.dirty = false; // Página recién cargada, no ha sido modificada
    file.seekg(pageNumber * PAGE_SIZE * sizeof(int), ios::beg); // Posicionar el puntero de lectura
    file.read(reinterpret_cast<char*>(page.data), PAGE_SIZE * sizeof(int)); // Leer la página
    return page;
}

// Guardar una página en el archivo
void PagedArray::savePage(Page& page) {
    file.seekp(page.pageNumber * PAGE_SIZE * sizeof(int), ios::beg); // Posicionar el puntero de escritura
    file.write(reinterpret_cast<char*>(page.data), PAGE_SIZE * sizeof(int)); // Escribir la página
    page.dirty = false; // Marcar la página como no modificada después de guardarla
}

// Constructor que abre el archivo binario
PagedArray::PagedArray(const string& filePath) : file(filePath, ios::in | ios::out | ios::binary) {
    if (!file.is_open()) {
        throw runtime_error("No se pudo abrir el archivo"); // Lanzar excepción si el archivo no se abre
    }
}

// Destructor que guarda las páginas modificadas y cierra el archivo
PagedArray::~PagedArray() {
    for (auto& page : pages) {
        if (page.dirty) {
            savePage(page); // Guardar las páginas modificadas en el archivo
        }
    }
    file.close(); // Cerrar el archivo
}

// Sobrecarga del operador [] para acceso a elementos
int& PagedArray::operator[](int index) {
    int pageNumber = index / PAGE_SIZE; // Calcular el número de página
    int offset = index % PAGE_SIZE; // Calcular el desplazamiento dentro de la página
    loadPage(pageNumber); // Cargar la página si no está en memoria
    int pageIndex = pageTable[pageNumber];
    pages[pageIndex].dirty = true; // Marcar la página como modificada
    return pages[pageIndex].data[offset]; // Retornar la referencia al elemento
}

// Retorna el número de fallos de página
int PagedArray::getPageFaults() const {
    return pageFaults;
}

// Retorna el número de aciertos de página
int PagedArray::getPageHits() const {
    return pageHits;
}

// Declaraciones de funciones de ordenamiento
void quickSort(PagedArray& arr, int low, int high);
int partition(PagedArray& arr, int low, int high);
void insertionSort(PagedArray& arr, int n);
void bubbleSort(PagedArray& arr, int n);

// Implementaciones de funciones de ordenamiento

// Implementación de Quick Sort
void quickSort(PagedArray& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// Función auxiliar para Quick Sort
int partition(PagedArray& arr, int low, int high) {
    int pivot = arr[high]; // Elegir el pivote
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (arr[j] < pivot) {
            ++i;
            swap(arr[i], arr[j]); // Intercambiar elementos
        }
    }
    swap(arr[i + 1], arr[high]); // Colocar el pivote en su posición final
    return i + 1;
}

// Implementación de Insertion Sort
void insertionSort(PagedArray& arr, int n) {
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

// Implementación de Bubble Sort
void bubbleSort(PagedArray& arr, int n) {
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]); // Intercambiar elementos
            }
        }
    }
}

// Convertir el archivo binario y ordenar los números
void convertAndSort(const string& inputFilePath, const string& outputFilePath, const string& sortAlgorithm) {
    ifstream inFile(inputFilePath, ios::binary);
    if (!inFile) {
        throw runtime_error("Fallo al abrir el archivo de entrada: " + inputFilePath); // Lanzar excepción si el archivo no se abre
    }

    // Leer el contenido del archivo binario y guardarlo en un vector
    inFile.seekg(0, ios::end);
    size_t fileSize = inFile.tellg(); // Obtener el tamaño del archivo
    inFile.seekg(0, ios::beg);
    int numElements = fileSize / sizeof(int); // Calcular el número de elementos
    vector<int> numbers(numElements);
    inFile.read(reinterpret_cast<char*>(numbers.data()), fileSize); // Leer los datos del archivo
    inFile.close();

    // Escribir los números ordenados en el archivo de salida de texto
    ofstream outTextFile(outputFilePath);
    if (!outTextFile) {
        throw runtime_error("Fallo al abrir el archivo de salida: " + outputFilePath); // Lanzar excepción si el archivo no se abre
    }

    PagedArray arr(inputFilePath); // Crear una instancia de PagedArray para el archivo binario
    for (int i = 0; i < numElements; ++i) {
        arr[i] = numbers[i]; // Cargar los números en el PagedArray
    }

    // Ordenar los números según el algoritmo especificado
    auto start = high_resolution_clock::now(); // Marcar el inicio del tiempo
    if (sortAlgorithm == "QS") {
        quickSort(arr, 0, numElements - 1);
    } else if (sortAlgorithm == "IS") {
        insertionSort(arr, numElements);
    } else if (sortAlgorithm == "BS") {
        bubbleSort(arr, numElements);
    } else {
        throw invalid_argument("Algoritmo invalido: " + sortAlgorithm); // Lanzar excepción si el algoritmo no es válido
    }
    auto end = high_resolution_clock::now(); // Marcar el final del tiempo
    auto duration = duration_cast<milliseconds>(end - start).count(); // Calcular la duración

    // Escribir los números ordenados en el archivo de salida
    bool first = true;
    for (int i = 0; i < numElements; ++i) {
        if (!first) {
            outTextFile << ", "; // Separar los números con comas
        }
        outTextFile << arr[i];
        first = false;
    }

    outTextFile.close();
    cout << "Resumen:" << endl;
    cout << "Tiempo durado: " << duration << " ms" << endl; // Imprimir el tiempo de ejecución
    cout << "Algoritmo utilizado: " << sortAlgorithm << endl; // Imprimir el algoritmo utilizado
    cout << "Page faults: " << arr.getPageFaults() << endl; // Imprimir la cantidad de fallos de página
    cout << "Page hits: " << arr.getPageHits() << endl; // Imprimir la cantidad de aciertos de página
}

// Función principal que procesa los argumentos de la línea de comandos
int main(int argc, char* argv[]) {
    if (argc != 7) {
        cerr << "Uso: " << argv[0] << " --input <INPUT FILE PATH> --output <OUTPUT FILE PATH> --alg <ALGORITHM>\n"; // Imprimir uso del programa
        return 1;
    }

    string inputFilePath;
    string outputFilePath;
    string sortAlgorithm;

    for (int i = 1; i < argc; i += 2) {
        string arg = argv[i];
        string value = argv[i + 1];
        if (arg == "--input") {
            inputFilePath = value;
        } else if (arg == "--output") {
            outputFilePath = value;
        } else if (arg == "--alg") {
            sortAlgorithm = value;
        } else {
            cerr << "Argumento desconocido: " << arg << endl;
            return 1;
        }
    }

    try {
        convertAndSort(inputFilePath, outputFilePath, sortAlgorithm); // Llamar a la función de conversión y ordenamiento
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl; // Imprimir el error si ocurre una excepción
        return 1;
    }

    return 0;
}

/* Ejemplo de uso:
    ./sorter -input numeros.bin -output numeros_ordenados.txt -alg QS
*/
