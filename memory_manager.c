#include "memory_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Structure to represent a memory block
typedef struct MemBlock {
    size_t offset;          // Offset into the memory pool
    size_t size;            // Size of the block
    int is_free;            // Whether the block is free
    struct MemBlock* next;  // Next block in the list
} MemBlock;

// Global variables
static char* memory_pool = NULL;
static size_t pool_size = 0;
static MemBlock* block_list = NULL;  // List of memory blocks


// Initialize the memory manager with a memory pool of the given size
void mem_init(size_t size) {
    memory_pool = (char*)malloc(size);
    if (memory_pool == NULL) {
        fprintf(stderr, "Failed to initialize memory pool.\n");
        exit(EXIT_FAILURE);
    }
    pool_size = size;

    // Initialize the block list with a single large free block
    block_list = (MemBlock*)malloc(sizeof(MemBlock));
    if (block_list == NULL) {
        fprintf(stderr, "Failed to allocate block metadata.\n");
        exit(EXIT_FAILURE);
    }
    block_list->offset = 0;
    block_list->size = size;
    block_list->is_free = 1;
    block_list->next = NULL;
}

// Allocate a block of memory of the given size
void* mem_alloc(size_t size) {
    if (size == 0) {
        // Return the data area of the first free block without allocating
        MemBlock* current = block_list;
        while (current != NULL) {
            if (current->is_free) {
                return memory_pool + current->offset;  // Return the memory pool offset
            }
            current = current->next;
        }
        return NULL;  // No free block found
    }

    MemBlock* current = block_list;
    MemBlock* prev = NULL;
    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            // Found a suitable block
            if (current->size > size) {
                // Split the block
                MemBlock* new_block = (MemBlock*)malloc(sizeof(MemBlock));
                if (new_block == NULL) {
                    return NULL;
                }
                new_block->offset = current->offset + size;
                new_block->size = current->size - size;
                new_block->is_free = 1;
                new_block->next = current->next;

                current->size = size;
                current->is_free = 0;
                current->next = new_block;
            } else {
                // Exact fit
                current->is_free = 0;
            }
            return memory_pool + current->offset;
        }
        prev = current;
        current = current->next;
    }
    // No suitable block found
    return NULL;
}

// Free the memory block pointed to by ptr
void mem_free(void* ptr) {
    if (ptr == NULL) return;
    size_t offset = (char*)ptr - memory_pool; // Calculate the offset

    MemBlock* current = block_list;
    MemBlock* prev = NULL;
    while (current != NULL) {
        if (current->offset == offset) {
            if (current->is_free) {
                // Block is already free
                return;
            }
            current->is_free = 1;

            // Merge with next block if it's free
            if (current->next != NULL && current->next->is_free) {
                MemBlock* next_block = current->next;
                current->size += next_block->size;
                current->next = next_block->next;
                free(next_block);
            }

            // Merge with previous block if it's free
            if (prev != NULL && prev->is_free) {
                prev->size += current->size;
                prev->next = current->next;
                free(current);
                current = prev;
            }
            break;
        }
        prev = current;
        current = current->next;
    }
}

// Resize the memory block pointed to by ptr to the given size
void* mem_resize(void* ptr, size_t size) {
    if (ptr == NULL) return mem_alloc(size);
    if (size == 0) {
        mem_free(ptr);
        return NULL;
    }
    size_t offset = (char*)ptr - memory_pool;  // Calculate the offset

    MemBlock* current = block_list;
    while (current != NULL) {
        if (current->offset == offset) {
            if (current->size >= size) {
                // Current block is large enough
                if (current->size > size) {
                    // Split the block
                    MemBlock* new_block = (MemBlock*)malloc(sizeof(MemBlock));
                    if (new_block == NULL) {
                        return NULL;
                    }
                    new_block->offset = current->offset + size;
                    new_block->size = current->size - size;
                    new_block->is_free = 1;
                    new_block->next = current->next;

                    current->size = size;
                    current->next = new_block;
                }
                return ptr;
            } else {
                // Try to expand into next block if it's free
                if (current->next != NULL && current->next->is_free &&
                    current->size + current->next->size >= size) {
                    // Merge with next block
                    current->size += current->next->size;
                    MemBlock* next_block = current->next;
                    current->next = next_block->next;
                    free(next_block);

                    if (current->size > size) {
                        // Split the block
                        MemBlock* new_block = (MemBlock*)malloc(sizeof(MemBlock));
                        if (new_block == NULL) {
                            return NULL;
                        }
                        new_block->offset = current->offset + size;
                        new_block->size = current->size - size;
                        new_block->is_free = 1;
                        new_block->next = current->next;

                        current->size = size;
                        current->next = new_block;
                    }
                    return ptr;   // Return the allocated memory
                } else {
                    // Allocate new block
                    void* new_ptr = mem_alloc(size);
                    if (new_ptr != NULL) {
                        memcpy(new_ptr, ptr, current->size);
                        mem_free(ptr);
                    }
                    return new_ptr;
                }
            }
        }
        current = current->next;
    }
    return NULL; // Block not found
}

// Deinitialize the memory manager
void mem_deinit() {
    if (memory_pool != NULL) {
        free(memory_pool);
        memory_pool = NULL;
        pool_size = 0;
    }
    // Free block metadata
    MemBlock* current = block_list;
    while (current != NULL) {
        MemBlock* next = current->next;
        free(current);
        current = next;
    }
    block_list = NULL;
}
