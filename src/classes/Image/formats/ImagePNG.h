//
// Created by asd on 6.5.22.
//

#ifndef ASCII_IMAGEPNG_H
#define ASCII_IMAGEPNG_H

#include "../Image.h"
#include <png.h>

/**
 * Class handling PNG formats.\n
 * One pixel always contains 4 bytes => RGBA.
 */
class ImagePNG : public Image
{
    png_infop _info;
    png_structp _struct;

public:
    ImagePNG(const std::string & path, std::shared_ptr<std::string> & default_ascii_gradient);
    ~ImagePNG();

    /**
     * Load the PNG image data from the associated image file in `_path` to a unified bitmap in the RGBA format.
     *
     * @see References - https://gist.github.com/niw/5963798, https://android.googlesource.com/platform/external/libpng/+/eee63db00955234e51954839d2dd47db529d05ce/example.c
     * @throws std::invalid_argument If a library error occurs, then an invalid_argument is thrown.
     *                               The error message will be available in the `what()` method of the exception.
     * @returns true = the associated image file is valid and can be parsed.
     */
    bool load() override;

    uint8_t get_gray_pixel(uint32_t y, uint32_t x, Bitmap<depth> & bmp) const override;

    int get_xterm_pixel(uint32_t y, uint32_t x, Bitmap<depth> & bmp) const override;

    void export_bmp(const std::string & filename, Bitmap<depth> & bmp) const override;
};


#endif //ASCII_IMAGEPNG_H
