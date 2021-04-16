#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
using namespace std;

struct record
{
  string nomer;
  string familia;
  string imya;
  string otchestvo;
};

class database
{
public:
  record* data;
  unsigned int length;
  void parse (string name)
  {
    fstream fin;
    fin.open(name);
    string line;
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
    }
    fin.close();
  }
  void search (string text)
  {
    for (unsigned long long int i = 0; i<length; i++)
    {
      if (data[i].nomer==text) cout<<data[i].familia<<'\n';
    }
  }
};
