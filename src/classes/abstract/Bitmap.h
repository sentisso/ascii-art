//
// Created by asd on 18.5.22.
//

#ifndef ASCII_BITMAP_H
#define ASCII_BITMAP_H

#include <memory>
#include <vector>
#include <string>
#include "Vector.h"
#include "../Image/ImageAdjustments.h"

/**
 * Class representing a bitmap of an Image. A grid/matrix of pixels with certain parameters such as:
 * it's width and height, number of color channels etc.\n
 * This class also provides a vector of "custom" strings (ascii map = amp) that is used for storing the actual ASCII-art.
 * 
 * @note An Image can have multiple Bitmap members (eg. the original image that serves as a reference, edited version, thumbnail etc.),
 *       the reason for this is that there's no need to resample, reapply any effects etc. on every windows resize, active image change and so on,
 *       because all of the modified versions of the image are stored in all of the individual bitmaps.
 */
template<typename T>
struct Bitmap
{
    using matrix_row = std::vector<T>;
    using matrix = std::vector<matrix_row>;
    
    ~Bitmap() {};
    
    /** The actual T-bit bitmap. */
    matrix bmp;
    
    /** How many pixels are in a single row. */
    uint32_t width = 0;
    /** How many rows this image consists of (= the height of the image). */
    uint32_t height = 0;
    
    /**
     * What were the last "bounds" that this image was downsampled to fit into.
     * @example max_width was 500 and max_height was 500 and the image was downsampled to 500x300,
     *          so bounds_y and x will be both 500. 
     */
    uint32_t bounds_y = 0;
    uint32_t bounds_x = 0;

    /** How many color channels this bitmap has (= how many bytes it takes to store a single pixel). */
    uint8_t channels = 0;
    /** How many bytes it takes to store a whole row of pixels (pixels per row * pixel_size). */
    uint32_t row_bytes = 0;

    /**
     * "Current" ASCII representation of this bitmap (asciimap).
     * std::pair<char, int> = {the ascii char, xterm color of this char}
     */
    std::vector<std::vector<std::pair<char, int16_t>>> amp;
    /** O(1) look up of how many times each character is used in the amp. */
    uint32_t amp_used_chars[256] = {};
    
    /**
     * What adjustments are currenlty applied to this bitmap.
     * These can differ from the bitmap's image.
     */
    ImageAdjustments adjustments;
    
    /**
     * Adjust (or "recalculate") this bitmap's attributes based on the given src and new sample ratio,
     * such as set the new width, height, row_bytes etc. 
     * @param sample_ratio the new sample ratio that this bitmap is being adjusted by.
     * @param src 
     */
    void adjust(const Vector<double> & sample_ratio, Bitmap & src)
    {
        *this = src;

        width /= sample_ratio.m_x;
        height /= sample_ratio.m_y;
        row_bytes = width * channels;
    }
    
    /**
     * Make sure that there's space for another row of pixels at the given row of this bitmap.
     * @param row y index of the row that is suppose to "exist" and have space for row_bytes.
     */
    void reserve(uint32_t row)
    {
        // make sure there's vertical and horizontal space
        if (row >= bmp.size())
            bmp.emplace_back(row_bytes);
            // make sure there's vertical space
        else
            bmp[row].reserve(row_bytes);
    }
    
    /**
     * Return the bitmap row at the given index.
     * @param y row index.
     */
    matrix_row & operator [] (uint32_t y)
    {
        return bmp[y];
    }
    
    /**
     * Copy the another bitmap's properties (except the bmp pointer).
     */
    Bitmap<T> & operator = (const Bitmap & other)
    {
        width = other.width;
        height = other.height;
        channels = other.channels;
        row_bytes = other.row_bytes;

        return *this;
    }
};


#endif //ASCII_BITMAP_H
