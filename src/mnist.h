#include <fstream>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
typedef unsigned char uint8_t;


void read_mnist_images(const char *filename, vector<vector<uint8_t> > &images, int n)
{
    std::ifstream file (filename, std::ios::binary);
    if (!file.is_open()) {
        cout << "Can't open mnist file." << endl;
        return;
    }

    file.seekg(16);
    for(int i = 0; i < n; ++i) {
        vector<uint8_t> image;
        for(int r = 0; r < 28; ++r) {
            for(int c = 0; c < 28; ++c) {
                uint8_t pixel;
                file.read(reinterpret_cast<char *>(&pixel), sizeof(pixel));
                image.push_back(pixel);
            }
        }
        images.push_back(image);
    }
}


void read_mnist_labels(const char *filename, vector<uint8_t> &labels, int n)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        cout << "Can't open mnist file." << endl;
        return;
    }

    file.seekg(8);
    for(int i = 0; i < n; ++i) {
        uint8_t label;
        file.read(reinterpret_cast<char *>(&label), sizeof(label));
        labels.push_back(label);
    }
}
