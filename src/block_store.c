#include <stdio.h>
#include <stdint.h>
#include "../include/bitmap.h"
#include "../include/block_store.h"

// include more if you need
#include <string.h>

// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

// Defining block_t struct
// Contains 1 field
// The field is an array of unit8_t of size BLOCK_SIZE_BYTES
typedef struct block
{
    uint8_t Block[BLOCK_SIZE_BYTES];
} block_t;

// Defining block_store_t struct
// Contains 2 fields
// First field is a pointer to a bitmap
// Second field is an array of block_t types of size BLOCK_SOTRE_NUM_BLOCKS
typedef struct block_store
{
    bitmap_t *map;
    block_t blocks[BLOCK_STORE_NUM_BLOCKS];
} block_store_t;

///
/// This creates a new BS device, ready to go
/// \return Pointer to a new block storage device, NULL on error
///
block_store_t *block_store_create()
{
    // Declare and assign a block_store_t pointer using malloc
    block_store_t *bs = (block_store_t*)malloc(sizeof(block_store_t));

    // Check if the block_store_t pointer was incorrectly allocated
    // Return null
    if(!bs)
    {
        return NULL;
    }
    
    // Zero out bs
    memset(bs, 0, sizeof(block_store_t));

    // Set bitmap field of bs to an overlay using bitmap_overlay
    bs->map = bitmap_overlay(BITMAP_SIZE_BITS, &(bs->blocks[BITMAP_START_BLOCK]));

    /* This implementation is for when I implement block_store_request
    for(size_t i = BITMAP_START_BLOCK; i < BITMAP_START_BLOCK + BITMAP_NUM_BLOCKS; i++)
    {
        if(!block_store_request(bs, i))
        {
            //TODO: Handle error & break
            fprintf(stderr,"*** Could not allocate block %lu for the bitmap ***\n",i);
            break;
        }
    }
    */

   // Return pointer to block_store_t pointer
    return bs;
}

///
/// Destroys the provided block storage device
/// This is an idempotent operation, so there is no return value
/// \param bs BS device
///
void block_store_destroy(block_store_t *const bs)
{
    // First check if bs is not NULL
    // If so, continue to free fields and eventually the bs pointer
    if(bs)
    {
        // Check if bitmap field of bs is not NULL
        if(bs->map)
        {
            // If so, free that memory
            free(bs->map);
        }

        // Free pointer to bs
        free(bs);
    }
}
size_t block_store_allocate(block_store_t *const bs)
{
    UNUSED(bs);
    return 0;
}

bool block_store_request(block_store_t *const bs, const size_t block_id)
{
    /*
    if(bs && block_id <= BLOCK_STORE_NUM_BLOCKS)
    {

    }
    */
    UNUSED(bs);
    UNUSED(block_id);
    return false;
}

void block_store_release(block_store_t *const bs, const size_t block_id)
{
    UNUSED(bs);
    UNUSED(block_id);
}

size_t block_store_get_used_blocks(const block_store_t *const bs)
{
    UNUSED(bs);
    return 0;
}

size_t block_store_get_free_blocks(const block_store_t *const bs)
{
    UNUSED(bs);
    return 0;
}

size_t block_store_get_total_blocks()
{
    return 0;
}

size_t block_store_read(const block_store_t *const bs, const size_t block_id, void *buffer)
{
    UNUSED(bs);
    UNUSED(block_id);
    UNUSED(buffer);
    return 0;
}

size_t block_store_write(block_store_t *const bs, const size_t block_id, const void *buffer)
{
    UNUSED(bs);
    UNUSED(block_id);
    UNUSED(buffer);
    return 0;
}

block_store_t *block_store_deserialize(const char *const filename)
{
    UNUSED(filename);
    return NULL;
}

size_t block_store_serialize(const block_store_t *const bs, const char *const filename)
{
    UNUSED(bs);
    UNUSED(filename);
    return 0;
}