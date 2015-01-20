#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int size = 0;
Mat image, result, final;

Point2i getTopLeftPoint(Mat& image);
void DrawGrid(Mat& image, int size);
void onSizeChange(int, void*);

int main(int argc, char** argv) {

  if (argc < 1) {
    cout << "Usage: cvsetup <path to image>" << endl;
    return 0;
  }

  image = imread(argv[1]);

  if (!image.data) {
    printf("No image data \n");
    return -1;
  }

  // Convert to greyscale
  cvtColor(image, result, CV_RGB2GRAY);

  // Binarise the image
  threshold(result, result, 200, 255, CV_THRESH_BINARY);

  // Draw the grid
  final = image.clone();
  DrawGrid(final, 5);

  // Display the binary image
  namedWindow("Intermediate image", CV_WINDOW_AUTOSIZE);
  namedWindow("Final image", CV_WINDOW_AUTOSIZE);
  imshow("Intermediate image", result);
  imshow("Final image", final);

  // Create a trakbar to control the grid size
  createTrackbar( "Grid size", "Final image", &size, 50, onSizeChange);
  waitKey(0);

  cout << getTopLeftPoint(result) << endl;

  return 0;
}

Point2i getTopLeftPoint(Mat& image) {
  int nRows = image.rows;
  int nCols = image.cols;

  // if the image has multiple channels
  // int nCols = image.cols*channels;

  if (image.isContinuous()) {
    nCols *= nRows;
    nRows = 1;
  }

  uchar* p;

  for (int i = 0; i < nRows; ++i) {
    p = image.ptr(i);
    for (int j = 0; j < nCols; ++j) {
      // Check for black pixel
      if (p[j] == 0) {
        if (image.isContinuous()) {
          nRows = image.rows;
          Point2i P(j / nRows, j % nRows);
          return P;
        } else {
          Point2i P(i, j);
          return P;
        }
      }
    }
  }

  Point2i P(-1, -1);
  return P;
}

void DrawGrid(Mat& img, int gsize) {
  if (gsize < 1)
    return;
  int nRows = img.rows;
  int nCols = img.cols;
  
  Point2i p1(0,0);
  Point2i p2(0,nRows);
  Point2i stepx(gsize, 0);
  for (int i = gsize; i < nCols; i += gsize)
  {
    p1 += stepx; p2+=stepx;
    line(img, p1, p2, CV_RGB(255, 0, 255), 1, CV_AA, 0);
  }

  p1 = Point2i(0, 0);
  p2 = Point2i(nCols,0);
  Point2i stepy(0, gsize);
  for (int i = gsize; i < nRows; i += gsize)
  {
    p1 += stepy; p2+= stepy;
    line(img, p1, p2, CV_RGB(255, 0, 255), 1, CV_AA, 0);
  }
}

void onSizeChange(int, void *){
  final = image.clone();
  DrawGrid(final, size);
  imshow("Final image", final);
}