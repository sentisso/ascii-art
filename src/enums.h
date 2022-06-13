//
// Created by asd on 6.5.22.
//

#ifndef ASCII_ENUMS_H
#define ASCII_ENUMS_H

/**
 * All supported image formats including errors (error_*).
 * none = no errors, but type isn't jpg nor png nor anything else.
 */
enum image_type {
    jpg, png, none
};

/**
 * All supported image adjustments.
 */
enum image_adjustments {
    brightness, contrast, inverted, stretch
};

#endif //ASCII_ENUMS_H
