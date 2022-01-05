#include <opusfile.h>

#include <iostream>

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: ./" << argv[0] << " <infile.opus> <outfile.wav>"
              << std::endl;
  }
}
