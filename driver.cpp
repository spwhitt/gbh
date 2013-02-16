#include <opencv/cv.h>
#include <string>
#include <iostream>
#include "gbh.h"
#include "io.h"

using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {

    if (argc < 3) {
        cout << "Not enough arguments" << endl;
        return 1;
    }

    string input_path = argv[1];
    int sigma = atoi(argv[2]);
    int c = atoi(argv[3]); 

    // ------------------------------------------------------------
    // Read input images

    cout << "Reading Input Images" << endl;

    vector<string>* paths = list_files(input_path);

    // Read each image file from list of paths
    vector<Mat>* images = new vector<Mat>();
    for(unsigned i = 0; i < paths->size(); i++) {
        string fname = paths->at(i);
        Mat im = imread(fname, CV_LOAD_IMAGE_COLOR);
        if(!im.data) {
            cout << "Skipping non-image file " << fname << endl;
        } else {
            cout << fname << endl;
            images->push_back(im);
        }
    }

    // ------------------------------------------------------------
    // Record start time

    double time = (double)getTickCount();

    // ------------------------------------------------------------
    // Segment video
    
    gbh_segment(images, sigma, c);

    // ------------------------------------------------------------
    // Compute time passed

    time = ((double)getTickCount() - time)/getTickFrequency();
    cout << "Time passed: " << time << " seconds" << endl;

    // ------------------------------------------------------------
    // Display results

    write_images(images);

    delete images;

    return 0;
}
