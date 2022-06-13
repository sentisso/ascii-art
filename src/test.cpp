#include <iostream>
#include <assert.h>
#include <filesystem>

#include "classes/Image/Image.h"
namespace fs = std::filesystem;

std::shared_ptr<std::string> ascii_gradient(new std::string("$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. "));

/**
 * Test whether or not the given image can be opened, read and processed.
 * @param path path to the image.
 * @return true if the image can be opened and processed, false if an exception occured.
 */
bool test_image(std::string path)
{
    try
    {
        std::shared_ptr<Image> img = Image::get_image(path, ascii_gradient);
        Vector ratio(1.1633, 0.9755);
        img->sample_bilinear_interpolation(ratio, img->m_bmp_edited, img->m_bmp_original);
        ratio.m_x = 1;
        ratio.m_y = 1.3338987;
        img->sample_bilinear_interpolation(ratio, img->m_bmp_edited, img->m_bmp_original);
//        img->export_bmp(path + "2", img->m_bmp_edited);
        return true;
    } catch (std::invalid_argument& e)
    {
        std::cout << "[" << path << "]: " << e.what() << std::endl;
        return false;
    }
}

void test_images()
{
    std::shared_ptr<Image> img;

    for (const auto & entry : fs::directory_iterator("./assets/tests/corrupted/png/"))
        assert(!test_image(entry.path()));
    
    for (const auto & entry : fs::directory_iterator("./assets/tests/ok/png/"))
        assert(test_image(entry.path()));

    for (const auto & entry : fs::directory_iterator("./assets/tests/corrupted/jpg/"))
        assert(!test_image(entry.path()));
    
    for (const auto & entry : fs::directory_iterator("./assets/tests/ok/jpg/"))
        assert(test_image(entry.path()));
}

int main(void)
{
    test_images();

    return EXIT_SUCCESS;
}