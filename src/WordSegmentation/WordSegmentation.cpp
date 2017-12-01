//
// Created by Ibrahim Radwan on 12/1/17.
//

#include "WordSegmentation.h"

void
WordSegmentation::segment() {
    // Real all lines
    vector<pair<string, string>> files;
    Utilities::getFilesInDirectory(LINE_OUTPUT_PATH, files);

    for (int i = 0; i < files.size(); ++i) {
        // Create folder for this line (to store words)
        mkdir((WORD_OUTPUT_PATH + files[i].first).c_str(), 0700);

        // Read each line and segment to words
        this->divide_line(LINE_OUTPUT_PATH + files[i].first + "." + files[i].second,
                          WORD_OUTPUT_PATH + files[i].first + "/",
                          45);
    }
}

void
WordSegmentation::divide_line(string line_path, string output_path, int threshold) {
    Mat line_img = imread(line_path, CV_LOAD_IMAGE_UNCHANGED);

    int rows = line_img.rows;
    int cols = line_img.cols;

    vector<int> x_histogram(cols, 0);

    int blankColsCnt = 0;
    int lastFirstBlack = -1;
    int cnt = 0;
    for (int i = 0; i < cols; ++i) {
        for (int j = 0; j < rows; ++j) {
            x_histogram[i] += ((line_img.at<uchar>(j, i) == 0) ? 1 : 0);
        }

        if (x_histogram[i] == 0) { // No black in this col
            blankColsCnt++;
        } else {
            if (blankColsCnt >= threshold && lastFirstBlack > -1) {
                Mat word = line_img(
                        Rect(max(0, lastFirstBlack - 1), 0, i - 1 - max(0, lastFirstBlack - 1), rows - 1));
                imwrite(output_path + to_string(cnt++) + ".jpg", word);
                lastFirstBlack = 0 - 1;
            }

            if (lastFirstBlack == -1) lastFirstBlack = i;
            blankColsCnt = 0;
        }
    }
    // Save last word
    if (blankColsCnt >= threshold && lastFirstBlack > -1) {
        Mat word = line_img(Rect(max(0, lastFirstBlack - 1), 0, cols - 1 - max(0, lastFirstBlack - 1), rows - 1));
        imwrite(output_path + to_string(cnt) + ".jpg", word);
    }
}