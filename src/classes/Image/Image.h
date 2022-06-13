#ifndef BI_PA2_IMAGE_H
#define BI_PA2_IMAGE_H

#include <string>
#include "../../enums.h"
#include "../abstract/Bitmap.h"
#include "ImageAdjustments.h"
#include "../Windows.h"
#include "../abstract/Vector.h"

/**
 * Class representing an image in an "abstract" form.
 * Children of this class (JPG, PNG...) differ, for example, in reading an image file of their own format, calculating a grayscale value of a pixel the correct way etc.
 */
class Image
{
protected:
    FILE* _file = nullptr;

public:
    using depth = uint8_t;
    
    /**
     * Pointer to the definition of the pixel-to-ascii character gradient.\n
     * This can either be the default ascii gradient or the Image can have it's own gradient defined.
     */
    std::shared_ptr<std::string> m_ascii_gradient;
    /** Bitmap of the original image that got originally uploaded. */
    Bitmap<uint8_t> m_bmp_original;
    /** Bitmap of an edited/downsampled/whatever'd version of the original image. */
    Bitmap<uint8_t> m_bmp_edited;
    /** Bitmap of the little thumbnail that gets rendered at the bottom of the screen. */
    Bitmap<uint8_t> m_bmp_thumbnail;

    /** Current adjustments that this image has. */
    ImageAdjustments m_adjustments;

    /** Path to the source image that the user defined when uploading. */
    std::string m_path;

    Image(const std::string& path, std::shared_ptr<std::string> & default_ascii_gradient);

    virtual ~Image() {}
    
    /**
     * Render this image's ascii map into the given window.
     * The width & height of this window will be used as the boundaries for shrink_to_fit of the image.\n
     * @param win reference to the window, where the image should be rendered to.
     */
    void render(window & win);

    /**
     * Convert the image's bitmap to it's ASCII representation to the specified "ascii map".\n
     * If the last max resolution is same as the specified one (max_width and max_height), then nothing gets converted
     *
     * @see ASCII GRAY SCALE CHARTS - http://mewbies.com/geek_fun_files/ascii/ascii_art_light_scale_and_gray_scale_chart.htm\n
     *      Character representation of grey scale images - http://paulbourke.net/dataformats/asciiart/
     * @param max_width Max allowed width of the ASCII image.
     * @param max_height Max allowed height of the ASCII image.
     * @param bmp Which bitmap should be represented as an ASCII-art. This given bitmap should be the member of this class.
     */
    void to_ascii(uint16_t max_width, uint16_t max_height, Bitmap<depth>& bmp);

    /**
     * Downsample the image's bitmap to fit into a given resolution using bilinear interpolation.\n
     * If the resolution is lower than the given one, nothing happens.
     * @see Understanding Bilinear Image Resizing - https://chao-ji.github.io/jekyll/update/2018/07/19/BilinearResize.html
     * @param max_width Maximal width the image should fit into.
     * @param max_height Maximal height the image should fit into.
     * @param dst Where should the shrinked image be stored.
     */
    void shrink_to_fit(uint16_t max_width, uint16_t max_height, Bitmap<depth>& dst);

    /**
     * Calcualete how big of a grid of pixels has to be downsampled to a single pixel in order to fit the given resolution.
     * @example If the image has a resolution of 500x500 and a given max is 200x200, then 2.5 will be returned.\n
     *          In order to fit 500 into 200, it needs to be scaled down by 2.5 (2.5 -> 1x1).\n
     *          500x500 / 2.5 = 200x200
     * @param max_width Maximal width the image should fit into.
     * @param max_height Maximal height the image should fit into.
     * @param bmp Reference to the bitmap whose resolution will be measured. This given bitmap should be the member of this class.
     *            (if this bitmap hasn't been initialized yet, then the m_bmp_original will be used as the reference).
     * @returns If the image already fits the given resolution, then 1.0 is returned.
     */
    Vector<double> get_downsample_ratio(uint16_t max_width, uint16_t max_height, Bitmap<depth>& bmp);

public:
    /**
     * Downsample the image's bitmap with an integer value, therefore this doesn't use weighted averages just as
     * bilinear interpolation does.\n
     * @example The downsample_ratio is set to 2, this means that every 2x2 grid in the original image will be averaged into a single pixel.\n
     *          single pixel = ([y][x] + [y][x+1] + [y+1][x] + [y+1][x+1]) / 4
     * @param downsample_ratio unsinged int ratio by which the image should by downsampled.
     * @param dst Where the result (the downsampled bitmap) should be stored.
     * @param src Which bitmap should the destination refer to when downsampling and calculating averages etc.
     *            The source can also be the destination itself.
     */
    void downsample_grid_average(uint16_t downsample_ratio, Bitmap<depth>& dst, Bitmap<depth>& src);

    /**
     * Sample the image's bitmap by a non-integer value using bilinear interpolation.\n
     * Not named downsample_... because it technically supports upsampling, but it causes some funny business.
     * @see inspiration - https://meghal-darji.medium.com/implementing-bilinear-interpolation-for-image-resizing-357cbb2c2722
     * @example 1. We want to know the (averaged) value of a pixel with the coordinates of [13][5] with a downsample ratio of 1,384615385.\n
     *          2. To get the "reference point" in the original image that should be averaged down into the single [13][5] pixel,\n
     *          we gotta multiply the coordinates by the downsample ratio (downsampled[13][5] * d_r = original[20][6,923076925]).\n
     *          3. The reference point is between 2 pixels on the horizontal axis, so to calculate the correct average value we gotta use linear interpolation:\n
     *          value of downsampled[13][5] is original[20][6] * (1 - 0,923076925) + original[20][7] * 0,923076925\n
     *          \<= original[20][6] is farther away from the reference point (see the float value), therefore it's value is less significant
     *          and needs to be multiplied by the "greater distance value", on the other hand original[20][7] is more significant
     *          and needs to be multiplied by the "closer distance value".\n
     *          4. The same algorithm applies for the vertical axis and for both of the axis in the same time (bilinear interpolation).
     * @param sample_ratio 2D vector that serves as a "map": dst -> src.\n
     *        If the value is outside of the interval (0, 2.0), then the results may be sub-optimal.
     * @param dst Where the result (the resampled bitmap) should be stored.
     * @param src Which bitmap should the destination refer to when resampling and calculating averages etc.
     *            The source can also be the destination itself.
     */
    void sample_bilinear_interpolation(Vector<double> & sample_ratio, Bitmap<depth>& dst, Bitmap<depth>& src);

    /**
     * Load the image data from the associated image file in `_path` to a unified bitmap.
     * @throws std::invalid_argument If a library error occurs, then an invalid_argument is thrown.
     *                               The error message will be available in the `what()` method of the exception.
     * @returns true = the associated image file is valid and can be parsed.
     */
    virtual bool load() = 0;

    /**
     * Calculate the grayscale value of the specified pixel and return it. This is later used for generating the ASCII art.
     * @see Grayscale to RGB Conversion - https://www.tutorialspoint.com/dip/grayscale_to_rgb_conversion.htm.
     * @param y Index of the row of the image.
     * @param x Index of the pixel of the image (not byte, pixel).
     * @param buffered If the grayscale pixel should be returned from the buffered bitmap.
     * @param bmp Which bitmap should be used for calculating it's grayscale pixel.
     * @return Grayscale value of the specified pixel (0-255).
     * @throws SEGFAULT logically heap-buffer-overflow if the specified y and x are outside of the m_bmp size lmao.
     */
    virtual uint8_t get_gray_pixel(uint32_t y, uint32_t x, Bitmap<depth>& bmp) const = 0;

    /**
     * Calculate and return an index of an xterm color that is closest to the specified pixel.
     * @see Xterm colors and their indexes - https://www.ditig.com/256-colors-cheat-sheet.
     * @param y Index of the row of the image.
     * @param x Index of the pixel of the image (not byte, pixel).
     * @param bmp Which bitmap should be used for calculating it's xterm pixel.
     * @return index of the closest Xterm color
     */
    virtual int get_xterm_pixel(uint32_t y, uint32_t x, Bitmap<depth>& bmp) const = 0;

    /**
     * Export the bitmap into a file in the associated image format. Useful for debugging purposes.
     * @param filename Path to the exported image.
     * @param bmp Which bitmap should be exported.
     */
    virtual void export_bmp(const std::string& filename, Bitmap<depth>& bmp) const = 0;

    /**
     * Determine what supported image the given file is by the filename extension and try to parse it as that image.\n
     * If the parsing succeedes then a new (format specific) Image gets allocated which can be later used for adjustments and ASCII-art generation.
     * The parsing heavily relies on 3rd party libraries (such is libjpeg.h and png.h).
     * @param path String path to the image file in the FS.
     * @param default_ascii_gradient pointer to the default ascii gradient.
     * @throws std::invalid_argument If a library error occurs, then an invalid_argument is thrown.\n
     *                               The error message will be available in the `what()` method of the exception.
     * @returns shared_ptr<Image*> if the parsing was successful, else an exception is thrown.
     */
    static std::shared_ptr<Image> get_image(const std::string& path, std::shared_ptr<std::string> & default_ascii_gradient);

    /**
     * Convert RGB24 to xterm-256 8-bit value.
     * For simplicity, assume RGB space is perceptually uniform.\n
     * There are 5 places where one of two outputs needs to be chosen when the input is the exact middle:\n
     * - The r/g/b channels and the gray value: the higher value output is chosen.\n
     * - If the gray and color have same distance from the input - color is chosen.
     * @see source - https://stackoverflow.com/a/41978310/9421094
     * @return index of the Xterm color (https://www.ditig.com/256-colors-cheat-sheet)
     */
    static int rgb_to_xterm(uint8_t r, uint8_t g, uint8_t b);

    /**
     * Adjust an 8bit value (frequently a single channel of a pixel) by adding an int and then truncate the value to uint8.
     * Used when applying brightness or contrast etc. to a pixel. 
     * 
     * @param value int16_t value that will be adjusted to uint8_t.
     * @return new adjusted uint8_t value.
     */
    static uint8_t truncate_uint8(int16_t value);

    /**
     * Try to parse the filename extension of a given path and return a corresponding image_type.
     * @param path Path to the image file.
     * @return jpg, png, none...
     */
    static image_type get_extension(const std::string& path);
};


#endif //BI_PA2_IMAGE_H
