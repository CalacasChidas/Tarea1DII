#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <deque>

class PagedArray {
private:
    std::vector<int> data;
    std::unordered_map<int, std::deque<int>::iterator> pageMap;  // Mapa para rastrear las páginas en memoria
    std::deque<int> pageQueue;  // Cola para el orden de uso de las páginas
    int pageSize;  // Tamaño de cada página
    int pageCount; // Cantidad de páginas
    
    // Algoritmo de reemplazo: Least Recently Used (LRU)
    void replaceLRU(int newPageNumber) {
        // Obtenemos la página menos usada (LRU)
        int lruPage = pageQueue.back();
        pageQueue.pop_back();
        pageMap.erase(lruPage);

        // Reemplazamos la página LRU con la nueva página
        pageQueue.push_front(newPageNumber);
        pageMap[newPageNumber] = pageQueue.begin();
    }

public:
    PagedArray(const std::string& filename, int pageSize);
    int& operator[](size_t index);
size_t PagedArray::getSize() const {
    return data.size();
}
};


PagedArray::PagedArray(const std::string& filename, int pageSize) {
    this->pageSize = pageSize;
    std::ifstream inputFile(filename, std::ios::binary);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        exit(1);
    }

    inputFile.seekg(0, std::ios::end);
    size_t fileSize = inputFile.tellg();
    pageCount = fileSize / (pageSize * sizeof(int)) + (fileSize % (pageSize * sizeof(int)) == 0 ? 0 : 1);
    data.resize(pageCount * pageSize);

    inputFile.seekg(0, std::ios::beg);
    inputFile.read(reinterpret_cast<char*>(data.data()), fileSize);

    inputFile.close();
}

int& PagedArray::operator[](size_t index) {
    int pageNumber = index / pageSize;

    // Verifica si la página está cargada en memoria
    auto it = pageMap.find(pageNumber);
    if (it == pageMap.end()) {
        // La página no está en memoria, así que la cargamos
        if (pageMap.size() >= pageCount) {
            // Si estamos llenos, reemplazamos la página menos usada (LRU)
            replaceLRU(pageNumber);
        } else {
            pageQueue.push_front(pageNumber);
            pageMap[pageNumber] = pageQueue.begin();
        }
    } else {
        // La página ya está en memoria, la movemos al frente de la cola (uso reciente)
        pageQueue.erase(it->second);
        pageQueue.push_front(pageNumber);
        it->second = pageQueue.begin();
    }

    int pageIndex = index % pageSize;
    return data[pageNumber * pageSize + pageIndex];
}

// Algoritmo de ordenamiento Quick Sort
//Tomado de: https://www.geeksforgeeks.org/cpp-program-for-quicksort/
void quickSort(PagedArray& arr, size_t low, size_t high) {
    if (low < high) {
        // Partición
        int pivot = arr[high];
        size_t i = low - 1;

        for (size_t j = low; j <= high - 1; j++) {
            if (arr[j] < pivot) {
                i++;
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        size_t pivotIndex = i + 1;

        // Recursivamente ordenamos las dos mitades
        if (pivotIndex - low < high - pivotIndex) {
            quickSort(arr, low, pivotIndex - 1);
            quickSort(arr, pivotIndex + 1, high);
        } else {
            quickSort(arr, pivotIndex + 1, high);
            quickSort(arr, low, pivotIndex - 1);
        }
    }
}

// Algoritmo de ordenamiento Insertion Sort
//Tomado de: https://www.geeksforgeeks.org/insertion-sort/
void insertionSort(PagedArray& arr, size_t size) {
    for (size_t i = 1; i < size; i++) {
        int key = arr[i];
        size_t j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Algoritmo de ordenamiento Selection Sort
//Tomado de: https://www.geeksforgeeks.org/selection-sort/
void selectionSort(PagedArray& arr, size_t size) {
    for (size_t i = 0; i < size - 1; i++) {
        size_t minIndex = i;

        for (size_t j = i + 1; j < size; j++) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }

        if (minIndex != i) {
            std::swap(arr[i], arr[minIndex]);
        }
    }
}

// Algoritmo de ordenamiento Propuesto (BubbleSort)
//Tomado de: https://www.geeksforgeeks.org/bubble-sort/
void bubbleSort(PagedArray& arr, size_t size) {
    for (size_t i = 0; i < size - 1; i++) {
        bool swapped = false;

        for (size_t j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }

        if (!swapped) {
            // Si no se realizó ninguna operación de intercambio en esta iteración, el arreglo ya está ordenado
            break;
        }
    }
}

int main(int argc, char* argv[]) {
     if (argc != 7) {
        std::cerr << "Usage: paged-sort -i <input_file> -a {QS|IS|SS|PS} -o <output_file>" << std::endl;
        return 1;
    }

    std::string inputFile;
    std::string algorithm;
    std::string outputFile;

    for (int i = 1; i < argc; i += 2) {
        std::string arg = argv[i];
        std::string value = argv[i + 1];

        if (arg == "-i") {
            inputFile = value;
        } else if (arg == "-a") {
            algorithm = value;
        } else if (arg == "-o") {
            outputFile = value;
        }
    }

    if (algorithm != "QS" && algorithm != "IS" && algorithm != "SS" && algorithm != "PS") {
        std::cerr << "Invalid algorithm choice. Please choose one of: QS, IS, SS, PS." << std::endl;
        return 1;
    }

    const int pageSize = 256;  // Tamaño de página
    PagedArray pagedArray(inputFile, pageSize);  // Crear instancia de PagedArray

    size_t totalSize = pagedArray.getSize();  // Obtener el tamaño total de la lista

    if (algorithm == "QS") {
        quickSort(pagedArray, 0, totalSize - 1);
    } else if (algorithm == "IS") {
        insertionSort(pagedArray, totalSize);
    } else if (algorithm == "SS") {
        selectionSort(pagedArray, totalSize);
    } else if (algorithm == "PS") {
        bubbleSort(pagedArray, totalSize);
    }

    std::ofstream output(outputFile, std::ios::binary);
    if (!output.is_open()) {
        std::cerr << "Error: Unable to open output file " << outputFile << std::endl;
        return 1;
    }

    for (size_t i = 0; i < totalSize; i++) {
        int value = pagedArray[i];
        output.write(reinterpret_cast<char*>(&value), sizeof(value));
    }

    output.close();

    return 0;
}
