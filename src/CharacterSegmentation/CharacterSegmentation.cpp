#pragma once

#include "CharacterSegmentation.h"

//
// Static members declaration
//
int CharacterSegmentation::n;
int CharacterSegmentation::m;
int CharacterSegmentation::id;
Mat CharacterSegmentation::word;
Mat CharacterSegmentation::visited;
vector<CRegion> CharacterSegmentation::regions;
map<int, int> CharacterSegmentation::regionsID;
CRegion CharacterSegmentation::region;
//========================================================================

/**
 * Get the words and send them to the segment function
 */
void CharacterSegmentation::segment() {
    // Read all folders (lines)
    vector<string> folders;
    Utilities::getFoldersInDirectory(WORD_OUTPUT_PATH, folders);

    // Iterate over all the lines to get the words
    for (int i = 0; i < folders.size(); ++i) {
        // Create folder for this line
        mkdir((CHARACTER_OUTPUT_PATH + folders[i]).c_str(), 0700);

        // Get all words pictures
        vector<pair<string, string>> files;
        Utilities::getFilesInDirectory(WORD_OUTPUT_PATH + folders[i], files);

        // Segment each word
        for (int j = 0; j < files.size(); ++j) {
            // Create folder for this word (to store characters)
            mkdir((CHARACTER_OUTPUT_PATH + folders[i] + "/" + files[j].first).c_str(), 0700);

            // Read each line and segment to words
            vector<Mat> chars;
            Mat word = Utilities::loadBinaryImage(
                    WORD_OUTPUT_PATH + folders[i] + "/" + files[j].first + "." + files[j].second);

            // Perform segmentation
            CharacterSegmentation::segment_word(word, chars);

            // Print chars to files
            CharacterSegmentation::exportChars(CHARACTER_OUTPUT_PATH + folders[i] + "/" + files[j].first + "/", chars);
        }
    }
}

/**
 * Segment the given word image into characters and add them
 * to the given chars vector.
 */
void CharacterSegmentation::segment_word(Mat &img, vector<Mat> &chars) {
    // Initialization
    init(img);

    // Scan word matrix
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            // Continue if previously visited or background pixel
            if (visited.at<uchar>(i, j) || word.at<uchar>(i, j) == BACKCOLOR) {
                continue;
            }

            // Get region boundries
            region = CRegion();
            region.id = ++id;
            regionsID[id] = id;
            dfs(i, j);

            // Store character region
            regions.push_back(region);
        }
    }

    mergeCRegions();
    extractChars(chars);
}

/**
 * Extract regions of the same id into a matrix
 * and add it to the chars vector.
 */
void CharacterSegmentation::extractChars(vector<Mat> &chars) {
    for (auto &r : regions) {
        int w = r.R - r.L + 1;
        int h = r.D - r.U + 1;
        Mat m(h, w, CV_8U, Scalar(BACKCOLOR));

        for (int i = r.U; i <= r.D; ++i) {
            for (int j = r.L; j <= r.R; ++j) {
                id = visited.at<uchar>(i, j);

                if (regionsID[id] == r.id) {
                    m.at<uchar>(i - r.U, j - r.L) = FORECOLOR;
                }
            }
        }

        chars.push_back(m);
    }
}

/**
 * Merge overlapping regions into one region.
 */
void CharacterSegmentation::mergeCRegions() {
    vector<CRegion> tmp;
    sort(regions.rbegin(), regions.rend());

    for (int i = 0; i < regions.size(); ++i) {
        CRegion &p = regions[i];

        for (i++; i < regions.size(); ++i) {
            CRegion &q = regions[i];

            int commonWidth = q.R - max(p.L, q.L) + 1;

            if (commonWidth * 100 < (p.R - p.L + 1) * MERGE_THRESHOLD) {
                i--;
                break;
            }

            regionsID[q.id] = p.id;
            p.merge(q);
        }

        tmp.push_back(p);
    }

    reverse(tmp.begin(), tmp.end());
    regions.swap(tmp);
}

/**
 * Start depth first search from the given point to get the boundaries
 * of the connected region and mark it with a unique id.
 */
void CharacterSegmentation::dfs(int row, int col) {
    // Update boundries
    region.U = min(region.U, row);
    region.L = min(region.L, col);
    region.D = max(region.D, row);
    region.R = max(region.R, col);

    // Set current pixel as visisted
    visited.at<uchar>(row, col) = id;

    // Visit neighbours
    for (int i = 0; i < 8; ++i) {
        int toR = row + dirR[i];
        int toC = col + dirC[i];

        if (valid(toR, toC) && !visited.at<uchar>(toR, toC)) {
            dfs(toR, toC);
        }
    }
}

/**
 * Check if the given pixel is inside the image
 * and that its color is foreground color.
 */
bool CharacterSegmentation::valid(int row, int col) {
    return (
            row >= 0 && row < n &&
            col >= 0 && col < m &&
            word.at<uchar>(row, col) == FORECOLOR
    );
}

/**
 * Initialize character segmentation variables.
 */
void CharacterSegmentation::init(Mat &img) {
    n = img.rows;
    m = img.cols;
    id = 0;
    word = img;
    visited = Mat::zeros(n, m, CV_8U);
    regions.clear();
    regionsID.clear();
}

/**
 * Export the segmented character as photos for the classifier
 */
void CharacterSegmentation::exportChars(string output_path, vector<Mat> &chars) {
    for (int i = 0; i < chars.size(); ++i) {
        imwrite(output_path + to_string(i) + ".jpg", chars[i]);
    }
}