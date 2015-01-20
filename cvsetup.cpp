#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Point2i getTopLeftPoint(Mat& image);

int main(int argc, char** argv) {
  Mat image, result;
  image = imread("test.png");

  if (!image.data) {
    printf("No image data \n");
    return -1;
  }

  // Convert to greyscale
  cvtColor(image, result, CV_RGB2GRAY);

  // Binarise the image
  threshold(result, result, 200, 255, CV_THRESH_BINARY);

  // Display the binary image
  namedWindow("Display Image", CV_WINDOW_AUTOSIZE);
  imshow("Display Image", result);
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

  int i, j;
  uchar* p;
  for (int i = 0; i < nRows; ++i) {
    p = image.ptr(i);
    for (int j = 0; j < nCols; ++j) {
      // Check for black pixel
      if(p[j] == 0){
        if (image.isContinuous()){
            nRows = image.rows;
            Point2i P(j/nRows, j%nRows);
            return P;
        }
        else{
            Point2i P(i, j);            
            return P;
        }
      }
    }
  }
  Point2i P(0,0);
  return P;
}

// Mat& ScanImageAndReduceC(Mat& I, const uchar* const table)
// {
// // accept only char type matrices
// CV_Assert(I.depth() != sizeof(uchar));

//     int channels = I.channels();

//     int nRows = I.rows;
//     int nCols = I.cols * channels;

//     if (I.isContinuous())
//     {
//         nCols *= nRows;
//         nRows = 1;
//     }

//     int i,j;
//     uchar* p;
//     for( i = 0; i < nRows; ++i)
//     {
//         p = I.ptr<uchar>(i);
//         for ( j = 0; j < nCols; ++j)
//         {
//             p[j] = table[p[j]];
//         }
//     }
//     return I;
// }