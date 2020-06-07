#include <iostream>
#include <fstream>
#include "src/InterfaceManager.h"
int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "usage: gen_interface_v2 ${interface_filename}.i > ${output_header_name}.hpp\n";
    return 1;
  }
  if (argc > 2) {
    std::ofstream os(argv[2]);
    InterfaceManager().parse(argv[1], os);
  } else {
    InterfaceManager().parse(argv[1]);
  }
}

