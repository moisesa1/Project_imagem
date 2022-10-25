#include <iostream>
#include <fstream>



using namespace std;



struct Pixel
{
    unsigned char red, green, blue, alpha;
};

// Convert Image to Gray at CPU
void ConvertImageToGrayCpu(unsigned char* imageRGBA, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            /*  Color to Grayscale Equation
             Ylinear = 0.2126RLinear + 0.7152GLiners +0.0722BLiners */
            Pixel* ptrPixel = (Pixel*)&imageRGBA[y * width * 4 + 4 * x];
            unsigned char pixelValue = (unsigned char)(ptrPixel->red * 0.2126f + ptrPixel->green * 0.7152f + ptrPixel->blue * 0.0722f);

            //float pixelValue = ptrPixel->red * 0.2126f + ptrPixel->green * 0.7152f + ptrPixel-> blue * 0.0722f;
            //unsigned char pixelValue = pixelValue;
            ptrPixel->red = pixelValue;
            ptrPixel->green = pixelValue;
            ptrPixel->blue = pixelValue;
            ptrPixel->alpha = 255;

        }
    }

}
    