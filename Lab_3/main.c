#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ADDRESS_MASK 0xFFFF
#define OFFSET_MASK 0xFF
#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE 256
#define NUM_FRAMES 256
#define TOTAL_MEMORY_SIZE FRAME_SIZE * NUM_FRAMES
#define ADDRESS_FILE "addresses.txt"
#define BACKING_STORE "BACKING_STORE.bin"

// Structures
typedef struct {
    int isValid;
    int pageNumber;
    int frameNumber;
} TLBEntry;

// Global variables
int pageTable[PAGE_TABLE_SIZE];
TLBEntry tlb[TLB_SIZE];
signed char physicalMemory[NUM_FRAMES][FRAME_SIZE];
int nextFrame = 0;
int nextTLBEntry = 0;
int totalAddresses = 0;
int pageFaults = 0;
int tlbHits = 0;

// Function prototypes
void initPageTable();
void initTLB();
int checkTLB(int pageNumber);
void addToTLB(int pageNumber, int frameNumber);
void readFromBackingStore(int pageNumber);
signed char readFromPhysicalMemory(int frameNumber, int offset);
int getOffset(int logicalAddress);
int getPageNumber(int logicalAddress);

int main() {
    // Initialize the page table and TLB
    initPageTable();
    initTLB();

    // Open the address file
    FILE *addressFile = fopen(ADDRESS_FILE, "r");
    if (addressFile == NULL) {
        fprintf(stderr, "Error opening addresses file.\n");
        return -1;
    }

    // Open the backing store
    FILE *backingStore = fopen(BACKING_STORE, "rb");
    if (backingStore == NULL) {
        fprintf(stderr, "Error opening BACKING_STORE.bin file.\n");
        return -1;
    }

    int logicalAddress, physicalAddress, pageNumber, offset, frameNumber;
    signed char value;

    // Read through the address file
    while (fscanf(addressFile, "%d", &logicalAddress) == 1) {
        totalAddresses++;
        pageNumber = getPageNumber(logicalAddress);
        offset = getOffset(logicalAddress);

        // First try to get the frame from the TLB
        frameNumber = checkTLB(pageNumber);

        // If not in TLB, check the page table
        if (frameNumber == -1) {
            frameNumber = pageTable[pageNumber];

            // If the page is not in the page table, we have a page fault
            if (frameNumber == -1) {
                readFromBackingStore(pageNumber);
                pageFaults++;
                frameNumber = nextFrame - 1;
            }

            // Add the page to the TLB
            addToTLB(pageNumber, frameNumber);
        }

        // Calculate the physical address and read the value
        physicalAddress = (frameNumber << 8) | offset;
        value = readFromPhysicalMemory(frameNumber, offset);

        // Print out the virtual address, physical address, and value
        printf("Virtual address: %d Physical address: %d Value: %d\n", logicalAddress, physicalAddress, value);
    }

    // Print statistics
    printf("Page fault rate: %.3f\n", (float)pageFaults / totalAddresses * 100.0f);
    printf("TLB hit rate: %.3f\n", (float)tlbHits / totalAddresses * 100.0f);

    // Close the address file and backing store
    fclose(addressFile);
    fclose(backingStore);

    return 0;
}

// Initialize the page table with -1 to indicate that no frames are loaded
void initPageTable() {
    int i;
    for (i = 0; i < PAGE_TABLE_SIZE; i++) {
        pageTable[i] = -1;
    }
}

// Initialize the TLB with invalid entries
void initTLB() {
    int i;
    for (i = 0; i < TLB_SIZE; i++) {
        tlb[i].isValid = 0;
    }
}

// Check the TLB for a given page number
int checkTLB(int pageNumber) {
    int i;
    for (i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].isValid && tlb[i].pageNumber == pageNumber) {
            tlbHits++;
            return tlb[i].frameNumber;
        }
    }
    return -1;
}

// Add an entry to the TLB
void addToTLB(int pageNumber, int frameNumber) {
    tlb[nextTLBEntry].pageNumber = pageNumber;
    tlb[nextTLBEntry].frameNumber = frameNumber;
    tlb[nextTLBEntry].isValid = 1;
    nextTLBEntry = (nextTLBEntry + 1) % TLB_SIZE;
}

// Read a page from the backing store into physical memory
void readFromBackingStore(int pageNumber) {
    FILE *backingStore = fopen(BACKING_STORE, "rb");
    if (backingStore == NULL) {
        fprintf(stderr, "Error opening BACKING_STORE.bin file.\n");
        exit(EXIT_FAILURE);
    }

    // Seek to the page number in the backing store
    if (fseek(backingStore, pageNumber * FRAME_SIZE, SEEK_SET) != 0) {
        fprintf(stderr, "Error seeking in BACKING_STORE.bin file.\n");
        exit(EXIT_FAILURE);
    }

    // Read the page into physical memory
    if (fread(physicalMemory[nextFrame], sizeof(signed char), FRAME_SIZE, backingStore) == 0) {
        fprintf(stderr, "Error reading from BACKING_STORE.bin file.\n");
        exit(EXIT_FAILURE);
    }

    // Store the frame number in the page table
    pageTable[pageNumber] = nextFrame;

    nextFrame++;
    fclose(backingStore);
}

// Read a byte from physical memory
signed char readFromPhysicalMemory(int frameNumber, int offset) {
    return physicalMemory[frameNumber][offset];
}

// Get the page number from a logical address
int getPageNumber(int logicalAddress) {
    return (logicalAddress >> 8) & 0xFF;
}

// Get the offset from a logical address
int getOffset(int logicalAddress) {
    return logicalAddress & OFFSET_MASK;
}
