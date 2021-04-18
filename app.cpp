/*
параметры конфигурации
*/
#define admin_login "admin" //логин для входа
#define admin_password "nimda"//пароль для входа
#define uart_port "/dev/ttyUSB0"//порт uart
#define image_directory "Images/"

//стрим высокого разрешения для распознавания номеров
#define main_stream_url "rtsp://admin:180500Kn@172.18.18.3:554/live/0/MAIN"

//стрим 800*448 10FPS
#define sub_stream_url "rtsp://admin:180500Kn@172.18.18.3:554/live/0/SUB"

/*
библиотеки
*/

//подключаем gtk3
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>

//подключаем opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core.hpp>
//просто нужные библиотеки
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <fstream>
#include <vector>

// подключаем библиотеки для uart
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

//теперь приложение многопоточное
#include <thread>

//including tesseract
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
/*
Глобальные переменные
*/

//виджеты первого окна
GtkWidget* window;
GtkWidget* fixed1;

GtkWidget* button1_1;
GtkWidget* button1_2;

GtkWidget* login_entry;
GtkWidget* password_entry;

GtkWidget* pass_incorrect;
//виджеты второго окна
GtkWidget* window2;
GtkWidget* fixed2;

GtkWidget* button2_1;
GtkWidget* button2_2;
GtkWidget* button2_3;
GtkWidget* button2_4;
GtkWidget* button2_5;
GtkWidget* button2_6;

GtkWidget* entry2_1;

GtkWidget* toggle1;
GtkWidget* toggle2_1;
GtkWidget* toggle2_2;

GtkWidget* video1;
GtkWidget* photo;

GtkWidget* text2_1;
GtkWidget* text_number;
GtkWidget* text_familia;
GtkWidget* text_imya;
GtkWidget* text_otchestvo;
GtkWidget* text_dolzhnost;
GtkWidget* text_avto;
GtkWidget* text_tsvet_avto;
GtkWidget* text_other;

//какая-то неведомая поебень
GtkBuilder* builder;
GdkPixbuf* ccc;
//служебные переменныые
int serial_port;
bool o=true;
bool toggle2_1_on = false;
bool toggle2_2_on = false;
char* sensor_data;

tesseract::TessBaseAPI* OCR;

/*
прототипы функций
*/

//функции кнопок
void on_button1_1_clicked(GtkButton *button, gpointer user_data);
void on_button1_2_clicked(GtkButton *button, gpointer user_data);
void on_button3_clicked(GtkButton *button, gpointer user_data);
void on_button26_clicked(GtkButton *button, gpointer user_data);
void toggle1_toggled(GtkButton *button, gpointer user_data);
void toggle2_1_toggled(GtkButton *button, gpointer user_data);
void toggle2_2_toggled(GtkButton *button, gpointer user_data);

//функции для uart
int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);

bool stream();
void request_sensors();
bool recognizer();
void request_data_from_sensor (char* sensor, int length);
bool update_sensor_widget (GtkWidget* widget);

inline int getRandomNumber(int min, int max)
{
  static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
  return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

bool update_stream(GdkPixbuf* frame)
{
  gtk_image_set_from_pixbuf(GTK_IMAGE(video1),frame);
  return FALSE;
}

Pix *mat8ToPix(cv::Mat *mat8)
{
  Pix *pixd = pixCreate(mat8->size().width, mat8->size().height, 8);
  for(int y=0; y<mat8->rows; y++) {
    for(int x=0; x<mat8->cols; x++) {
      pixSetPixel(pixd, x, y, (l_uint32) mat8->at<uchar>(y,x));
    }
  }
  return pixd;
}

struct record
{
  std::string nomer;
  std::string familia;
  std::string imya;
  std::string otchestvo;
  std::string dolzhnost;
  std::string avto;
  std::string tsvet_avto;
  std::string other;
  std::string filename;
};

class database
{
public:
  record* data;
  unsigned int length;
  void parse (std::string name)
  {
    std::fstream fin;
    fin.open(name);
    std::string line;
    fin >> line;
    length = stoi(line);
    data = new record [length];
    fin >> line;
    int pos;
    for (unsigned int i = 0; i<length; i++) {
      fin >> line;
      pos = line.find(";");
      data[i].nomer=line.substr(0,pos);
      line.erase(0,pos+1);
      pos = line.find(";");
      data[i].familia=line.substr(0,pos);
      line.erase(0,pos+1);
      pos = line.find(";");
      data[i].imya=line.substr(0,pos);
      line.erase(0,pos+1);
      pos = line.find(";");
      data[i].otchestvo=line.substr(0,pos);
      line.erase(0,pos+1);
      pos = line.find(";");
      data[i].dolzhnost=line.substr(0,pos);
      line.erase(0,pos+1);
      pos = line.find(";");
      data[i].avto=line.substr(0,pos);
      line.erase(0,pos+1);
      pos = line.find(";");
      data[i].tsvet_avto=line.substr(0,pos);
      line.erase(0,pos+1);
      pos = line.find(";");
      data[i].other=line.substr(0,pos);
      line.erase(0,pos+1);
      pos = line.find(";");
      data[i].filename=image_directory+line.substr(0,pos);
      line.erase(0,pos+1);
    }
    fin.close();
  }
  record search (std::string text)
  {
    for (unsigned long long int i = 0; i<length; i++)
    {
      if (data[i].nomer==text) return data[i];
    }
    record none;
    none.nomer="";
    return none;
  }
};

database dbase;
void draw_responsed_data ();
record wtf;

int main (int argc, char* argv[])
{
  gtk_init(&argc, &argv);
  srand(time(0));
  sensor_data = new char [50];
  dbase.parse("database.csv");
  wtf.nomer="";
  OCR = new tesseract::TessBaseAPI();
  OCR[0].Init(NULL, "eng");
  OCR[0].SetVariable("tessedit_char_whitelist","abekmhopctyxABEKMHOPCTYX0123456789");
  OCR[0].SetVariable("user_defined_dpi", "100");
  builder = gtk_builder_new_from_file("app.glade");
  //окно входа
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

  fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));

  button1_1 = GTK_WIDGET(gtk_builder_get_object(builder, "button1_1"));
  button1_2 = GTK_WIDGET(gtk_builder_get_object(builder, "button1_2"));

  login_entry = GTK_WIDGET(gtk_builder_get_object(builder, "login_entry"));
  password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry"));

  pass_incorrect = GTK_WIDGET(gtk_builder_get_object(builder, "pass_incorrect"));
  //основное окно
  window2 = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

  fixed2 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed2"));

  button2_1 = GTK_WIDGET(gtk_builder_get_object(builder, "button2_1"));
  button2_2 = GTK_WIDGET(gtk_builder_get_object(builder, "button2_2"));
  button2_3 = GTK_WIDGET(gtk_builder_get_object(builder, "button2_3"));
  button2_4 = GTK_WIDGET(gtk_builder_get_object(builder, "button2_4"));
  button2_5 = GTK_WIDGET(gtk_builder_get_object(builder, "button2_5"));
  button2_6 = GTK_WIDGET(gtk_builder_get_object(builder, "button2_6"));

  text2_1 = GTK_WIDGET(gtk_builder_get_object(builder, "text2_1"));
  text_number = GTK_WIDGET(gtk_builder_get_object(builder, "text_number"));
  text_familia = GTK_WIDGET(gtk_builder_get_object(builder, "text_familia"));
  text_imya = GTK_WIDGET(gtk_builder_get_object(builder, "text_imya"));
  text_otchestvo = GTK_WIDGET(gtk_builder_get_object(builder, "text_otchestvo"));
  text_dolzhnost = GTK_WIDGET(gtk_builder_get_object(builder, "text_dolzhnost"));
  text_avto = GTK_WIDGET(gtk_builder_get_object(builder, "text_avto"));
  text_tsvet_avto = GTK_WIDGET(gtk_builder_get_object(builder, "text_tsvet_avto"));
  text_other = GTK_WIDGET(gtk_builder_get_object(builder, "text_other"));

  entry2_1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry2_1"));

  toggle1 = GTK_WIDGET(gtk_builder_get_object(builder, "toggle1"));
  toggle2_1 = GTK_WIDGET(gtk_builder_get_object(builder, "toggle2_1"));
  toggle2_2 = GTK_WIDGET(gtk_builder_get_object(builder, "toggle2_2"));

  video1 = GTK_WIDGET(gtk_builder_get_object(builder, "video1"));
  photo = GTK_WIDGET(gtk_builder_get_object(builder, "photo"));
  //подсоединяем функции кнопок
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(window2, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(toggle1, "toggled", G_CALLBACK(toggle1_toggled), NULL);
  g_signal_connect(toggle2_1, "toggled", G_CALLBACK(toggle2_1_toggled), NULL);
  g_signal_connect(toggle2_2, "toggled", G_CALLBACK(toggle2_2_toggled), NULL);
  g_signal_connect(GTK_BUTTON(button2_1), "clicked", G_CALLBACK(on_button3_clicked), NULL);
  g_signal_connect(GTK_BUTTON(button2_6), "clicked", G_CALLBACK(on_button26_clicked), NULL);
  g_signal_connect(GTK_BUTTON(button1_2), "clicked", G_CALLBACK(on_button1_2_clicked), NULL);
  g_signal_connect(GTK_BUTTON(button1_1), "clicked", G_CALLBACK(on_button1_1_clicked), NULL);
  serial_port = open(uart_port, O_RDWR | O_NOCTTY | O_SYNC);
  if (serial_port < 0)
  {
    std::cout<<"Error from open:"<<errno<<std::endl<<strerror(errno)<<std::endl;
  }
  set_interface_attribs (serial_port, B9600, 0);
  set_blocking (serial_port, 0);
  //g_timeout_add_seconds (10, GSourceFunc(request_sensors), NULL);
  gtk_widget_show(window);
  std::thread thread1(stream);
  std::thread thread2(request_sensors);
  std::thread thread3(recognizer);
  thread1.detach();
  thread2.detach();
  thread3.detach();
  gtk_main();
  close(serial_port);
  return 0;
}

inline int set_interface_attribs (int fd, int speed, int parity)
{
  struct termios tty;
  if (tcgetattr (fd, &tty) != 0)
  {
    std::cout<<"error from tcgetattr"<<errno;
    return -1;
  }

  cfsetospeed (&tty, speed);
  cfsetispeed (&tty, speed);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
  // disable IGNBRK for mismatched speed tests; otherwise receive break
  // as \000 chars
  tty.c_iflag &= ~IGNBRK;         // disable break processing
  tty.c_lflag = 0;                // no signaling chars, no echo,
  // no canonical processing
  tty.c_oflag = 0;                // no remapping, no delays
  tty.c_cc[VMIN]  = 0;            // read doesn't block
  tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

  tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
  // enable reading
  tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if (tcsetattr (fd, TCSANOW, &tty) != 0)
  {
    std::cout<<"error from tcsetattr"<<errno<<std::endl;
    return -1;
  }
  return 0;
}

inline void set_blocking (int fd, int should_block)
{
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0)
  {
    std::cout<<"error from tggetattr"<<errno<<std::endl;
    return;
  }

  tty.c_cc[VMIN]  = should_block ? 1 : 0;
  tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

  if (tcsetattr (fd, TCSANOW, &tty) != 0)
  std::cout<<"error setting term attributes"<<errno<<std::endl;
}

void on_button1_1_clicked(GtkButton *button, gpointer user_data)
{
  std::string login;
  const char* log = gtk_entry_get_text(GTK_ENTRY((GtkWidget*) login_entry));
  std::string password;
  const char* pas = gtk_entry_get_text(GTK_ENTRY((GtkWidget*) password_entry));
  guint16 len = gtk_entry_get_text_length(GTK_ENTRY((GtkWidget*) login_entry));
  login.assign(log, len);
  len = gtk_entry_get_text_length(GTK_ENTRY((GtkWidget*) password_entry));
  password.assign(pas, len);
  if ((login == admin_login)&&(password==admin_password))
  {
    gtk_widget_destroy(window);
    gtk_widget_show(window2);
    gtk_main();
  }
  else gtk_label_set_text(GTK_LABEL(pass_incorrect), "Неправильный логин/пароль!");
}

void on_button1_2_clicked(GtkButton *button, gpointer user_data)
{
  gtk_entry_set_text(GTK_ENTRY(login_entry), "");
  gtk_entry_set_text(GTK_ENTRY(password_entry), "");
}

void on_button3_clicked (GtkButton *button, gpointer user_data)
{
  write (serial_port, "ser1", 4);
}

void toggle1_toggled(GtkButton *button, gpointer user_data)
{
  std::string msg;
  if (o) msg="ser2";
  else msg="ser3";
  write (serial_port,msg.c_str(), 100);
  o=!o;
}

void toggle2_1_toggled(GtkButton *button, gpointer user_data)
{
  toggle2_1_on = !toggle2_1_on;
  if ((!toggle2_1_on)&&(toggle2_2_on))
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle2_2), FALSE);
  }
}

void toggle2_2_toggled(GtkButton *button, gpointer user_data)
{
  toggle2_2_on = !toggle2_2_on;
  if ((!toggle2_1_on)&&(toggle2_2_on))
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle2_1), TRUE);
  }
}

bool stream()
{
  GdkPixbuf* pix;
  unsigned long long int a=0;
  std::string url="rtsp://admin:180500Kn@172.18.18.3:554/live/0/SUB";
  cv::VideoCapture cap(url);
  if (!cap.isOpened()) {std::cout<<"No video";return 1;}
  cv::Mat frame;
  while (true) {
    cap>>frame;
    cvtColor(frame, frame, cv::COLOR_BGR2RGBA);
    pix = gdk_pixbuf_new_from_data(frame.data, GDK_COLORSPACE_RGB, true, 8, frame.cols, frame.rows, frame.step, NULL, NULL);
    g_idle_add(GSourceFunc(update_stream), pix);
    usleep(9000);
  }
  cap.release();
  return TRUE;
}

void request_sensors ()
{
  //echo каждые 10 сек
  //tem каждые 90 сек
  //hum каждые 30 сек
  //газ каждые 60 сек
  auto t1 = std::chrono::high_resolution_clock::now();
  auto t2 = t1;
  bool update_needed = true;
  int last_updated_interval=0;
  while (true)
  {
    t2=std::chrono::high_resolution_clock::now();
    int duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
    if (duration>90)
    {
      t1 = std::chrono::high_resolution_clock::now();
      t2 = t1;
      update_needed = true;
      last_updated_interval=0;
      continue;
    }
    if (duration/10>last_updated_interval) //ебздец тут костыль
    {
      last_updated_interval++;
      update_needed=true;
    }
    if (update_needed) { //хуйня ебанистическая получилась в итоге
      switch (duration/10)
      {
        case 0:
        break;
        case 3:
        {
          request_data_from_sensor("hum", 3);
          g_idle_add(GSourceFunc(update_sensor_widget), button2_3);
          update_needed = false;
          break;
        }
        case 6:
        {
          request_data_from_sensor("smoke", 5);
          g_idle_add(GSourceFunc(update_sensor_widget), button2_4);
          update_needed = false;
          break;
        }
        case 9:
        {
          request_data_from_sensor("tem", 3);
          g_idle_add(GSourceFunc(update_sensor_widget), button2_2);
          update_needed = false;
          break;
        }
        default:
        {
          request_data_from_sensor("echo", 4);
          g_idle_add(GSourceFunc(update_sensor_widget), button2_5); //gtk suka zaebal
          update_needed = false;
          break;
        }
      }
    }
  }
} //я в ахуе, но она бля работает

void request_data_from_sensor (char* sensor, int length)
{
  int n = 0;
  sensor_data = new char [50];
  for (int i=0; i<50; i++)
  {
    sensor_data[i]='\0';
  }
  auto t1 = std::chrono::high_resolution_clock::now();
  auto t2 = t1;
  write (serial_port, sensor, length);
  usleep ((7 + 25) * 100);
  while (n<=0)
  {
    n = read (serial_port, sensor_data, 50);
    t2 = std::chrono::high_resolution_clock::now();
    int duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
    if (duration>5)
    {
      std::cout<<"timeout!!"<<std::endl;
      break;
    }
  }
}

bool update_sensor_widget (GtkWidget* widget) //gtk иди в пизду со своей ошибкой сегментации
{
  gtk_button_set_label(GTK_BUTTON(widget), sensor_data);
  return FALSE;
}

bool recognizer()
{
  cv::Mat source_img;
  cv::Mat gray;
  cv::CascadeClassifier cascadePlate;
  cascadePlate.load("haarcascade_russian_plate_number.xml");
  cv::VideoCapture cap;
  cap.open(main_stream_url);
  if (!cap.isOpened()) {std::cout<<"No video";return 1;}
  while(true)
  {
    if (toggle2_1_on)
    {
      cap>>source_img;
      cv::cvtColor(source_img, gray, cv::COLOR_BGR2GRAY);
      threshold(gray, gray, 0, 255, cv::THRESH_OTSU|cv::THRESH_BINARY);
      std::vector<cv::Rect> plate;
      cascadePlate.detectMultiScale(gray, plate);
      cv::Point rbeg;
      cv::Point rend;
      cv::Mat cropped;
      for (auto& i:plate)
      {
        rbeg = cv::Point(i.x, i.y);
        rend = cv::Point(i.x+i.width, i.y+i.height);
        cv::Mat ROI(source_img, cv::Rect(i.x, i.y,i.width, i.height));
        ROI.copyTo(cropped);
        Pix* pix = mat8ToPix(&cropped);
        OCR[0].SetImage(pix);
        std::string ab = OCR[0].GetUTF8Text();
        for (int i=0;i<ab.size();i++)
        {
          ab[i]=toupper(ab[i]);
        }
        wtf = dbase.search(ab);
        if (wtf.nomer!="")
        {
          g_idle_add(GSourceFunc(draw_responsed_data), NULL);
          sleep(1);
          if (toggle2_2_on)
          {
            write (serial_port, "ser1", 4);
            usleep ((7 + 25) * 100);
          }
        }
      }
    }
  }
}

void draw_responsed_data ()
{
  gtk_entry_set_text(GTK_ENTRY(text_number), ("Номер: "+wtf.nomer).c_str());
  gtk_entry_set_text(GTK_ENTRY(text_familia), ("Фамилия: "+wtf.familia).c_str());
  gtk_entry_set_text(GTK_ENTRY(text_imya), ("Имя: "+wtf.imya).c_str());
  gtk_entry_set_text(GTK_ENTRY(text_otchestvo), ("Отчество: "+wtf.otchestvo).c_str());
  gtk_entry_set_text(GTK_ENTRY(text_dolzhnost), ("Должность: "+wtf.dolzhnost).c_str());
  gtk_entry_set_text(GTK_ENTRY(text_avto), ("Автомобиль: "+wtf.avto).c_str());
  gtk_entry_set_text(GTK_ENTRY(text_tsvet_avto), ("Цвет авто: "+wtf.tsvet_avto).c_str());
  gtk_entry_set_text(GTK_ENTRY(text_other), ("Другое: "+wtf.other).c_str());
  cv::Mat frame = cv::imread(wtf.filename);
  if (frame.empty()) return;
  cv::resize(frame, frame, cv::Size(300,400));
  GdkPixbuf* pix;
  cvtColor(frame, frame, cv::COLOR_BGR2RGBA);
  pix = gdk_pixbuf_new_from_data(frame.data, GDK_COLORSPACE_RGB, true, 8, frame.cols, frame.rows, frame.step, NULL, NULL);
  gtk_image_set_from_pixbuf(GTK_IMAGE(photo),pix);
  wtf.nomer="";
}

void on_button26_clicked(GtkButton *button, gpointer user_data)
{
  std::string search_number = std::string(gtk_entry_get_text(GTK_ENTRY(entry2_1)));
  record current;
  current = dbase.search(search_number);
  gtk_entry_set_text(GTK_ENTRY(text_number), ("Номер: "+current.nomer).c_str());
  gtk_entry_set_text(GTK_ENTRY(text_familia), ("Фамилия: "+current.familia).c_str());
  gtk_entry_set_text(GTK_ENTRY(text_imya), ("Имя: "+current.imya).c_str());
  gtk_entry_set_text(GTK_ENTRY(text_otchestvo), ("Отчество: "+current.otchestvo).c_str());
  gtk_entry_set_text(GTK_ENTRY(text_dolzhnost), ("Должность: "+current.dolzhnost).c_str());
  gtk_entry_set_text(GTK_ENTRY(text_avto), ("Автомобиль: "+current.avto).c_str());
  gtk_entry_set_text(GTK_ENTRY(text_tsvet_avto), ("Цвет авто: "+current.tsvet_avto).c_str());
  gtk_entry_set_text(GTK_ENTRY(text_other), ("Другое: "+current.other).c_str());
  cv::Mat frame = cv::imread(current.filename);
  GdkPixbuf* pix;
  if (frame.empty()) return;
  cv::resize(frame, frame, cv::Size(300,400));
  cvtColor(frame, frame, cv::COLOR_BGR2RGBA);
  pix = gdk_pixbuf_new_from_data(frame.data, GDK_COLORSPACE_RGB, true, 8, frame.cols, frame.rows, frame.step, NULL, NULL);
  gtk_image_set_from_pixbuf(GTK_IMAGE(photo),pix);
}
