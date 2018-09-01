//
// Created by Ibrahim Radwan on 11/29/17.
//

#include "Utilities.h"
#include <dirent.h>

vector<int> Utilities::primes;
bool Utilities::notPrimesArr[];

void
Utilities::sieve() {
    notPrimesArr[0] = notPrimesArr[1] = 1;
    for (int i = 2; i < 1e5; ++i) {
        if (primes.size() > 10) break;
        if (notPrimesArr[i]) continue;

        primes.push_back(i);
        for (int j = i * 2; j < 1e5; j += i) {
            notPrimesArr[j] = 1;
        }
    }
}

void
Utilities::addPrimesToVector(int n, vector<int> &probPrimes) {
    for (int i = 0; i < primes.size() && n; ++i) {
        while (n % primes[i]) {
            n /= primes[i];
            probPrimes[i]++;
        }
    }
    if (n != 0) throw Exception();
}

void
Utilities::getFilesInDirectory(const string &directory, vector<pair<string, string>> &files) {
    // Make sure to change the following command to the corresponding
    // one on your operating system when using Linux, MAC or Windows

    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(directory.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {

            string s = ent->d_name;
            if (s[0] == '.') continue;

            int idx = (int) s.size() - 1;
            while (idx >= 0 && s[idx] != '.') --idx;

            files.push_back({s.substr(0, idx), s.substr(idx + 1)});
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror("Error");
    }
}

void
Utilities::getFoldersInDirectory(const string &directory, vector<string> &folders) {
    // Make sure to change the following command to the corresponding
    // one on your operating system when using Linux, MAC or Windows

    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(directory.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {

            if (ent->d_type == DT_DIR && ent->d_name[0] != '.')
                folders.push_back(ent->d_name);
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror("Error");
    }
}

Mat
Utilities::loadBinaryImage(const string& path) {
    // Load gray scale image from file
    Mat grayMat = imread(path, CV_LOAD_IMAGE_GRAYSCALE);

    // Check for invalid input
    if (grayMat.empty() || !grayMat.data) {
        cout << path;
        string errorMessage = "Could not load the image at: " + path;
        throw exception();
    }
    // Binary image
    Mat binaryMat(grayMat.size(), grayMat.type());

    // Apply thresholding
    threshold(grayMat, binaryMat, CHAR_COLOR_THRESHOLD, 255, THRESH_BINARY);

    return binaryMat;
}