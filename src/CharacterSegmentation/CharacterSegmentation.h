#pragma once
// STL libraries
#include <iostream>
#include <vector>
#include <map>
using namespace std;

// OpenCV libraries
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

// Utilities
#include "../Utilities/Utilities.h"

// Constants
const int BACKCOLOR = 255;
const int FORECOLOR = 0;
const int MERGE_THRESHOLD = 50;
const int dirR[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
const int dirC[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };

struct CRegion {
	int U;	// Up
	int D;	// Down
	int L;	// Left
	int R;	// Right
	int id;

	CRegion() {
		U = L = 1e9;
		D = R = -1e9;
	}

	void merge(const CRegion& rhs) {
		U = min(U, rhs.U);
		D = max(D, rhs.D);
		L = min(L, rhs.L);
		R = max(R, rhs.R);
	}

	bool operator<(const CRegion& rhs) const {
		if (R == rhs.R)
			return L > rhs.L;
		return R < rhs.R;
	}
};

class CharacterSegmentation {
private:
	static int n, m, id;
	static Mat word;
	static Mat visited;
	static vector<CRegion> regions;
	static map<int, int> regionsID;
	static CRegion region;

public:
	static void segment();

private:
	CharacterSegmentation() {}	// Private constructor

	static void segment_word(Mat& img, vector<Mat>& chars);

	static void extractChars(vector<Mat>& chars);

	static void mergeCRegions();

	static void dfs(int row, int col);

	static bool valid(int row, int col);

	static void init(Mat& img);

	static void exportChars(string output_path, vector<Mat> &chars);
};