#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <deque>

class PagedArray {
private:
    std::vector<int> data;
    std::unordered_map<int, std::deque<int>::iterator> pageMap;  //Map to track pages in memory
    std::deque<int> pageQueue;  //Queue for page's order
    int pageSize;  //Size of each page
    int pageCount; //Ammout of pages

    //Replacement algorith LRU
    //Based from: https://www.geeksforgeeks.org/program-for-least-recently-used-lru-page-replacement-algorithm/
    void replaceLRU(int newPageNumber) {
        if (!pageQueue.empty()) {
            int lruPage = pageQueue.back();
            pageQueue.pop_back();
            pageMap.erase(lruPage);
        }

        pageQueue.push_front(newPageNumber);
        pageMap[newPageNumber] = pageQueue.begin();
    }

public:
    PagedArray(const std::string& filename, int pageSize);
    int& operator[](size_t index);
    size_t getSize() const;
    void convertToBinary(const std::string& binaryOutputFile);
};
size_t PagedArray::getSize() const {
    return data.size();
}

std::string binaryFileToString(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file." << std::endl;
        return "";
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer(static_cast<std::size_t>(size), '\0');
    if (file.read(&buffer[0], size)) {
        return buffer;
    } else {
        std::cerr << "Error: Could not read the file." << std::endl;
        return "";
    }
}

std::vector<int> splitStringToNumbers(const std::string& input) {
    std::vector<int> numbers;
    std::istringstream ss(input);
    std::string token;

    while (std::getline(ss, token, ',')) {
        try {
            int number = std::stoi(token);
            numbers.push_back(number);
        } catch (const std::invalid_argument&) {
            // Ignorar tokens no numéricos
        }
    }

    return numbers;
}

void PagedArray::convertToBinary(const std::string& binaryOutputFile) {
    std::ofstream outputFile(binaryOutputFile, std::ios::binary);

    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open outu" << std::endl;
        return;
    }

}
PagedArray::PagedArray(const std::string& filename, int pageSize) {
    this->pageSize = pageSize;

    std::string file = binaryFileToString(filename);

    data = splitStringToNumbers(file);

}

int& PagedArray::operator[](size_t index) {
    int pageNumber = index / pageSize;
    auto it = pageMap.find(pageNumber);
    if (it == pageMap.end()) {
        if (pageMap.size() >= pageCount) {
            replaceLRU(pageNumber);
        } else {
            pageQueue.push_front(pageNumber);
            pageMap[pageNumber] = pageQueue.begin();
        }
    } else {
        pageQueue.erase(it->second);
        pageQueue.push_front(pageNumber);
        it->second = pageQueue.begin();
    }

    int pageIndex = index % pageSize;
    return data[pageNumber * pageSize + pageIndex];
}

//Quick Sort
//Taken from: https://www.geeksforgeeks.org/cpp-program-for-quicksort/

/*
 * void quickSort_a(PagedArray& arr, size_t low, size_t high) {
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
        if (pivotIndex - low < high - pivotIndex) {
            quickSort(arr, low, pivotIndex - 1);
            quickSort(arr, pivotIndex + 1, high);
        } else {
            quickSort(arr, pivotIndex + 1, high);
            quickSort(arr, low, pivotIndex - 1);
        }
    }
} */

void swap(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

int partition(PagedArray& arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);

    return (i + 1);
}

void quickSort(PagedArray& arr, int low, int high) {
    if (low < high) {
        int partitionIndex = partition(arr, low, high);

        quickSort(arr, low, partitionIndex - 1);
        quickSort(arr, partitionIndex + 1, high);
    }
}

//Insertion Sort
//Taken from: https://www.geeksforgeeks.org/insertion-sort/
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

//Selection Sort
//Taken from: https://www.geeksforgeeks.org/selection-sort/
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

//BubbleSort
//Taken from: https://www.geeksforgeeks.org/bubble-sort/
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

    const int pageSize = 256;
    PagedArray pagedArray(inputFile, pageSize);

    size_t totalSize = pagedArray.getSize();

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
    std::string buffer = "";
    for (size_t i = 0; i < totalSize; i++) {
        int value = pagedArray[i];
        buffer = buffer + std::to_string(value);
        if (i != totalSize - 1) {
            buffer = buffer + ",";
        }

    }
    
    output.write(buffer.c_str(), buffer.size());
    output.close();

    return 0;
}