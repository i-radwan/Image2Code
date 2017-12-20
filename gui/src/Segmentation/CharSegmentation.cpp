#include "CharSegmentation.h"

//
// Static members declaration
//
int CharSegmentation::rows;
int CharSegmentation::cols;
int CharSegmentation::id;
cv::Mat CharSegmentation::word;
cv::Mat CharSegmentation::redWord;
cv::Mat CharSegmentation::visited;
vector<Region> CharSegmentation::regions;
map<int, int> CharSegmentation::regionsID;
Region CharSegmentation::region;
int CharSegmentation::recursiveCallsCount = 0;
//========================================================================

void CharSegmentation::segment(cv::Mat& img, cv::Mat& redImg, vector<cv::Mat>& chars, int threshold) {
	init(img, redImg);
	detectComponents();
	mergeRegions(threshold);
	extractChars(chars);
}

void CharSegmentation::detectComponents() {
	// Scan word matrix
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			// Continue if previously visited or background pixel
			if (visited.at<uchar>(i, j) || word.at<uchar>(i, j) == BACKCOLOR) {
				continue;
			}

			// Get region boundries	
			region = Region();
			region.id = ++id;
			regionsID[id] = id;
			recursiveCallsCount = 0;
			dfs(i, j);

			if (recursiveCallsCount > MAX_RECURSIVE_CALL) {
				continue;
			}

			int w = region.R - region.L + 1;
			int h = region.D - region.U + 1;

			// Ignore component if it is smaller than a certain threshold
			if (max(w, h) <= IGNORE_SIZE_THRESHOLD) {
				continue;
			}

			// Store character region
			regions.push_back(region);
		}
	}
}

void CharSegmentation::extractChars(vector<cv::Mat>& chars) {
	for (auto& r : regions) {
		int w = r.R - r.L + 1;
		int h = r.D - r.U + 1;
		cv::Mat m(h, w, CV_8U, cv::Scalar(BACKCOLOR));

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

void CharSegmentation::mergeRegions(int avgCharWidth) {
	vector<Region> tmp;
	vector<bool> vis(regions.size(), 0);
	sort(regions.begin(), regions.end());
	
	for (int i = 0; i < regions.size(); ++i) {
		if (vis[i]) {
			continue;
		}

		tmp.push_back(regions[i]);
		Region& p = tmp.back();

		for (int j = i + 1; j < regions.size(); ++j) {
			Region& q = regions[j];

			if (p.type != q.type) {
				break;
			}

			int widthP = p.R - p.L - 1;
			int widthQ = q.R - q.L - 1;
			int maxWidht = max(widthP, widthQ);
			int mergeWidth = q.R - min(p.L, q.L) + 1;

			if (maxWidht > 2 * avgCharWidth) {
				break;
			}

			if ((mergeWidth - maxWidht) * 2 > avgCharWidth) {
				break;
			}
			
			vis[j] = 1;
			regionsID[q.id] = p.id;
			p.merge(q);
		}
	}

	regions.swap(tmp);
}

void CharSegmentation::dfs(int row, int col) {
	if (recursiveCallsCount >= MAX_RECURSIVE_CALL) {
		return;
	}

	recursiveCallsCount++;

	// Update boundries
	region.U = min(region.U, row);
	region.L = min(region.L, col);
	region.D = max(region.D, row);
	region.R = max(region.R, col);

	// Check if its special char based on its red color
	if (redWord.at<uchar>(row, col) == FORECOLOR) {
		region.type = true;
	}

	// Set current pixel as visisted with current component id
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

bool CharSegmentation::valid(int row, int col) {
	return (
		row >= 0 && row < rows &&
		col >= 0 && col < cols &&
		word.at<uchar>(row, col) == FORECOLOR
	);
}

void CharSegmentation::init(cv::Mat& img, cv::Mat& redImg) {
	rows = img.rows;
	cols = img.cols;
	id = 0;
	word = img;
	redWord = redImg;
	visited = cv::Mat::zeros(rows, cols, CV_8U);
	regions.clear();
	regionsID.clear();
}