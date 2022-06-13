//
// Created by asd on 6.5.22.
//

#include "ImagePNG.h"
#include <string>
#include <stdexcept>
#include <algorithm>

ImagePNG::ImagePNG(const std::string & path, std::shared_ptr<std::string> & default_ascii_gradient) : Image(path, default_ascii_gradient)
{
    // define a custom error handler
    _struct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                     [](png_struct_def* def, const char* err_msg) {
                                         throw std::invalid_argument(err_msg);
                                     }, NULL);
    _info = png_create_info_struct(_struct);
}

// ---------------------------------------------------------------------------------------------------------------------

ImagePNG::~ImagePNG()
{
    png_destroy_read_struct(&_struct, &_info, NULL);
    if (_file) fclose(_file);
}

// ---------------------------------------------------------------------------------------------------------------------

bool ImagePNG::load()
{
    if (_file != nullptr) fclose(_file);
    _file = fopen(m_path.c_str(), "rb");

    if (!_file) throw std::invalid_argument("File cannot be opened or it does not exist.");

    png_init_io(_struct, _file);

    png_read_info(_struct, _info);

    m_bmp_original.width = png_get_image_width(_struct, _info);
    m_bmp_original.height = png_get_image_height(_struct, _info);
    png_byte color_type = png_get_color_type(_struct, _info);
    png_byte bit_depth = png_get_bit_depth(_struct, _info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if (bit_depth == 16)
        png_set_strip_16(_struct);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(_struct);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(_struct);

    if (png_get_valid(_struct, _info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(_struct);

    // these color_types don't have an alpha channel so fill it with 0xff.
    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(_struct, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(_struct);

    png_read_update_info(_struct, _info);

    m_bmp_original.bmp.reserve(m_bmp_original.height);

    m_bmp_original.row_bytes = png_get_rowbytes(_struct, _info);
    
    // reserve the space
    m_bmp_original.bmp = {m_bmp_original.height, Bitmap<depth>::matrix_row(m_bmp_original.row_bytes)};
    
    // see How to read PNG to std::vector https://stackoverflow.com/a/63172481/9421094
    std::vector<png_byte *> ppixels(m_bmp_original.height);
    std::transform(
        m_bmp_original.bmp.begin(),
        m_bmp_original.bmp.end(),
        ppixels.begin(),
        [](auto & vec){ return vec.data(); }
    );
    
    png_read_image(_struct, ppixels.data());

    m_bmp_original.channels = m_bmp_original.row_bytes / m_bmp_original.width;

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ImagePNG::get_gray_pixel(uint32_t y, uint32_t x, Bitmap<depth> & bmp) const
{
    // x * how many bytes it takes to store a single pixel = starting index of the pixel
    x *= m_bmp_original.channels;
            
    // the greater the alpha, the less transparent (more opaque) the pixel will be.
    // considering that the more transparent a pixel is, the less "significant" it is relative to more opaque pixels...
    // ...so it needs to be represented "darker" so it gets assigned a less "significant" ASCII character
    // 255 = 100% opaque (0% transparent), 0 = 0% opaque (100% transparent)
    double A = (double) bmp[y][x + 3] / 0xFF; // just as base10 => (percentage / 100) = one percent
    // 255 / 255 = 1 etc.

    uint8_t R = truncate_uint8((int16_t) bmp[y][x] + m_adjustments.brightness);
    uint8_t G = truncate_uint8((int16_t) bmp[y][x + 1] + m_adjustments.brightness);
    uint8_t B = truncate_uint8((int16_t) bmp[y][x + 2] + m_adjustments.brightness);
    
    float factor = (259.0 * (m_adjustments.contrast + 255.0)) / (255.0 * (259.0 - m_adjustments.contrast));
    R = truncate_uint8((int16_t) (factor * (R - 128) + 128)) * A;
    G = truncate_uint8((int16_t) (factor * (G - 128) + 128)) * A;
    B = truncate_uint8((int16_t) (factor * (B - 128) + 128)) * A;

    if (m_adjustments.inverted)
    {
        R = 255 - R;
        G = 255 - G;
        B = 255 - B;
    }

    return 0.299 * R +  0.587 * G + 0.114 * B;
}

// ---------------------------------------------------------------------------------------------------------------------

int ImagePNG::get_xterm_pixel(uint32_t y, uint32_t x, Bitmap<depth> & bmp) const
{
    x *= m_bmp_original.channels;
    double A = (double) bmp[y][x + 3] / 0xFF;

    uint8_t R = truncate_uint8((int16_t) bmp[y][x] + m_adjustments.brightness);
    uint8_t G = truncate_uint8((int16_t) bmp[y][x + 1] + m_adjustments.brightness);
    uint8_t B = truncate_uint8((int16_t) bmp[y][x + 2] + m_adjustments.brightness);

    float factor = (259.0 * (m_adjustments.contrast + 255.0)) / (255.0 * (259.0 - m_adjustments.contrast));
    R = truncate_uint8((int16_t) (factor * (R - 128) + 128)) * A;
    G = truncate_uint8((int16_t) (factor * (G - 128) + 128)) * A;
    B = truncate_uint8((int16_t) (factor * (B - 128) + 128)) * A;

    if (m_adjustments.inverted)
    {
        R = 255 - R;
        G = 255 - G;
        B = 255 - B;
    }

    return Image::rgb_to_xterm(R, G, B);
}

// ---------------------------------------------------------------------------------------------------------------------

void ImagePNG::export_bmp(const std::string & filename, Bitmap<depth> & bmp) const
{
    FILE *fp = fopen(filename.c_str(), "wb");
    if(!fp) abort();

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) abort();

    png_infop info = png_create_info_struct(png);
    if (!info) abort();

    if (setjmp(png_jmpbuf(png))) abort();

    png_init_io(png, fp);

    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
            png,
            info,
            bmp.width, bmp.height,
            8,
            PNG_COLOR_TYPE_RGBA,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    //png_set_filler(png, 0, PNG_FILLER_AFTER);

    // TODO: fix
//    png_write_image(png, bmp);
    png_write_end(png, NULL);

    fclose(fp);

    png_destroy_write_struct(&png, &info);
}