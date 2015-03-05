#include "stdio.h"
#include "isothetic.hpp"

int main(int argc, char** argv) {
  Mat image, result, final, test;
  int size;
  int opt;
  int threshval = 200;
  char c;

  if (argc < 2) {
    cout << "Usage: cvsetup <path to image>" << endl;
    return 0;
  }

  image = imread(argv[1]);
  if (!image.data) {
    printf("No image data \n");
    return -1;
  }

  test = image.clone();
  // Convert to greyscale
  cvtColor(image, result, CV_RGB2GRAY);

  // Binarise the image
  cout << "For inverted press 1 for normal 2 : ";
  cin >> opt;
  if (opt == 1)
    threshold(result, result, threshval, 255, CV_THRESH_BINARY_INV);
  else
    threshold(result, result, threshval, 255, CV_THRESH_BINARY);

  // Draw the grid
  cout << "size : ";
  cin >> size;
  final = image.clone();
  drawGrid(final, size);
  // Display the binary image
  imshow("Threshold", result);

  // Make isothetic cover
  vector<Point2i> isotheticcover = animateOIP(result, final, size);
  drawOIC(result, isotheticcover, true);
  imshow("Isothetic Cover", result);
  
  // Fill the isothetic cover with random colors
  int nRows = result.rows;
  int nCols = result.cols;

  Mat pattern(nRows,nCols, CV_8UC3, Scalar(255,255,255));
  patternRandRGB(result, pattern, size, true);
  imshow("Result", pattern);

  // test
  rainbowFill(image, test, result, size, true);
  imshow("Test", test);
  waitKey(0);
  return 0;
}