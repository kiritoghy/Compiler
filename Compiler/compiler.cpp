#include <iostream>
#include <string>
#include "compiler.h"
using namespace std;



int main(int argc, char* argv[]) {
  if (argc != 2) {
    cout << "You should enter the name of source code.";
  }

  string sourceFileName = argv[1];
  lexical_analyzer(sourceFileName);
}