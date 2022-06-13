//
// Created by asd on 22.5.22.
//

#ifndef ASCII_IMAGEADJUSTMENTS_H
#define ASCII_IMAGEADJUSTMENTS_H

#include <cstdint>

/**
 * Class storing and managing all of the image's adjustments and modifications, such as brightness, contrast etc.
 */
class ImageAdjustments
{
public:
    ImageAdjustments();
    
    /**
     * Custom brightness of the image.
     * @note Value should be betwwen <-0xFF,+0xFF>
     */
    int16_t brightness = 0;
    /**
     * Custom contrast of the image.
     * @note Value should be betwwen <-0xFF,+0xFF>
     */
    int16_t contrast = 0;
    /**
     * Percentual vertical stretch of the image.
     * If the value is 50, then the image should be vertically stretched by 50%. 
     */
    int16_t stretch = 0;
    /**
     * If the colors of the ascii art are enabled or disabled.
     */
    bool colors_enabled = false;
    /**
     * Super ray-tracing high quality liquid retina HDR.
     */
    bool hdr = false;
    /**
     * If a "negative" is set.
     */
    bool inverted = false;
    
    ImageAdjustments & operator = (const ImageAdjustments & other);
    
    /**
     * Check if the given adjustments are the same as these.
     * @note adjustments that don't affect the ascii map generation or bitmap resizing are ignored
     *      (only colors_enabled atm.).
     * @param other image adjustments for comparison.
     */
    bool operator == (const ImageAdjustments & other);

    void change_brightness(int16_t value);

    void change_contrast(int16_t value);

    void change_invert(bool value);
    
    void change_stretch(int16_t value);
};


#endif //ASCII_IMAGEADJUSTMENTS_H
