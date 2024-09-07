//
// Created by asd on 25.4.22.
//

#include <iostream>
#include <algorithm>
#include <math.h>
#include "Image.h"
#include "./formats/ImageJPG.h"
#include "./formats/ImagePNG.h"
#include <ncurses.h>
#include <filesystem>

#define v2ci(v) (v < 48 ? 0 : v < 115 ? 1 : (v - 35) / 40)
#define color_index() (36 * ir + 6 * ig + ib)  /* 0..215, lazy evaluation */
#define dist_square(A, B, C, a, b, c) ((A-a)*(A-a) + (B-b)*(B-b) + (C-c)*(C-c))

Image::Image(const std::string & path, std::shared_ptr<std::string> & default_ascii_gradient) : m_ascii_gradient(default_ascii_gradient), m_path(path)
{}

// ---------------------------------------------------------------------------------------------------------------------

void Image::render(window & win)
{
    wclear(win.win);

    Bitmap<depth>& bmp = m_bmp_edited;

    // if the ascii map already exists then that means it was previously rendered, so don't animate it
    bool animate = bmp.amp.size() == 0 ? true : false;

    to_ascii(win.x, win.y, bmp);

    // image adjustments were applied to the bitmap
    bmp.adjustments = m_adjustments;

    uint16_t abs_x = bmp.width < win.x ? win.mid_x() - round(bmp.width / 2) : 0;

    if (animate)
    {
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 1000000L * 10; // 10ms

        // for each available character in the ascii gradient...
        for (size_t i = 0; i < m_ascii_gradient->size(); ++i)
        {
            // how many times this character is used in this amp
            uint32_t char_count = bmp.amp_used_chars[(int) m_ascii_gradient->at(i)];

            // if this character is used in this ascii map
            if (char_count > 0)
            {
                for (uint32_t y = 0; y < bmp.height; ++y)
				{
					for (size_t x = 0; x < bmp.width; ++x)
					{
						if (bmp.amp[y][x].first != m_ascii_gradient->at(i)) continue;

						// if the character has a color...
						if (bmp.amp[y][x].second != -1 && m_adjustments.colors_enabled)
						{
							if (m_adjustments.hdr)
								wattron(win.win, COLOR_PAIR(bmp.amp[y][x].second) | A_REVERSE);
							else
								wattron(win.win, COLOR_PAIR(bmp.amp[y][x].second));
						}

						mvwaddch(win.win, y, x + abs_x, bmp.amp[y][x].first);

						if (bmp.amp[y][x].second != -1 && m_adjustments.colors_enabled)
						{
							if (m_adjustments.hdr)
								wattroff(win.win, COLOR_PAIR(bmp.amp[y][x].second) | A_REVERSE);
							else
								wattroff(win.win, COLOR_PAIR(bmp.amp[y][x].second));
						}

						// if there are no characters of the same type left...
						if (--char_count == 0)
						{
							// ...then there's no need to iterate through the amp and look for it anymore
							y = bmp.height;
							break;
						}
					}
				}

                wrefresh(win.win);
                nanosleep(&ts, NULL);
                // creates an easing effect
                ts.tv_nsec += 50000L; // + 0.5ms
            }
        }
    }
    else
    {
        for (uint32_t y = 0; y < bmp.height; ++y)
            for (uint32_t x = 0; x < bmp.width; ++x)
            {
                // if the character has a color...
                if (bmp.amp[y][x].second != -1 && m_adjustments.colors_enabled)
                {
                    if (m_adjustments.hdr)
                        wattron(win.win, COLOR_PAIR(bmp.amp[y][x].second) | A_REVERSE);
                    else
                        wattron(win.win, COLOR_PAIR(bmp.amp[y][x].second));
                }

                mvwaddch(win.win, y, x + abs_x, bmp.amp[y][x].first);

                if (bmp.amp[y][x].second != -1 && m_adjustments.colors_enabled)
                {
                    if (m_adjustments.hdr)
                        wattroff(win.win, COLOR_PAIR(bmp.amp[y][x].second) | A_REVERSE);
                    else
                        wattroff(win.win, COLOR_PAIR(bmp.amp[y][x].second));
                }
            }

        wrefresh(win.win);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Image::to_ascii(uint16_t max_width, uint16_t max_height, Bitmap<depth> & bmp)
{
    // if the bounds didn't change and the adjustments were applied, then don't convert it
    if (bmp.bounds_y == max_height && bmp.bounds_x == max_width && bmp.adjustments == m_adjustments) return;

    shrink_to_fit(max_width, max_height, bmp);

    uint8_t gray;
    for (uint32_t y = 0; y < bmp.height; ++y)
    {
        // if there's no room left to store the Y axis...
        if (bmp.amp.size() == y)
            bmp.amp.push_back({}); // ...then make some lmao

        // for each pixel (character)
        for (uint32_t x = 0; x < bmp.width; ++x)
        {
            gray = get_gray_pixel(y, x, bmp);

            // most left character is the brightest in the ascii gradient
            // but grayscale black is 0, therefore 0 != index of the darkest character
            char c = m_ascii_gradient->at(m_ascii_gradient->size() - 1 - (int) round(gray * m_ascii_gradient->size() / 256));

            bmp.amp_used_chars[(int) c]++; // register the used char

            if (bmp.amp[y].size() == x)
                bmp.amp[y].push_back({c, get_xterm_pixel(y, x, bmp)});
            else
                bmp.amp[y][x] = {c, get_xterm_pixel(y, x, bmp)};
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Image::shrink_to_fit(uint16_t max_width, uint16_t max_height, Bitmap<depth> & dst)
{
    // if the bounds or the stretch didn't change, then don't shrink...
    if (dst.bounds_y == max_height && dst.bounds_x == max_width && dst.adjustments.stretch == m_adjustments.stretch)
        return;

    Bitmap<depth> * src = &dst;

    // if the given bitmap is suppose to be "upsampled" (the given bounds are greater than the last ones)
    if (dst.bounds_y != 0 && dst.bounds_x != 0 && (dst.bounds_y < max_height || dst.bounds_x < max_width))
        src = &m_bmp_original;

    if (dst.adjustments.stretch != m_adjustments.stretch || dst.bmp.size() == 0)
        src = &m_bmp_original;

    dst.bounds_x = max_width;
    dst.bounds_y = max_height;

    Vector sample_ratio = get_downsample_ratio(max_width, max_height, *src);

    // if there's no need to downsample, then just don't...
    if (sample_ratio == 1) return;

    if (sample_ratio >= 2)
    {
        downsample_grid_average(std::min(sample_ratio.m_x, sample_ratio.m_y), dst, *src);

        // either way the src now needs to be the dst itself, cause it already got downsampled (no need for the original)
        src = &dst;

        // downsample_grid_average only works with a whole number as the ratio, therefore the original downsample ratio
        // needs to be recalculated
        sample_ratio = get_downsample_ratio(max_width, max_height, dst);

        if (sample_ratio.m_x < 1) sample_ratio.m_x = 1;
        if (sample_ratio.m_y < 1) sample_ratio.m_y = 1;
    }

    if (sample_ratio >= 1)
        sample_bilinear_interpolation(sample_ratio, dst, *src);
}

// ---------------------------------------------------------------------------------------------------------------------

void Image::downsample_grid_average(uint16_t downsample_ratio, Bitmap<depth> & dst, Bitmap<depth> & src)
{
    dst.adjust({downsample_ratio, downsample_ratio}, src);

    uint32_t grid_size = downsample_ratio * downsample_ratio;

    for (uint32_t i = 0; i < dst.height; ++i)
    {
        dst.reserve(i);

        uint32_t y = i * downsample_ratio;

        for (uint32_t j = 0; j < dst.row_bytes; j += dst.channels)
        {
            uint32_t x = j * downsample_ratio;

            // for each color channel (RGB or RGBA etc.)
            for (uint8_t ch = 0; ch < dst.channels; ++ch)
            {
                uint32_t avg = 0;

                // loop through the downsample grid (gridy x gridx)
                for (uint16_t gy = 0; gy < downsample_ratio; ++gy)
                    for (uint16_t gx = 0; gx < downsample_ratio * dst.channels; gx += dst.channels)
                        avg += src[y + gy][x + gx];

                x++; // next color channel

                dst[i][j + ch] = avg / grid_size;
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Image::sample_bilinear_interpolation(Vector<double> & sample_ratio, Bitmap<depth> & dst, Bitmap<depth> & src)
{
    dst.adjust(sample_ratio, src);

    // i = y of the dst
    for (uint32_t i = 0; i < dst.height; ++i)
    {
        dst.reserve(i);

        // y = y of the src
        double y = i * sample_ratio.m_y;

        uint32_t y_f = floor(y);
        uint32_t y_c = ceil(y);

        if (y_c >= src.height)
            y_c = y_f;

        // which weights should the neighbouring pixels have (y_c_w = y_ceil_weight, y_f_w = y_floor_weight)
        // example: we need an average of the "1.7th" pixel, so the 1st pixel will have a weight of 0.3...
        //          and the 2nd pixel will have a weight of 0.7, because it's closer to the 1.7
        double y_c_w = y_c - y;
        double y_f_w = y - y_f;

        // j = x of the dst
        for (uint32_t j = 0; j < dst.width; ++j)
        {
            // x = x of the src
            double x = j * sample_ratio.m_x;

            uint32_t x_f = floor(x) * dst.channels;
            uint32_t x_c = ceil(x) * dst.channels;

            if (x_c >= src.row_bytes)
                x_c = x_f;

            double x_c_w = ceil(x) - x;
            double x_f_w  = x - floor(x);

            // for each color channel (RGB or RGBA etc.)
            for (uint8_t ch = 0; ch < dst.channels; ++ch)
            {
                uint8_t avg;

                // if we're between four pixels on the vertical and the horizontal axis
                if (x_f != x_c && y_f != y_c)
                {
                    uint8_t v1 = src[y_f][x_f];
                    uint8_t v2 = src[y_f][x_c];
                    uint8_t v3 = src[y_c][x_f];
                    uint8_t v4 = src[y_c][x_c];

                    double q1 = v1 * x_c_w + v2 * x_f_w;
                    double q2 = v3 * x_c_w + v4 * x_f_w;
                    avg = q1 * y_c_w + q2 * y_f_w;
                }
                // if we're between two pixels on the horizontal axis (y_f == y_c)
                else if (x_f != x_c)
                    avg = src[y_f][x_f] * x_c_w + src[y_f][x_c] * x_f_w;

                // if we're between two pixels on the vertical axis (x_f == x_c)
                else if (y_f != y_c)
                    avg = src[y_f][x_f] * y_c_w + src[y_c][x_f] * y_f_w;

                // if we're not between two pixels
                else
                    avg = src[y_f][x_f];

                dst[i][j * dst.channels + ch] = avg;

                // increase the index for the next color channel
                x_f++;
                x_c++;
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Vector<double> Image::get_downsample_ratio(uint16_t max_width, uint16_t max_height, Bitmap<depth> & bmp)
{
    // if the given bitmap hasn't been malloced yet, then reference the original bitmap
    uint32_t width = bmp.bmp.size() == 0 ? m_bmp_original.width : bmp.width;
    uint32_t height = bmp.bmp.size() == 0 ? m_bmp_original.height : bmp.height;

    double stretch = 1;

    if (bmp.adjustments.stretch != m_adjustments.stretch)
        stretch = 1 + (double) m_adjustments.stretch / 100;

    width *= stretch;

    // if "there's room" for the image (even with the stretch), then shrink the height instead
    if (width <= max_width && height <= max_height) return {1, stretch};

    double wratio = (double) width / max_width;
    double hratio = (double) height / max_height;

    double ratio = wratio > hratio ? wratio : hratio;

    return {ratio / stretch, ratio};
}

// ---------------------------------------------------------------------------------------------------------------------

std::shared_ptr<Image> Image::get_image(const std::string& path, std::shared_ptr<std::string> & default_ascii_gradient)
{
    if (!std::filesystem::exists(path))
        throw std::invalid_argument("File does not exist.");

    image_type ext = Image::get_extension(path);

    std::shared_ptr<Image> img(nullptr);
    switch (ext)
    {
        case jpg:
            img.reset(new ImageJPG(path, default_ascii_gradient));
            if (img->load()) return img;
            throw std::invalid_argument("Not a valid JPG.");

        case png:
            img.reset(new ImagePNG(path, default_ascii_gradient));
            if (img->load()) return img;
            throw std::invalid_argument("Not a valid PNG.");

        default:
            throw std::invalid_argument("Unsupported image type.");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

int Image::rgb_to_xterm(uint8_t r, uint8_t g, uint8_t b)
{
    if (COLORS == 256)
    {
        // Calculate the nearest 0-based color index at 16 .. 231
        int ir = v2ci(r), ig = v2ci(g), ib = v2ci(b);   // 0..5 each

        // Calculate the nearest 0-based gray index at 232 .. 255
        int average = (r + g + b) / 3;
        int gray_index = average > 238 ? 23 : (average - 3) / 10;  // 0..23

        // Calculate the represented colors back from the index
        static const int i2cv[6] = {0, 0x5f, 0x87, 0xaf, 0xd7, 0xff};
        int cr = i2cv[ir], cg = i2cv[ig], cb = i2cv[ib];  // r/g/b, 0..255 each
        int gv = 8 + 10 * gray_index;  // same value for r/g/b, 0..255

        // Return the one which is nearer to the original input rgb value
        int color_err = dist_square(cr, cg, cb, r, g, b);
        int gray_err = dist_square(gv, gv, gv, r, g, b);

        return color_err <= gray_err ? 16 + color_index() : 232 + gray_index;
    }

    // else the default color
    return -1;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Image::truncate_uint8(int16_t value)
{
    if (value < 0) return 0;
    else if (value > 0xFF) return 0xFF;

    return value;
}

// ---------------------------------------------------------------------------------------------------------------------

image_type Image::get_extension(const std::string& path)
{
    size_t pos = path.rfind('.');
    if (pos == std::string::npos)
        return none;

    std::string ext = path.substr(pos + 1);
    // ext to lower
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });

    if (ext == "jpg" || ext == "jpeg") return jpg;
    else if (ext == "png") return png;

    return none;
}