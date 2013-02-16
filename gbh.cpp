#include <opencv/cv.h>
#include <string>
#include <iostream>
#include "union-find.h"
#include "edges.h"
#include "gbh.h"

using namespace cv;
using namespace std;

// random color
Vec3b random_rgb() {
	Vec3b c;
	c[0] = (uchar) random();
	c[1] = (uchar) random();
	c[2] = (uchar) random();
	return c;
}

//struct node {


void gbh_segment(vector<Mat>* images, int SIGMA, int c) {

    cout << "Image smoothing and color conversion" << endl;
    for (unsigned z = 0; z < images->size(); z++) {
        cvtColor((*images)[z], (*images)[z], CV_BGR2Lab);
        (*images)[z].convertTo((*images)[z], CV_32F);
        GaussianBlur((*images)[z], (*images)[z], Size(0,0), SIGMA);
    }

    // ------------------------------------------------------------

    cout << "Building edges" << endl;
    vector<edge>* edges = make_pixel_edges(images);

    // ------------------------------------------------------------

    cout << "Building nodes" << endl;
    // Create union find data structure
    unsigned num_pixels = (*images)[0].total() * images->size();
    UFarray ufa(num_pixels);

    // ------------------------------------------------------------
    
    cout << "Over-Segmentation" << endl;

    sort(edges->begin(), edges->end());

    int min_size = 5;

    vector<unsigned> sizes(num_pixels, 1);
    vector<float> mst(num_pixels, 0);
    //vector<Histogram> hists(num_pixels);

    vector<edge>* remaining_edges = new vector<edge>();

    for (unsigned n=0; n < edges->size(); n++) {
        edge e = (*edges)[n];
        // TODO: Better names than a_label, b_label
        unsigned a_label = ufa.find(e.a);
        unsigned b_label = ufa.find(e.b);
        if (a_label != b_label) {
            if (e.weight <= mst[a_label]+c/sizes[a_label] 
                    && e.weight <= mst[b_label]+c/sizes[b_label]) {
                // Join the labels
                unsigned newroot = ufa.join(a_label, b_label);
                // Record the size of the joined component
                sizes[newroot] = sizes[a_label] + sizes[b_label];
                // Record the largest edge in the minimum spanning tree
                // It can be either:
                //   1) the largest edge from component a
                //   2) the largest edge from component b
                //   3) The edge which connects those two components
                float tmp = std::max(mst[a_label], mst[b_label]);
                mst[newroot] = std::max(tmp, e.weight);
                //cout << e.weight << ", " << mst[newroot] << endl;
            } else {
                remaining_edges->push_back(e);
            }
        }
    }

    // Min-size seems like a crutch
    //for (unsigned n=0; n < edges->size(); n++) {
        //edge e = (*edges)[n];
        //// TODO: Better names than a_label, b_label
        //unsigned a_label = ufa.find(e.a);
        //unsigned b_label = ufa.find(e.b);
        //if (a_label != b_label) {
            //if (sizes[a_label] < min_size || sizes[b_label] < min_size) {
                //ufa.join(a_label, b_label);
            //}
        //}
    //}

    // ------------------------------------------------------------


    cout << "Visualizing results" << endl;
    // TODO: Don't visualize in this function

    Vec3b* colors = new Vec3b[num_pixels];
    for (unsigned i=0; i < num_pixels; i++) {
        colors[i] = random_rgb();
    }

    vector<unsigned> P = ufa.flattenL();

    for (unsigned z = 0; z < images->size(); z++) {
        Mat out(images->at(0).size(), CV_8UC3);
        for (int y = 0; y < out.rows; y++) {
            for (int x = 0; x < out.cols; x++) {
                //unsigned num = x + y*out.rows + z*out.total();
                unsigned num = pixelnum(x, y, z, out);
                unsigned label = P[num];
                out.at<Vec3b>(y,x) = colors[label];
            }
        }
        images->at(z) = out;
    }

    delete edges;
    delete [] colors;
}
