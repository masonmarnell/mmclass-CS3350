/*
 * CS3375 Computer Architecture
 * Programming Project 2
 * Cache Simulator Design and Development
 * Fall 2021
 * By Mason Marnell
 * Modified from Yong Chen's code of the same name
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "cachesim.h"

#include <stdlib.h> //imported to be able to convert strings to ints

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <direct> <trace file name>\n", argv[0]);
        return 1;
    }

#ifdef DBG
    printf("BLOCK SIZE = %d Bytes\n", BLOCK_SIZE);
    printf("%d-WAY\n", WAY_SIZE);
    printf("CACHE SIZE = %d Bytes\n", CACHE_SIZE);
    printf("NUMBER OF BLOCKS = %d\n", NUM_BLOCKS);
    printf("NUMBER OF SETS = %d\n", NUM_SETS);
    printf("\n");
#endif

    char* trace_file_name = argv[2];
    struct direct_mapped_cache d_cache;
    struct nway_cache n_cache; //making the struct for an n-way cache
    char mem_request[20];
    uint64_t address;
    FILE *fp;


    /* Initialization */
    for (int i=0; i<NUM_BLOCKS; i++) {
        d_cache.valid_field[i] = 0;
        d_cache.dirty_field[i] = 0;
        d_cache.tag_field[i] = 0;
    }
    d_cache.hits = 0;
    d_cache.misses = 0;

    for (int i=0; i<NUM_BLOCKS; i++) { //initializing the n-way cache
        n_cache.valid_field[i] = 0;
        n_cache.dirty_field[i] = 0;
        n_cache.tag_field[i] = 0;
    }
    n_cache.hits = 0;
    n_cache.misses = 0;


    /* Opening the memory trace file */
    fp = fopen(trace_file_name, "r");

    int waysArg = atoi(argv[1]);

    if (strncmp(argv[1], "direct", 6)==0) { /* Simulating direct-mapped cache */
        /* Read the memory request address and access the cache */
        while (fgets(mem_request, 20, fp)!= NULL) {
            address = convert_address(mem_request);
            direct_mapped_cache_access(&d_cache, address);
        }
        /*Print out the results*/
        printf("\n==================================\n");
        printf("Cache type:    Direct-Mapped Cache\n");
        printf("==================================\n");
        printf("Cache Hits:    %d\n", d_cache.hits);
        printf("Cache Misses:  %d\n", d_cache.misses);

        //--------------- Part 1 -----------------

        printf("Cache Hit Rate Percentage:  %.3f\n", (100*((float)d_cache.hits / ((float)d_cache.misses + (float)d_cache.hits))));
        printf("Cache Miss Rate Percentage:  %.3f\n", (100*((float)d_cache.misses / ((float)d_cache.misses + (float)d_cache.hits))));

        // converts hits and misses to floats and divides by total number of hits and misses, then multiplies by 100 to get the percentages

        //----------------------------------------

        printf("\n");
    }

    if (strncmp(argv[1], "full", 6)==0) { //Simulating Fully-Associative Cache by passing in number of blocks to nway cache
        /* Read the memory request address and access the cache */
        while (fgets(mem_request, 20, fp)!= NULL) {
            address = convert_address(mem_request);
            nway_cache_access(&n_cache, address, NUM_BLOCKS);
        }

        /*Print out the results*/
        printf("\n==================================\n");
        printf("Cache type:   Fully-Associative Cache\n");
        printf("==================================\n");
        printf("Cache Hits:    %d\n", n_cache.hits);
        printf("Cache Misses:  %d\n", n_cache.misses);

        printf("Cache Hit Rate Percentage:  %.3f\n", (100*((float)n_cache.hits / ((float)n_cache.misses + (float)n_cache.hits))));
        printf("Cache Miss Rate Percentage:  %.3f\n", (100*((float)n_cache.misses / ((float)n_cache.misses + (float)n_cache.hits))));
        
        printf("\n");
    }

    if (waysArg>=1) { //Simulating n-way cache
        /* Read the memory request address and access the cache */
        while (fgets(mem_request, 20, fp)!= NULL) {
            address = convert_address(mem_request);
            nway_cache_access(&n_cache, address, waysArg);
        }

        /*Print out the results*/
        printf("\n==================================\n");
        printf("Cache type:   %d-way Cache\n", waysArg);
        printf("==================================\n");
        printf("Cache Hits:    %d\n", n_cache.hits);
        printf("Cache Misses:  %d\n", n_cache.misses);

        printf("Cache Hit Rate Percentage:  %.3f\n", (100*((float)n_cache.hits / ((float)n_cache.misses + (float)n_cache.hits))));
        printf("Cache Miss Rate Percentage:  %.3f\n", (100*((float)n_cache.misses / ((float)n_cache.misses + (float)n_cache.hits))));
        
        printf("\n");
    }

    fclose(fp);

    return 0;
}

uint64_t convert_address(char memory_addr[])
/* Converts the physical 32-bit address in the trace file to the "binary" \\
 * (a uint64 that can have bitwise operations on it) */
{
    uint64_t binary = 0;
    int i = 0;

    while (memory_addr[i] != '\n') {
        if (memory_addr[i] <= '9' && memory_addr[i] >= '0') {
            binary = (binary*16) + (memory_addr[i] - '0');
        } else {
            if(memory_addr[i] == 'a' || memory_addr[i] == 'A') {
                binary = (binary*16) + 10;
            }
            if(memory_addr[i] == 'b' || memory_addr[i] == 'B') {
                binary = (binary*16) + 11;
            }
            if(memory_addr[i] == 'c' || memory_addr[i] == 'C') {
                binary = (binary*16) + 12;
            }
            if(memory_addr[i] == 'd' || memory_addr[i] == 'D') {
                binary = (binary*16) + 13;
            }
            if(memory_addr[i] == 'e' || memory_addr[i] == 'E') {
                binary = (binary*16) + 14;
            }
            if(memory_addr[i] == 'f' || memory_addr[i] == 'F') {
                binary = (binary*16) + 15;
            }
        }
        i++;
    }

#ifdef DBG
    printf("%s converted to %llu\n", memory_addr, binary);
#endif
    return binary;
}

void direct_mapped_cache_access(struct direct_mapped_cache *cache, uint64_t address)
{
    uint64_t block_addr = address >> (unsigned)log2(BLOCK_SIZE);
    uint64_t index = block_addr % NUM_BLOCKS;
    uint64_t tag = block_addr >> (unsigned)log2(NUM_BLOCKS);

#ifdef DBG
    printf("Memory address: %llu, Block address: %llu, Index: %llu, Tag: %llu ", address, block_addr, index, tag);
#endif

    if (cache->valid_field[index] && cache->tag_field[index] == tag) { /* Cache hit */
        cache->hits += 1;
#ifdef DBG
        printf("Hit!\n");
#endif
    } else {
        /* Cache miss */
        cache->misses += 1;
#ifdef DBG
        printf("Miss!\n");
#endif
        if (cache->valid_field[index] && cache->dirty_field[index]) {
            /* Write the cache block back to memory */
        }
        cache->tag_field[index] = tag;
        cache->valid_field[index] = 1;
        cache->dirty_field[index] = 0;
    }
}

void nway_cache_access(struct nway_cache *cache, uint64_t address, int ways) //function made to access an n way (and fully associative) cache
{
    uint64_t block_addr = address >> (unsigned)log2(BLOCK_SIZE);

    uint64_t index = block_addr % (NUM_BLOCKS / ways);
    
    uint64_t tag = block_addr >> (unsigned)log2(NUM_BLOCKS/ways);

    if(ways==NUM_BLOCKS){ //if statement used for fully associative version
    uint64_t tag = block_addr;
    uint64_t index = 0;
    }
    
    //according to the homework and online reading supplements, this should be the correct way to calculate the neccessary fields

#ifdef DBG
    printf("Memory address: %llu, Block address: %llu, Index: %llu, Tag: %llu ", address, block_addr, index, tag);
#endif

    if (cache->valid_field[index] && cache->tag_field[index] == tag) { /* Cache hit */
        cache->hits += 1;
#ifdef DBG
        printf("Hit!\n");
#endif
    }
    else if(!cache->valid_field[index] && cache->tag_field[index] == tag){ //probably not completely neccessary but added logic for miss and place
        cache->misses += 1;
        cache->tag_field[index] = tag;
        cache->valid_field[index] = 1;
        cache->dirty_field[index] = 0;
    }
    else {

        //randomly place a block
        int indexRand = rand()%(unsigned)log2(NUM_BLOCKS);
        cache->tag_field[indexRand] = tag;
        cache->valid_field[indexRand] = 1;
        cache->dirty_field[indexRand] = 0;
                
        cache->misses += 1;
#ifdef DBG
        printf("Miss!\n");
#endif
        if (cache->valid_field[index] && cache->dirty_field[index]) {
            /* Write the cache block back to memory */
        }
        cache->tag_field[index] = tag;
        cache->valid_field[index] = 1;
        cache->dirty_field[index] = 0;
    }
}