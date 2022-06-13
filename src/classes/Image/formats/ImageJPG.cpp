//
// Created by asd on 6.5.22.
//

#include "ImageJPG.h"
#include <string>
#include <stdexcept>

ImageJPG::ImageJPG(const std::string& path, std::shared_ptr<std::string> & default_ascii_gradient) : Image(path, default_ascii_gradient)
{
    _info.err = jpeg_std_error(&_err_mgr);

    // define a custom error handler
    _err_mgr.error_exit = [](j_common_ptr cinfo) {
        char msg[JMSG_LENGTH_MAX];
        (*(cinfo->err->format_message))(cinfo, msg);
        throw std::invalid_argument(msg);
    };

    jpeg_create_decompress(&_info);
    _info.out_color_space = JCS_EXT_RGB;
}

// ---------------------------------------------------------------------------------------------------------------------

ImageJPG::~ImageJPG()
{
    jpeg_destroy_decompress(&_info);
    if (_file) fclose(_file);
}

// ---------------------------------------------------------------------------------------------------------------------

bool ImageJPG::load()
{
    if (_file != nullptr) fclose(_file);
    _file = fopen(m_path.c_str(), "rb");

    if (!_file) throw std::invalid_argument("File cannot be opened or it does not exist.");

    jpeg_stdio_src(&_info, _file);

    jpeg_read_header(&_info, TRUE);

    jpeg_start_decompress(&_info);

    m_bmp_original.width = _info.output_width;
    m_bmp_original.height = _info.output_height;
    m_bmp_original.channels = _info.output_components;
    m_bmp_original.row_bytes = m_bmp_original.width * m_bmp_original.channels;

    // buffer just one row
    uint8_t* row_buffer[1];
    row_buffer[0] = new uint8_t[m_bmp_original.row_bytes];

    // read each scanline (row)
    JDIMENSION row;
    while (_info.output_scanline < m_bmp_original.height)
    {
        row = _info.output_scanline;

        // store the row pointer
        jpeg_read_scanlines(&_info, row_buffer, 1);

        // kinda sus
        // https://stackoverflow.com/a/261607/9421094
        m_bmp_original.bmp.push_back({});
        m_bmp_original[row].insert(
            m_bmp_original[row].end(),
            &row_buffer[0][0],
            &row_buffer[0][m_bmp_original.row_bytes]
        );
    }

    delete[] row_buffer[0];

    jpeg_finish_decompress(&_info);

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ImageJPG::get_gray_pixel(uint32_t y, uint32_t x, Bitmap<depth>& bmp) const
{
    // x * how many bytes it takes to store a single pixel = starting index of the pixel
    x *= m_bmp_original.channels;

    uint8_t R = truncate_uint8((int16_t) bmp[y][x] + m_adjustments.brightness);
    uint8_t G = truncate_uint8((int16_t) bmp[y][x + 1] + m_adjustments.brightness);
    uint8_t B = truncate_uint8((int16_t) bmp[y][x + 2] + m_adjustments.brightness);

    float factor = (259.0 * (m_adjustments.contrast + 255.0)) / (255.0 * (259.0 - m_adjustments.contrast));
    R = truncate_uint8((int16_t) (factor * (R - 128) + 128));
    G = truncate_uint8((int16_t) (factor * (G - 128) + 128));
    B = truncate_uint8((int16_t) (factor * (B - 128) + 128));
    
    if (m_adjustments.inverted)
    {
        R = 255 - R;
        G = 255 - G;
        B = 255 - B;
    }

    return 0.299 * R + 0.587 * G + 0.114 * B;
}

// ---------------------------------------------------------------------------------------------------------------------

int ImageJPG::get_xterm_pixel(uint32_t y, uint32_t x, Bitmap<depth> & bmp) const
{
    x *= m_bmp_original.channels;

    uint8_t R = truncate_uint8((int16_t) bmp[y][x] + m_adjustments.brightness);
    uint8_t G = truncate_uint8((int16_t) bmp[y][x + 1] + m_adjustments.brightness);
    uint8_t B = truncate_uint8((int16_t) bmp[y][x + 2] + m_adjustments.brightness);

    float factor = (259.0 * (m_adjustments.contrast + 255.0)) / (255.0 * (259.0 - m_adjustments.contrast));
    R = truncate_uint8((int16_t) (factor * (R - 128) + 128));
    G = truncate_uint8((int16_t) (factor * (G - 128) + 128));
    B = truncate_uint8((int16_t) (factor * (B - 128) + 128));

    if (m_adjustments.inverted)
    {
        R = 255 - R;
        G = 255 - G;
        B = 255 - B;
    }
    
    return Image::rgb_to_xterm(R, G, B);
}

// ---------------------------------------------------------------------------------------------------------------------

void ImageJPG::export_bmp(const std::string& filename, Bitmap<depth>& bmp) const
{
    struct jpeg_compress_struct info;
    struct jpeg_error_mgr err;

    FILE* fHandle;

    fHandle = fopen(filename.c_str(), "wb");

    info.err = jpeg_std_error(&err);
    jpeg_create_compress(&info);

    jpeg_stdio_dest(&info, fHandle);

    info.image_width = bmp.width;
    info.image_height = bmp.height;
    info.input_components = bmp.channels;
    info.in_color_space = JCS_RGB;

    jpeg_set_defaults(&info);
    jpeg_set_quality(&info, 100, TRUE);

    jpeg_start_compress(&info, TRUE);

    // Write every scanline...
    JSAMPROW row_buffer[1];
    while (info.next_scanline < info.image_height)
    {
        // see vector to pointer array https://stackoverflow.com/a/2923290/9421094
        row_buffer[0] = &bmp[info.next_scanline][0];
        jpeg_write_scanlines(&info, row_buffer, 1);
    }

    jpeg_finish_compress(&info);
    fclose(fHandle);

    jpeg_destroy_compress(&info);
}