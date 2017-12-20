//
// Created by Ibrahim Radwan on 11/29/17.
//

#ifndef IMAGE2LINES2_UTILITIES_H
#define IMAGE2LINES2_UTILITIES_H

#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <map>
#include <cstring>
#include <cmath>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "opencv2/opencv.hpp"
#include "opencv/cv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "Constants.h"

using namespace std;
using namespace cv;

class Utilities {
public:
    static vector<int> primes;
    ///< Vector contains the needed primes
    static bool notPrimesArr[100007];
    ///< Not primes array (flags)

    /// Apply sieve algorithm to fill primes vector
    static void
    sieve();

    /// Add the primes factors of n to the given primes vector(frequency vector)
    static void
    addPrimesToVector(int n, vector<int> &probPrimes);

    /// Returns a list of files (name, extension) in the given directory
    static void
    getFilesInDirectory(const string &directory, vector<pair<string, string>> &files);

    /// Returns a list of folders (name) in the given directory
    static void
    getFoldersInDirectory(const string &directory, vector<string> &folders);

    /// Loads binary image from the given path into a matrix of pixels
    static Mat
    loadBinaryImage(const string &path);
};


#endif //IMAGE2LINES2_UTILITIES_H
