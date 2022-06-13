//
// Created by asd on 7.6.22.
//

#ifndef ASCII_VECTOR_H
#define ASCII_VECTOR_H

#include <cstdint>

/**
 * Just a 2D vector that is used for resizing images.
 */
template<typename T>
struct Vector
{
    using unit = T;
    
    unit m_x;
    unit m_y;
    
    Vector(unit x, unit y) : m_x(x), m_y(y) {}
    Vector(uint16_t x, uint16_t y)  : m_x(x), m_y(y) {}

    Vector<unit>& operator+(const Vector& other)
    {
        m_x += other.m_x;
        m_y += other.m_y;
        return *this;
    }
    
    /** Scalar comparison - if both axis are greater than "value". */
    bool operator > (unit value)
    {
        return m_x > value && m_y > value;
    }
    /** Scalar comparison - if both axis are greater or equal than "value". */
    bool operator >= (unit value)
    {
        return m_x >= value && m_y >= value;
    }
    /** Scalar comparison - if both axis are less than "value". */
    bool operator < (unit value)
    {
        return m_x < value && m_y < value;
    }
    /** Scalar comparison - if both axis are less or equal than "value". */
    bool operator <= (unit value)
    {
        return m_x <= value && m_y <= value;
    }
    /** Scalar comparison - if both axis are equal to "value". */
    bool operator == (unit value)
    {
        return m_x == value && m_y == value;
    }
    /** Scalar comparison - if both axis are not equal to "value". */
    bool operator != (unit value)
    {
        return m_x != value && m_y != value;
    }
};


#endif //ASCII_VECTOR_H
