#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
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
        free(bs); // Free the blockstore pointer if it was incorrectly allocated
        return NULL;
    }
    
    // Zero out bs
    memset(bs, 0, sizeof(block_store_t));

    // Set bitmap field of bs to an overlay using bitmap_overlay
    bs->map = bitmap_overlay(BITMAP_SIZE_BITS, &(bs->blocks[BITMAP_START_BLOCK]));

    //This implementation is for when I implement block_store_request
    for(size_t i = BITMAP_START_BLOCK; i < BITMAP_START_BLOCK + BITMAP_NUM_BLOCKS; i++)
    {
        if(!block_store_request(bs, i))
        {
            //TODO: Handle error & break
            perror("Could not allocate block for the bitmap\n");
            return NULL;
            break;
        }
    }
    

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

///
/// Searches for a free block, marks it as in use, and returns the block's id
/// \param bs BS device
/// \return Allocated block's id, SIZE_MAX on error
///
size_t block_store_allocate(block_store_t *const bs)
{
    // Check for NULL params
    if(!bs)
    {
        return SIZE_MAX;
    }

    // Assign the index using bitmap_ffz
    size_t index = bitmap_ffz(bs->map);

    // Error checking
    if(index >= BLOCK_STORE_NUM_BLOCKS)
    {
        return SIZE_MAX;
    }

    // Use bitmap_set to set the bit at the current index
    bitmap_set(bs->map, index);

    // Return that index
    return index;
}

///
/// Attempts to allocate the requested block id
/// \param bs the block store object
/// \block_id the requested block identifier
/// \return boolean indicating succes of operation
///
bool block_store_request(block_store_t *const bs, const size_t block_id)
{

    //If the index is out of bounds, or bs is null
    if(bs == NULL || block_id >= BLOCK_STORE_NUM_BLOCKS)
    {
        return false; //Bad parameter, return false
    }

    // Use bitmap_test in conditional checking if the bit at block_id int the map is set
    if(bitmap_test(bs->map, block_id))
    {
        // If it already is, return false
        return false;
    }

    // Otherwise, set it using bitmap_set
    bitmap_set(bs->map, block_id);

    // Check that the bit was actually set
    if(!(bitmap_test(bs->map, block_id)))
    {
        return false;
    }

    // Here, return true, meaning we correct using bitmap_test
    return true;
}


///
/// Frees the specified block
/// \param bs BS device
/// \param block_id The block to free
///
void block_store_release(block_store_t *const bs, const size_t block_id) 
{
    // Check for NULL params
    if(bs == NULL || block_id >= BLOCK_STORE_NUM_BLOCKS)
    {
        return;
    }

    // Use bitmap_reset to release all the currently used blocks
    bitmap_reset(bs->map, block_id);
}


///
/// Counts the number of blocks marked as in use
/// \param bs BS device
/// \return Total blocks in use, SIZE_MAX on error
///
size_t block_store_get_used_blocks(const block_store_t *const bs)
{
    // Check for NULL params
    if(bs == NULL || bs->map == NULL)
    {
        return SIZE_MAX;
    }

    // Use bitmap_total_set to get the number of used blocks, return this
    return bitmap_total_set(bs->map);
}

///
/// Counts the number of blocks marked free for use
/// \param bs BS device
/// \return Total blocks free, SIZE_MAX on error
///
size_t block_store_get_free_blocks(const block_store_t *const bs)
{
    // Check for NULL params
    if(bs == NULL || bs->map == NULL)
    {
        return SIZE_MAX;
    }

    // Return the difference of total block and the number of used block to get free blocks
    return BLOCK_STORE_NUM_BLOCKS - block_store_get_used_blocks(bs);
}


///
/// Returns the total number of user-addressable blocks
///  (since this is constant, you don't even need the bs object)
/// \return Total blocks
///
size_t block_store_get_total_blocks()
{
    // Simply return BLOCK_STORE_NUM_BLOCKS
    return BLOCK_STORE_NUM_BLOCKS;
}



/// Reads data from the specified block and writes it to the designated buffer
/// \param bs BS device
/// \param block_id Source block id
/// \param buffer Data buffer to write to
/// \return Number of bytes read, 0 on error
size_t block_store_read(const block_store_t *const bs, const size_t block_id, void *buffer)
{
    // Check for invalid parameters
    if(bs == NULL || bs->blocks == NULL || buffer == NULL || block_id >= BLOCK_STORE_NUM_BLOCKS)
    {
        return 0;
    }

    // Copy the data from the specified block to the buffer
    memcpy(buffer, bs->blocks[block_id].Block, BLOCK_SIZE_BYTES);

    // Return the total number of bytes read (which is the block size)
    return BLOCK_SIZE_BYTES;
}


///
/// Reads data from the specified buffer and writes it to the designated block
/// \param bs BS device
/// \param block_id Destination block id
/// \param buffer Data buffer to read from
/// \return Number of bytes written, 0 on error
///
size_t block_store_write(block_store_t *const bs, const size_t block_id, const void *buffer)
{
    // Check for invalid parameters
    if(bs == NULL || bs->blocks == NULL || buffer == NULL || block_id >= BLOCK_STORE_NUM_BLOCKS)
    {
        return 0; //If invalid, return 0 for error
    }

    // Copy into buffer using memcpy
    memcpy(bs->blocks[block_id].Block, buffer, BLOCK_SIZE_BYTES);

    // return number bytes written
    return BLOCK_SIZE_BYTES;
}


///
/// Writes the entirety of the BS device to file, overwriting it if it exists - for grads/bonus
/// \param bs BS device
/// \param filename The file to write to
/// \return Number of bytes written, 0 on error
///
size_t block_store_serialize(const block_store_t *const bs, const char *const filename)
{
    // Check for incorrect params
    if(bs == NULL || filename == NULL)
    {
        return 0;
    }

    // Open the file for writing return int to file descriptor
    int fd = open(filename, O_WRONLY);

    // Check the return value of descriptor for error checking
    if (fd == -1) {
        perror("Error opening file");
        return 0;
    }

    // Write each block to the file
    size_t total_bytes_written = 0;

    // Iterate through writing
    for (size_t i = 0; i < BLOCK_STORE_NUM_BLOCKS; ++i) {
        ssize_t bytes_written = write(fd, bs->blocks[i].Block, BLOCK_SIZE_BYTES);

        // Check for return value writing to the file
        if (bytes_written == -1) {
            perror("Error writing to file");
            return 0;
        }

        // Cumulate the total bytes written
        total_bytes_written += bytes_written;
    }

    // Close the file, and check for error
    if (close(fd) == -1) {
        perror("Error closing file");
        return 0;
    }

    return total_bytes_written;
}


//OPTIONAL METHOD

///
/// Imports BS device from the given file - for grads/bonus
/// \param filename The file to load
/// \return Pointer to new BS device, NULL on error
///
block_store_t *block_store_deserialize(const char *const filename)
{
    UNUSED(filename);
    return NULL;
}