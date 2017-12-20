#pragma once

// STL libraries
#include <vector>
#include <map>

// OpenCV libraries
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

// Custom libraries
#include "Segmentation.h"
#include "../Utilities/Constants.h"

using namespace std;
using namespace cv;


// Constants
const int IGNORE_SIZE_THRESHOLD = 3;
const int dirR[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
const int dirC[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
const int MAX_RECURSIVE_CALL = 50000;

struct Region {
	int id;
	int U, D, L, R;	// Up, Down, Left, Right

	bool type;		// To distinguish special characters

	Region() {
		U = L = 1e9;
		D = R = -1e9;

		type = false;
	}

	void merge(const Region& rhs) {
		U = min(U, rhs.U);
		D = max(D, rhs.D);
		L = min(L, rhs.L);
		R = max(R, rhs.R);
	}

	bool operator<(const Region& rhs) const {
		if (R == rhs.R)
			return L < rhs.L;
		return R < rhs.R;
	}
};

class CharSegmentation {
	friend class Segmentation;

private:
	static int rows;
	static int cols;
	static int id;
	static cv::Mat word;
	static cv::Mat redWord;
	static cv::Mat visited;
	static vector<Region> regions;
	static map<int, int> regionsID;
	static Region region;

	static int recursiveCallsCount;

public:
	/**
	 * Segment the given word image into characters.
	 */
	static void segment(cv::Mat& img, cv::Mat& redImg, vector<cv::Mat>& chars, int avgCharWidth);

private:
	/**
	 * Private constructor (i.e. static class).
	 */
	CharSegmentation() {}


	/**
	 * Detect all connected components in the given word image 
	 * that are greater than a certain threshold.
	 */
	static void detectComponents();

	/**
	 * Extract regions of the same id into a matrix, exculding overlapping regions
	 * of different ids, and add it to the chars vector.
	 */
	static void extractChars(vector<cv::Mat>& chars);

	/**
	 * Merge overlapping regions into one region.
	 */
	static void mergeRegions(int avgCharWidth);

	/**
	 * Start depth first search from the given point to get the boundaries
	 * of the connected region and mark it with a unique id.
	 */
	static void dfs(int row, int col);

	/**
	 * Check if the given pixel is inside the image
	 * and that its color is foreground color.
	 */
	static bool valid(int row, int col);

	/**
	 * Initialize character segmentation variables.
	 */
	static void init(cv::Mat& img, cv::Mat& redImg);
};