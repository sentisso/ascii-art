//
// Created by asd on 22.5.22.
//

#include "ImageAdjustments.h"

ImageAdjustments::ImageAdjustments()
{}

// ---------------------------------------------------------------------------------------------------------------------

ImageAdjustments & ImageAdjustments::operator=(const ImageAdjustments& other)
{
    brightness = other.brightness;
    contrast = other.contrast;
    inverted = other.inverted;
    stretch = other.stretch;
    
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

bool ImageAdjustments::operator == (const ImageAdjustments& other)
{
    if (brightness != other.brightness) return false;
    if (contrast != other.contrast) return false;
    if (inverted != other.inverted) return false;
    if (stretch != other.stretch) return false;
    
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void ImageAdjustments::change_brightness(int16_t value)
{
    if (value >= -256 && value <= 256)
    {
        brightness = value;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ImageAdjustments::change_contrast(int16_t value)
{
    if (value >= -256 && value <= 256)
    {
        contrast = value;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ImageAdjustments::change_invert(bool value)
{
    inverted = value;
}

// ---------------------------------------------------------------------------------------------------------------------

void ImageAdjustments::change_stretch(int16_t value)
{
    if (value >= -516 && value <= 516)
        stretch = value;
}