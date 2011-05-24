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

#define U32 unsigned long
#define S32 signed long
#define U16 unsigned short
#define U8 unsigned char
#define MINIMUM_BLOCKSIZE 4

void m_init(unsigned char * heap,U32 size);
void m_destroy(void);

void * m_malloc(U32 blocksize);
void m_free(void * ptr);

U32 m_largest_free_block(void);
U32 m_free_heap(void);

#ifdef WITH_OWN_LIBC
void * m_memcpy(void *dst, const void *src, unsigned long length);
void * m_memset(void *dst, int c, unsigned long length);
U8 * m_strcpy(U8 *dst, const U8 *src);
size_t m_strlen(const U8 *S);
#endif