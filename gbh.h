#ifndef GBH_H
#define GBH_H

#include <vector>
#include <opencv/cv.h>

void gbh_segment(std::vector<cv::Mat>* images, int sigma, int c);

#endif
