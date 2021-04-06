//including opencv
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
//including tesseract
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
//other libs libs
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
//for serial port
#include <termios.h>
#include <iostream>


using namespace cv;
using namespace std;

int main()
{
  tesseract::TessBaseAPI* OCR = new tesseract::TessBaseAPI();
  OCR->Init(NULL, "eng");
  OCR->SetVariable("tessedit_char_whitelist","abekmhopctyxABEKMHOPCTYX01234567890");

//  while (true)
  //{
    //cap>>source_img;
    //namedWindow("main stream", WINDOW_AUTOSIZE);
    //imshow("main stream", source_img);
    /*Mat edges;
    cvtColor(source_img, edges, COLOR_BGR2GRAY);
    Canny(edges, edges, 30, 60);
    namedWindow("main stream2", WINDOW_AUTOSIZE);
    imshow("main stream2", edges);
    waitKey(0);*/
  //}
  Mat source_img=imread("test.png");
  //Mat source_img2=imread("test.jpeg");
  Mat gray;
  Mat gray2;
  CascadeClassifier cascadePlate;
  CascadeClassifier cascadePlate2;
  cascadePlate.load("haarcascade_russian_plate_number.xml"); // Загрузка каскада
  //cascadePlate2.load("");
  cvtColor(source_img, gray, COLOR_BGR2GRAY);
  threshold(gray, gray, 0, 255, THRESH_OTSU|THRESH_BINARY);
  //cvtColor(source_img2, gray2, COLOR_BGR2GRAY);
  vector<Rect> plate;
  cascadePlate.detectMultiScale(gray, plate);
  Point rbeg;
  Point rend;
  Mat cropped;
  for (auto& i:plate)
  {
  rbeg = Point(i.x, i.y);
  rend = Point(i.x+i.width, i.y+i.height);
  rectangle(source_img, rbeg, rend, Scalar(1,255,1), 2);
  Mat ROI(source_img, Rect(i.x, i.y,i.width, i.height));
  ROI.copyTo(cropped);
  //resize(cropped, cropped, Size(150,40));
  //threshold(cropped, cropped, 0, 255, THRESH_OTSU|THRESH_BINARY);
  imwrite("tess.jpg", cropped);
  //Canny(cropped,cropped,30,70,7);
  imshow("aa", cropped);
  Pix* pix = pixRead("tess.jpg");
  OCR->SetImage(pix);
  cout<<OCR->GetUTF8Text();
  }
  //imshow("aaaa", cropped);
  imshow("Result", source_img);
  plate.clear();
  waitKey(0);
  /*cascadePlate.detectMultiScale(gray2, plate);
  for (auto& i:plate)
  {
  rbeg = Point(i.x, i.y);
  rend = Point(i.x+i.width, i.y+i.height);
  rectangle(source_img2, rbeg, rend, Scalar(255,1,1), 2);
  }
  imshow("Result2", source_img2);//тут тоже
  waitKey(0);*/
  //waitKey(100);
}
