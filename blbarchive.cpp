#include "blbarchive.h"

namespace Blb {

  BlbArchive::BlbArchive(std::string filename) {
    _filename = filename;
    _fs.open(filename, std::ios::binary);

    if (!_fs.is_open()) {
      throw std::invalid_argument(filename + ": failed to open archive file.");
    }

    _fs.read((char*)&_header.id1, sizeof(uint32_t));
    _fs.read((char*)&_header.id2, sizeof(uint16_t));
    _fs.read((char*)&_header.shiftSize, sizeof(uint16_t));
    _fs.read((char*)&_header.archiveSize, sizeof(uint32_t));
    _fs.read((char*)&_header.fileCount, sizeof(uint32_t));

    // TODO: check archive validity
  }

  BlbArchive::~BlbArchive() {
    _fs.close();
  }

  void BlbArchive::debug() {
    std::cerr << _filename << " (header)" << std::endl;
    std::cerr << "---------------" << std::endl;
    std::cerr << "id1: " << "0x" << std::hex << (uint)_header.id1 << std::endl;
    std::cerr << "id2: " << "0x" << std::hex << (uint)_header.id2 << std::endl;
    std::cerr << "shiftSize: "<< std::dec << (uint)_header.shiftSize << std::endl;
    std::cerr << "archiveSize: " << std::dec <<  (uint)_header.archiveSize << std::endl;
    std::cerr << "fileCount: " << std::dec << (uint)_header.fileCount << std::endl;
    std::cerr << "---------------" << std::endl;
  }
}
