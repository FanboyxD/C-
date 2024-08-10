#ifndef PAGEDARRAY_H
#define PAGEDARRAY_H

#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <ctime>
#include <cstdlib>

class PagedArray {
public:
    PagedArray(const std::string &filePath, size_t pageSize = 1024, size_t numPages = 4)
        : filePath(filePath), pageSize(pageSize), numPages(numPages) {
        file.open(filePath, std::ios::in | std::ios::out | std::ios::binary);
        if (!file) {
            std::cerr << "Error opening file." << std::endl;
            exit(1);
        }
        std::srand(std::time(nullptr));

        // Calculate file size
        file.seekg(0, std::ios::end);
        fileSize = file.tellg() / sizeof(int);
        file.seekg(0, std::ios::beg);
    }

    ~PagedArray() {
        for (auto &page : pages) {
            if (page.second.dirty) {
                writePage(page.first, page.second.data);
            }
        }
        file.close();
    }

    int& operator[](size_t index) {
        size_t pageIndex = index / pageSize;
        size_t offset = index % pageSize;

        if (pages.find(pageIndex) == pages.end()) {
            if (pages.size() >= numPages) {
                evictPage();
            }
            loadPage(pageIndex);
        } else {
            ++pageHits;
        }

        pages[pageIndex].lastAccessed = std::time(nullptr);
        return pages[pageIndex].data[offset];
    }

    size_t getPageFaults() const { return pageFaults; }
    size_t getPageHits() const { return pageHits; }
    size_t size() const { return fileSize; }

private:
    struct Page {
        std::vector<int> data;
        bool dirty = false;
        std::time_t lastAccessed;
    };

    std::fstream file;
    std::string filePath;
    size_t pageSize;
    size_t numPages;
    size_t fileSize;
    std::unordered_map<size_t, Page> pages;
    size_t pageFaults = 0;
    size_t pageHits = 0;

    void loadPage(size_t pageIndex) {
    std::vector<int> data(pageSize, 0);
    file.seekg(pageIndex * pageSize * sizeof(int), std::ios::beg);
    file.read(reinterpret_cast<char*>(data.data()), pageSize * sizeof(int));

    for (size_t i = 0; i < data.size(); ++i) {
        if (i < 10) { // Imprime los primeros 10 elementos cargados
            std::cout << "Loaded element: " << data[i] << " from page " << pageIndex << std::endl;
        }
    }

    pages[pageIndex] = {data, false, std::time(nullptr)};
    ++pageFaults;
}

void writePage(size_t pageIndex, const std::vector<int>& data) {
    file.seekp(pageIndex * pageSize * sizeof(int), std::ios::beg);
    file.write(reinterpret_cast<const char*>(data.data()), pageSize * sizeof(int));

    for (size_t i = 0; i < data.size(); ++i) {
        if (i < 10) { // Imprime los primeros 10 elementos escritos
            std::cout << "Wrote element: " << data[i] << " to page " << pageIndex << std::endl;
        }
    }
}


    void evictPage() {
        size_t oldestPage = 0;
        std::time_t oldestTime = std::time(nullptr);

        for (const auto &page : pages) {
            if (page.second.lastAccessed < oldestTime) {
                oldestTime = page.second.lastAccessed;
                oldestPage = page.first;
            }
        }

        if (pages[oldestPage].dirty) {
            writePage(oldestPage, pages[oldestPage].data);
        }

        pages.erase(oldestPage);
    }
};

#endif // PAGEDARRAY_H
