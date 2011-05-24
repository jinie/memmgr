/* Copyright (C) 2002 Jimmy Selgen Nielsen (nielsenj(at)mail.tele.dk). All Rights Reserved. */
/*
*File: memmgr.c, memmgr.h
*Desc: Memory manager that runs on a small (unsigned 16 bit value) character array
*Author: Jimmy Selgen Nielsen
*Date: 13/02/2002
*Notes:
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <stdlib.h>
#include "memmgr.h"
#include "assert.h"




typedef U32 UsedBlockType;
typedef struct FreeBlockType_
{
	UsedBlockType BlockSize;
	struct FreeBlockType_ * next;
}FreeBlockType;

static FreeBlockType *FBLHeader=NULL;
static U8 * heaparea=NULL;
static U32 heapsize;

/* Initialise memmgr */
void m_init(unsigned char * heap, U32 size)
{
	assert(heap && (size > 0) && (FBLHeader==NULL));

	heapsize = size;
	heaparea = heap;

	FBLHeader=(FreeBlockType *)heaparea;
	FBLHeader->BlockSize = (heapsize - sizeof(FreeBlockType));
	FBLHeader->next = NULL;
}

/* "Reset" memmgr */
void m_destroy(void)
{
	FBLHeader =  NULL;
	heaparea = NULL;
        heapsize = 0;
}

void * m_malloc(U32 blocksize)
{
	FreeBlockType *blockptr = FBLHeader, *previous = NULL;
	U32 bsize = sizeof(UsedBlockType);
	bsize += (blocksize > (sizeof(FreeBlockType) - sizeof(UsedBlockType))) ? blocksize : (sizeof(FreeBlockType) - sizeof(UsedBlockType));
	bsize += bsize % MINIMUM_BLOCKSIZE;
 	assert(blocksize > 0 && heaparea != 0);

	/* Find first free block large enough to hold blocksize + sizeof(UsedBlockTypy) */
  	while(blockptr!=NULL)
  	{
  		if(blockptr->BlockSize >= bsize)
  			break;
  		else
  		{
  			previous = blockptr;
  			blockptr=blockptr->next;
  		}
  	}

	/* Allocate new block */
	if(blockptr!=NULL)
	{
		FreeBlockType *usedblock = (FreeBlockType *)blockptr;

		/* If block is larger than the requested size, insert the remainder into the FBL */
		if(blockptr->BlockSize > bsize)
		{
			/* Move (freeblock) bl ptr forward */
			blockptr= (FreeBlockType*)((U8 *)blockptr+bsize );
			blockptr->BlockSize = usedblock->BlockSize - bsize;
			blockptr->next = usedblock->next;

		}
		else /* Remove block from FBL */
		{
			/* Add remainder of blocksize to allocated block */
			bsize += (usedblock->BlockSize - bsize);
			blockptr = usedblock->next;
		}

		/* Update previous */
		if(previous!=NULL)
			previous->next=blockptr;
		else //bl is FBLHeader
			FBLHeader = blockptr;

		usedblock->next = NULL;
		usedblock->BlockSize = bsize;
		return((U8 *)usedblock+sizeof(UsedBlockType));
	}
	return(NULL);
}

void m_free(void * ptr)
{
	FreeBlockType *head = FBLHeader, *previous=NULL, *current = (FreeBlockType *)((U8 *)ptr - sizeof(UsedBlockType));
	assert(ptr!=NULL);
	assert(((U8 *)ptr > heaparea) && ((U8 *)ptr < (heaparea + heapsize)));
	while(head!=NULL)
	{	
		if(current < head)
			break;
		previous=head;
		head=head->next;
	}
	if(previous!=NULL)
	{
		/* Insert current */
		current->next = previous->next;
		previous->next = current;
		/* Merge with next block */
		if(current->next!=NULL)
			if((FreeBlockType *)((unsigned char *)current + current->BlockSize) == current->next)
			{
				current->BlockSize+=current->next->BlockSize;
				current->next=current->next->next;
			}
		/* Merge with previous block ? */
		if((FreeBlockType *)((unsigned char *)previous + previous->BlockSize) == current)
		{
			previous->BlockSize+=current->BlockSize;
			previous->next=current->next;
		}
	}
	else // Block is to be inserted before FBLHeader
	{
		current->next = FBLHeader;
		FBLHeader=current;
		/* Merge with next block */
		if(current->next!=NULL)
			if((FreeBlockType *)((unsigned char *)current + current->BlockSize ) == current->next)
			{
				current->BlockSize+=current->next->BlockSize;
				current->next=current->next->next;
			}
	}
}


/* Returns largest available single block */
U32 m_largest_free_block(void)
{
	FreeBlockType *blockptr = FBLHeader;
	U32 blockfree = 0;

	assert(heaparea);

	while(blockptr!=NULL)
	{
		if(blockptr->BlockSize > blockfree)
			blockfree = blockptr->BlockSize;
		blockptr=blockptr->next;
	}
	if(blockfree > 0)
	{
		blockfree-=sizeof(UsedBlockType);
		blockfree-=blockfree%MINIMUM_BLOCKSIZE;
	}
	return((blockfree > 0) ? blockfree : 0);
}

/* Returns total free heap */
U32 m_free_heap(void)
{
	FreeBlockType * blockptr = FBLHeader;
	U32 memfree=0;
	assert(heaparea);

	while(blockptr!=NULL)
	{
		memfree += blockptr->BlockSize;
		blockptr=blockptr->next;
	}
	return(memfree);
}


#ifdef WITH_OWN_LIBC
void * m_memcpy(void *dst, const void *src, unsigned long length)
{
	U8 *Dst = (U8 *)dst, *Src = (U8 *)src;
	while(length-->0)
		*Dst++=*Src++;
	return(dst);
}
/*------------------------------------------------------------------------*/
void * m_memset(void *dst, int c, unsigned long length)
{
	U8 * Dst = (U8 *) dst;
	while(length-- > 0)
		*Dst++ = (U8)c;
	return(dst);
}


void * m_memmove( void *s, const void *ct, U32 n )
{
   	U8 *so = s;
	if( so > (U8 *)ct )	/* copy from right to left */
	{
		for(	so+=n, ct = (U8 *)ct + n;
			n--;
			ct = (U8 *)ct - 1, *--so = *(U8 *)ct
		   )
			;
	}
	else		/* copy from left to right */
	{
		for( ; n--; *so++=*(U8 *)ct, ct = (U8 *)ct + 1 )
			;
	}
	return( (void *)s );
}
/*------------------------------------------------------------------------*/
U8 * m_strcpy(U8 *Dst, const U8 *Src)
   {
   U8 *D = Dst;
   while ((*D++ = *Src++)!=0);
   return Dst;
   }

/*------------------------------------------------------------------------*/

size_t m_strlen(const U8 *S)
   {
   size_t Result;
   for (Result = 0; *S++; Result++);
   return Result;
   }

/*------------------------------------------------------------------------*/
#endif