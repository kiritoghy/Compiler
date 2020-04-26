#include <cstdio>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <map>
#include "compiler.h"
using namespace std;
string targetFileName;
string errorFileName;
int currentLine = 1;
map<string, string> dydTabel;


int check(char c) {
  if (c == ' ') return 0;
  if (c >= 'a' && c <= 'z') return 1;
  if (c >= '0' && c <= '9') return 2;
  if (c == '=') return 3;
  if (c == '-') return 4;
  if (c == '*') return 5;
  if (c == '(') return 6;
  if (c == ')') return 7;
  if (c == '<') return 8;
  if (c == '>') return 9;
  if (c == ':') return 10;
  if (c == ';') return 11;
  if (c == '\r' || c == '\n') return 12;
  return 13;
}

/*
 1: Invalid symbol. 非法字符
 2: Identifier is too long. 标识符过长
 3: miss '=' after ':'.
*/

string format(string key, string type) {
  int len = 16 - key.length();
  for (int i = 0; i < len; ++i) {
    key = " " + key;
  }
  return key + " " + type;
}
void error(int type, ofstream &outErrorFile) {
  switch (type) {
    case 1: {
      outErrorFile << "***LINK:" + to_string(currentLine) + " Invalid symbol." << endl;
      break;
    }
    case 2: {
      outErrorFile << "***LINK:" + to_string(currentLine) + " Identifier is too long." << endl;
      break;
    }
    case 3: {
      outErrorFile << "***LINK:" + to_string(currentLine) + " miss '=' after ':'." << endl;
      break;
    }
  }
}
void endWord(string &word, ofstream &outTargetFile, ofstream &outErrorFile) {
  if (word.length() == 0)return;
  if (word.length() > 16) {
    error(2, outErrorFile);
    word.clear();
    return;
  }
  map<string, string>::iterator it = dydTabel.find(word);
  if (it != dydTabel.end()) {
    string key = it->first;
    string type = it->second;
    if (key.compare("EOF") == 0) {
      outTargetFile << format(key, type);
    }
    else {
      outTargetFile << format(key, type) << endl;
    }
  }
  else if (word[0] != ':') {
    string type = dydTabel.find("symbol")->second;
    string key = word;
    outTargetFile << format(key, type) << endl;
  }
  else {
    error(3, outErrorFile);
  }
  word.clear();
}

/*
  type:
  blank 0
  char 1
  number 2
  = 3
  - 4
  * 5
  ( 6
  ) 7
  < 8
  > 9
  : 10
  ; 11
  /n 12
  others 13
*/
void gendyd(string sourceFileName) {
  ifstream infile;
  infile.open(sourceFileName.data(), ios::binary);
  assert(infile.is_open());
  string word;
  char c;
  int currentState = 0;
  ofstream outTargetFile, outErrorFile;
  outTargetFile.open(targetFileName.data(), ios::out);
  outErrorFile.open(errorFileName.data(), ios::out);
  assert(outTargetFile.is_open());
  assert(outErrorFile.is_open());
  while (!infile.eof()) {
    infile >> noskipws >>c;
    int type = check(c);

    switch (type) {

      case 0: {
        if (currentState != 0) {
          endWord(word, outTargetFile, outErrorFile);
          currentState = 0;
        }
        break;
      }
      case 1: {
        if (currentState != 0 && currentState != 1) {
          endWord(word, outTargetFile, outErrorFile);
        }
        word += c;
        currentState = 1;
        break;
      }
      case 2: {
        if (currentState == 0) {
          word += c;
          currentState = 3;
        }
        else if (currentState == 3 || currentState == 1) {
          word += c;
        }
        else {
          endWord(word, outTargetFile, outErrorFile);
          word += c;
          currentState = 3;
        }
        break;
      }
      case 3: {
        if (currentState != 0 && currentState != 10 && currentState != 14 && currentState != 17) {
          endWord(word, outTargetFile, outErrorFile);
          word += c;
          currentState = 5;
        }
        else {
          word += c;
          endWord(word, outTargetFile, outErrorFile);
          currentState = 0;
        }
        break;
      }
      case 4: {
        endWord(word, outTargetFile, outErrorFile);
        word += c;
        currentState = 6;
        break;
      }
      case 5: {
        endWord(word, outTargetFile, outErrorFile);
        word += c;
        currentState = 7;
        break;
      }
      case 6: {
        endWord(word, outTargetFile, outErrorFile);
        word += c;
        currentState = 8;
        break;
      }
      case 7: {
        endWord(word, outTargetFile, outErrorFile);
        word += c;
        currentState = 9;
        break;
      }
      case 8: {
        endWord(word, outTargetFile, outErrorFile);
        word += c;
        currentState = 10;
        break;
      }
      case 9: {
        if (currentState == 10) {
          word += c;
          endWord(word, outTargetFile, outErrorFile);
          currentState = 0;
        }
        else {
          endWord(word, outTargetFile, outErrorFile);
          word += c;
          currentState = 14;
        }
        break;
      }
      case 10: {
        endWord(word, outTargetFile, outErrorFile);
        word += c;
        currentState = 17;
        break;
      }
      case 11: {
        endWord(word, outTargetFile, outErrorFile);
        word += c;
        currentState = 20;
        break;
      }
      case 12: {
        endWord(word, outTargetFile, outErrorFile);
        word = "EOLN";
        endWord(word, outTargetFile, outErrorFile);
        currentLine++;
        if (c == '\r') 
          infile >> noskipws >> c;
        break;
      }
      case 13: {
        error(1, outErrorFile);
        break;
      }
    }
  }
  word = "EOF";
  endWord(word, outTargetFile, outErrorFile);
  infile.close();
  outTargetFile.close();
  outTargetFile.close();
}

void initdyd() {
  dydTabel["begin"] = "01";
  dydTabel["end"] = "02";
  dydTabel["integer"] = "03";
  dydTabel["if"] = "04";
  dydTabel["then"] = "05";
  dydTabel["else"] = "06";
  dydTabel["function"] = "07";
  dydTabel["read"] = "08";
  dydTabel["write"] = "09";
  dydTabel["symbol"] = "10";
  dydTabel["constant"] = "11";
  dydTabel["="] = "12";
  dydTabel["<>"] = "13";
  dydTabel["<="] = "14";
  dydTabel["<"] = "15";
  dydTabel[">="] = "16";
  dydTabel[">"] = "17";
  dydTabel["-"] = "18";
  dydTabel["*"] = "19";
  dydTabel[":="] = "20";
  dydTabel["("] = "21";
  dydTabel[")"] = "22";
  dydTabel[";"] = "23";
  dydTabel["EOLN"] = "24";
  dydTabel["EOF"] = "25"; 
}

int lexical_analyzer(string sourceFileName) {
  targetFileName = sourceFileName.substr(0, sourceFileName.find_last_of(".")) + ".dyd";
  errorFileName = sourceFileName.substr(0, sourceFileName.find_last_of(".")) + ".err";

  initdyd();
  gendyd(sourceFileName);

  return 0;
}