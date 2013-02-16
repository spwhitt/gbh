#ifndef IO_H
#define IO_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <dirent.h>

using namespace std;

vector<string>* list_files(string dir) {
    DIR* dir_stream = opendir(dir.c_str());

    if (!dir_stream) {
        cout << "Failed to open directory " << dir << endl;
        exit(EXIT_FAILURE);
    }

    dirent* entry;

    vector<string>* paths = new vector<string>();

    // Get image paths
    while ((entry = readdir(dir_stream))) {
        // If the entry is a regular file
        if (entry->d_type == DT_REG) {
            // Get the filename
            string fname = entry->d_name;
            fname = dir + "/" + fname;
            paths->push_back(fname);
        }
    }

    // clean up
    closedir(dir_stream);

    if (!paths->size()) {
        cout << "Directory is empty" << endl;
        exit(EXIT_FAILURE);
    }

    sort(paths->begin(), paths->end());

    return paths;
}

void write_images(vector<cv::Mat>* images) {
    char buff[15];
    for (unsigned z = 0; z < images->size(); z++) {
        sprintf(buff, "output%04d.png", z);
        cv::imwrite(string(buff), images->at(z));
    }
}

#endif
