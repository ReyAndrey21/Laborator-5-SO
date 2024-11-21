#include <iostream>
#include <Windows.h>
#include <ctime>
#include <cstdlib>

#define SHM_NAME L"Local\\shm_counter_cpp"  
#define SEM_NAME L"Local\\sem_counter_cpp"  
#define MAX_COUNT 1000


struct SharedData {
    int counter;
};

int main() {
    
    srand(static_cast<unsigned>(time(nullptr)));

    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,      
        nullptr,                   
        PAGE_READWRITE,            
        0,                         
        sizeof(SharedData),        
        SHM_NAME                   
    );

    if (hMapFile == nullptr) {
        std::cerr << "Nu am putut crea obiectul de memorie partajata. Cod eroare: " << GetLastError() << "\n";
        return 1;
    }

    
    SharedData* data = (SharedData*)MapViewOfFile(
        hMapFile,                  
        FILE_MAP_ALL_ACCESS,       
        0, 0,                      
        sizeof(SharedData)         
    );

    if (data == nullptr) {
        std::cerr << "Nu am putut mapa obiectul de memorie partajata. Cod eroare: " << GetLastError() << "\n";
        CloseHandle(hMapFile);
        return 1;
    }

    
    data->counter = 1;

    
    HANDLE hSemaphore = CreateSemaphore(
        nullptr,                   
        1,                         
        1,                         
        SEM_NAME                   
    );

    if (hSemaphore == nullptr) {
        std::cerr << "Nu am putut crea semaforul. Cod eroare: " << GetLastError() << "\n";
        UnmapViewOfFile(data);
        CloseHandle(hMapFile);
        return 1;
    }

    while (data->counter <= MAX_COUNT) {
        
        WaitForSingleObject(hSemaphore, INFINITE);

        int current_count = data->counter;
    
        int dice = rand() % 2 + 1;
  
        if (dice == 2) {
            std::cout << "Proces " << GetCurrentProcessId() << ": Contorul este " << current_count << std::endl;
            data->counter = current_count + 1;
        }
        
        ReleaseSemaphore(hSemaphore, 1, nullptr);

        
        Sleep(1);  
    }
    
    UnmapViewOfFile(data);
    CloseHandle(hMapFile);
    CloseHandle(hSemaphore);

    return 0;
}
