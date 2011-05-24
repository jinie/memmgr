#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define _NDEBUG
#include "memmgr.h"

int allocate_all(void *ptarr[],size_t block_size){
    int i = 0;
    void *pt = m_malloc(block_size);
    while(pt!=NULL){
        ptarr[i++] = pt;
        pt = m_malloc(block_size);
    }
    return i;
}

void free_all(void *ptarr[]){
    int i = 0;
    while(ptarr[i] != NULL){
        m_free(ptarr[i]);
        ptarr[i] = NULL;
        i++;
    }
}

int main(void)
{
    unsigned char *charr= malloc(2048000);
    static void * ptarr[2048000];

    int max = 0;
    int i;
    int size;
    m_init(charr,2048000);
    const int sizes[] = {16,13,65,51,182,1};
    printf("Largest block available %ld, free heap size %ld\n",m_largest_free_block(),m_free_heap());

    for(size = 0; size < 6; size++){
        printf("Allocating all memory in %d byte blocks\n",sizes[size]);
        max = allocate_all(ptarr,sizes[size]);
        printf("Allocation failed for block %d, largest block available %u, free heap size %ld\n",max,(unsigned int)m_largest_free_block(),m_free_heap());
        printf("Deallocating %d blocks\n",max);
        free_all(ptarr);
        printf("Deallocation done, largest block available %ld, free heap size %ld\n",m_largest_free_block(),m_free_heap());
        printf("================================================================================\n");
    }
    m_destroy();
	free(charr);
    
    return(0);
}
