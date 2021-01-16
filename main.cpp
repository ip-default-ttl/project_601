//including opencv
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
//other libs libs
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>


using namespace cv;
using namespace std;

int main()
{
  //видеопоток
 /* cam.url="rtsp://172.18.18.1:554/live/0/MAIN";
  VideoCapture cap (cam.url);
  Mat source_img;
  while (true)
  {
    cap>>source_img;
    Mat edges;
    cvtColor(img, edges, COLOR_BGR2GRAY);
    Canny(edges, edges, 30, 60);
    namedWindow("main stream", WINDOW_AUTOSIZE);
    imshow("main stream", edges);
    waitKey(0);
  }*/
  Mat source_img=imread("test.jpeg");
  Mat gray;
  CascadeClassifier cascadePlate;
  bool cascadeLoad = cascadePlate.load("haarcascade_russian_plate_number.xml"); // Загрузка каскада
  if(!cascadeLoad)
  {
    cerr << "Cascade not load. Check your directory \"haarcascade_russian_plate_number_symbol.xml\"" << std::endl;
    return 0;
  }
  cvtColor(source_img, gray, COLOR_BGR2GRAY);
  vector<Rect> plate;
  cascadePlate.detectMultiScale(gray, plate);
  Point rbeg;
  Point rend;
  for (auto& i:plate)
  {
  rbeg = Point(i.x, i.y);
  rend = Point(i.x+i.width, i.y+i.height);
  }
  rectangle(source_img, rbeg, rend, Scalar(1,255,1), 2);
  imshow("Пидорас найден", source_img);
  waitKey(0);
}
