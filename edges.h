#ifndef EDGES_H
#define EDGES_H

#include <cv.h>
#include <iostream>

unsigned num_pixels;

unsigned pixelnum(int x, int y, int z, cv::Mat img) {
    return x + y*img.cols + z*img.total();

}

typedef struct {
    // Edge weight, color distance
	float weight;
    // a and b are pixel numbers that this edge connects.
    // they count pixels in the following order: row, height, depth.
    // x + y*width + z*width*height
	unsigned a, b;
} edge;

// Compare edges by weight
bool operator<(const edge &a, const edge &b) {
    return a.weight < b.weight;
}

float dist(cv::Vec3f p1, cv::Vec3f p2) {
    float a = std::pow(p1[0] - p2[0], 2);
    float b = std::pow(p1[1] - p2[1], 2);
    float c = std::pow(p1[2] - p2[2], 2);

    return std::sqrt(a+b+c);
}

/**
 * Add an edge to the list
 */
void make_edge(vector<edge>* edges, cv::Vec3f p0, unsigned p0_num, vector<cv::Mat> *images, unsigned x, unsigned y, unsigned z) {
    cv::Mat frame = (*images)[z];
    cv::Vec3f pixel = frame.at<cv::Vec3f>(y,x);

    edge e;
    e.a = p0_num;
    //e.b = x+y*frame.rows+z*frame.total();
    e.b = pixelnum(x, y, z, frame);

    if (e.b > num_pixels) {
        cout << e.b << ": " << x << ", " << y << ", " << z << endl;
    }
    e.weight = dist(p0, pixel);
    edges->push_back(e);
}

/**
 * Build an undirected edge between each pair of neighboring pixels.
 *
 * images: Vector of smoothed frames in the CIE-Lab colorspace
 */
vector<edge>* make_pixel_edges(vector<cv::Mat> *images) {

    //       z      |     z+1
    // ---------------------------
    //     x x x    |    x x x
    //     x 0 1    |    x 4 5
    //     8 3 2    |    9 7 6
    //     
    // The current pixel is 0
    //
    // Any pixel labeled x can be safely ignored, because the graph is
    // undirected, and an edge has already been created between them.
    //
    // 1-9 are the pixels which we must actually consider.
    //
    // 8 and 9 are the only pixels which we must check to ensure they do not
    // fall outside the image
    num_pixels = images->at(0).total() * images->size();

    vector<edge>* edges = new vector<edge>();

    for (unsigned z=0; z < images->size(); z++) {
        cv::Mat current_frame = images->at(z);
        for (int y=0; y < images->at(0).rows; y++) {
            for (int x=0; x < images->at(0).cols; x++) {
                // Current pixel
                // 0: (x,y,z)
                cv::Vec3f p0 = current_frame.at<cv::Vec3f>(y,x);
                //unsigned p0_num = x + y*current_frame.rows + z*current_frame.total();
                unsigned p0_num = pixelnum(x, y, z, current_frame);

                //std::cout << p0[0] << ", " << p0[1] << ", " << p0[2] << std::endl;

                // Compute edge for each pixel of interest.
                if (x < current_frame.cols - 1) {
                // 1: (x+1, y,   z  )
                make_edge(edges, p0, p0_num, images, x+1, y, z);
                    if (y < current_frame.rows - 1) {
                        // 2: (x+1, y+1, z  )
                        make_edge(edges, p0, p0_num, images, x+1, y+1, z);
                    }
                }
                if (y < current_frame.rows - 1) {
                    // 3: (x,   y+1, z  )
                    make_edge(edges, p0, p0_num, images, x, y+1, z);
                }
                if (z < images->size() - 1) {
                    // 4: (x,   y,   z+1)
                    make_edge(edges, p0, p0_num, images, x, y, z+1);
                    if (x < current_frame.cols - 1) {
                        // 5: (x+1, y,   z+1)
                        make_edge(edges, p0, p0_num, images, x+1, y, z+1);
                    }
                    if (y < current_frame.rows - 1) {
                        // 7: (x,   y+1, z+1)
                        make_edge(edges, p0, p0_num, images, x, y+1, z+1);
                        if (x < current_frame.cols - 1) {
                            // 6: (x+1, y+1, z+1)
                            make_edge(edges, p0, p0_num, images, x+1, y+1, z+1);
                        }
                    }

                }
                if (x > 0 && y < current_frame.rows - 1) {
                    // 8: (x-1, y+1, z)
                    make_edge(edges, p0, p0_num, images, x-1, y+1, z);
                    if (z < images->size()-1) {
                    // 9: (x-1, y+1, z+1)
                    make_edge(edges, p0, p0_num, images, x-1, y+1, z+1);
                    }
                }
            }
        }
    }

    return edges;
}

#endif
