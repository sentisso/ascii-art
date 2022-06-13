//
// Created by asd on 6.5.22.
//

#ifndef ASCII_IMAGEJPG_H
#define ASCII_IMAGEJPG_H

#include "../Image.h"
#include <cstring>
#include <jpeglib.h>

/**
 * Class handling JPG formats.\n
 * One pixel always contains 3 bytes => RGB.
 */
class ImageJPG : public Image
{
    jpeg_decompress_struct _info;
    jpeg_error_mgr _err_mgr;

public:
    ImageJPG(const std::string & path, std::shared_ptr<std::string> & default_ascii_gradient);
    ~ImageJPG();

    /**
     * Load the JPG image data from the associated image file in `_path` to a unified bitmap in the RGB format.
     *
     * @see References - https://gist.github.com/PhirePhly/3080633
     * @throws std::invalid_argument If a library error occurs, then an invalid_argument is thrown.
     *                               The error message will be available in the `what()` method of the exception.
     * @returns true = the associated image file is valid and can be parsed.
     */
    bool load() override;

    uint8_t get_gray_pixel(uint32_t y, uint32_t x, Bitmap<depth> & bmp) const override;

    int get_xterm_pixel(uint32_t y, uint32_t x, Bitmap<depth> & bmp) const override;

    void export_bmp(const std::string & filename, Bitmap<depth> & bmp) const override;
};


#endif //ASCII_IMAGEJPG_H
