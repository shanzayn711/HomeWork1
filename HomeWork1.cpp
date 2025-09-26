#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <cstdint>
#include <string>

struct Allocation 
{
    int pid;
    int startAddrMB;
    int processSizeMB;
    int pagesAllocated;
    int unusedMB;
};

int main(int argc, char* argv[]) 
{  
    const int TOTAL_PAGES = 100;   
    const int PAGE_SIZE_MB = 160;   
    const int START_ADDR_MB = 2000;  
    const int UNIT_SIZE_MB = 80;    
    const int RAND_MIN_UNITS = 1;
    const int RAND_MAX_UNITS = 30;

    std::vector<int> memory(TOTAL_PAGES, 0);

    uint64_t seed;
    if (argc > 1) 
    {
        try {
            seed = std::stoull(argv[1]);
        }
        catch (...) 
        {
            seed = std::random_device{}();
        }
    }
    else 
    {
        seed = std::random_device{}();
    }
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<int> dist(RAND_MIN_UNITS, RAND_MAX_UNITS);

    std::vector<Allocation> report;
    int nextFreePage = 0;
    int currentStartAddrMB = START_ADDR_MB;
    int pid = 1;

    while (nextFreePage < TOTAL_PAGES) 
    {
        int units = dist(rng);                      
        int processSizeMB = units * UNIT_SIZE_MB;   
        int requiredPages = (processSizeMB + PAGE_SIZE_MB - 1) / PAGE_SIZE_MB; 

        if (nextFreePage + requiredPages > TOTAL_PAGES) break; 

        for (int i = 0; i < requiredPages; ++i)
        {
            memory[nextFreePage + i] = pid;
        }

        Allocation a;
        a.pid = pid;
        a.startAddrMB = currentStartAddrMB;
        a.processSizeMB = processSizeMB;
        a.pagesAllocated = requiredPages;
        a.unusedMB = requiredPages * PAGE_SIZE_MB - processSizeMB;
        report.push_back(a);

        nextFreePage += requiredPages;
        currentStartAddrMB += requiredPages * PAGE_SIZE_MB; 
        ++pid;
    }

     std::cout << "\nSummary Report\n";
     std::cout << std::left
        << std::setw(14) << "Process Id"
        << std::setw(30) << "Starting Memory Address"
        << std::setw(30) << "Size of the Process MB"
        << std::setw(18) << "Unused Space MB"
        << "\n";

    for (const auto& a : report) 
    {
        std::cout << std::left
            << std::setw(14) << a.pid
            << std::setw(30) << a.startAddrMB
            << std::setw(30) << a.processSizeMB
            << std::setw(18) << a.unusedMB
            << "\n";
    }


    int usedPages = 0;
    for (int p : memory) if (p != 0) ++usedPages;
    int totalMB = TOTAL_PAGES * PAGE_SIZE_MB;
    int usedMB = usedPages * PAGE_SIZE_MB;
    int freeMB = totalMB - usedMB;

    std::cout << "\nMemory Usage\n";
    std::cout << "Total pages: " << TOTAL_PAGES << "  (" << totalMB << " MB)\n";
    std::cout << "Used pages : " << usedPages << "  (" << usedMB << " MB)\n";
    std::cout << "Free pages : " << (TOTAL_PAGES - usedPages) << "  (" << freeMB << " MB)\n\n";

    return 0;
}
