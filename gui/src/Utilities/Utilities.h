#pragma once

// STL libraries
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <map>

// OpenCV libraries
#include <cv.h>
#include <opencv/cv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"


// Custom libraries
#include "Constants.h"

using namespace std;
using namespace cv;


class Utilities {
public:
	/**
	 * Load image from the given path and 
	 * return it in a matrix of pixels form.
	 */
	static cv::Mat loadImage(const string& path);

	/**
	 * Resize the given image if its size is above the maximum limit
	 * while keeping the original aspect ratio.
	 */
	static void resizeImage(Mat& img, int maxWidth, int maxHeight);

	/**
	 * Perform gamma correction function to the given image.
	 */
	static void gammaCorrection(Mat& src, double gamma);

	/**
	 * Draw rectangle with the specified color
	 * starting from the given upper left point with the given width and height.
	 */
	static void drawRect(Mat& img, int x, int y, int w, int h, Vec3b color);

	/**
	 * Convert the given number to a numerical string of the specified length
	 */
	static string toString(int num, int length);

	/**
	 * Make a new directory at the given path.
	 */
	static void makeDir(const string& dir);

	/**
	 * Remove the given directory recursively.
	 */
	static void removeDir(const string& dir);
};