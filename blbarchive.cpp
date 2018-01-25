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

    _fileIds.reserve(_header.fileCount);
    for (uint32_t i = 0; i < _header.fileCount; i++) {
      uint32_t fileId;
      _fs.read((char*)&fileId, sizeof(uint32_t));
      _fileIds.push_back(fileId);
    }

    uint16_t* shiftOffsets = (uint16_t*)malloc(sizeof(uint16_t) * _header.fileCount);
    for (uint32_t i = 0; i < _header.fileCount; i++) {
      BlbFileEntry entry;

      _fs.read((char*)&entry.type, sizeof(uint8_t));
      _fs.read((char*)&entry.compr, sizeof(uint8_t));
      entry.shiftVal = DW_SHIFT_DEFAULT;
      _fs.read((char*)&shiftOffsets[i], sizeof(uint16_t));
      _fs.read((char*)&entry.timeStamp, sizeof(uint32_t));
      _fs.read((char*)&entry.offset, sizeof(uint32_t));
      _fs.read((char*)&entry.diskSize, sizeof(uint32_t));
      _fs.read((char*)&entry.outSize, sizeof(uint32_t));

      std::pair<uint32_t, BlbFileEntry> entryPair(_fileIds[i], entry);
      _fileEntries.insert(entryPair);
    }

    uint8_t* const shiftArray = (uint8_t*)malloc(_header.shiftSize);
    _fs.read((char*)shiftArray, _header.shiftSize);

    for (uint32_t i = 0; i < _header.fileCount; i++) {
      BlbFileEntry &entry = _fileEntries[_fileIds[i]];

      if (shiftOffsets[i] < _header.shiftSize) {
	entry.shiftVal = shiftArray[shiftOffsets[i]];
      }
    }

    free(shiftOffsets);
    free(shiftArray);

    _fs.seekg(0, std::ios::beg);
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

    for (uint i = 0; i < _header.fileCount; i++) {
      std::cerr << _filename << " (" << i + 1 << "/" << _header.fileCount << ")" << std::endl;
      std::cerr << "---------------" << std::endl;
      std::cerr << "id: " << "0x" << std::hex << (uint)_fileIds[i] << std::endl;
      std::cerr << "type: " << "0x" << std::hex << (uint)_fileEntries[_fileIds[i]].type << std::endl;
      std::cerr << "compr: " << "0x" << std::hex << (uint)_fileEntries[_fileIds[i]].compr << std::endl;
      std::cerr << "shiftVal: " << "0x" << std::hex << (uint)_fileEntries[_fileIds[i]].shiftVal << std::endl;
      std::cerr << "timeStamp: " << std::dec << (uint)_fileEntries[_fileIds[i]].timeStamp << std::endl;
      std::cerr << "offset: " << std::dec << (uint)_fileEntries[_fileIds[i]].offset << std::endl;
      std::cerr << "diskSize: " << std::dec << (uint)_fileEntries[_fileIds[i]].diskSize << std::endl;
      std::cerr << "outSize: " << std::dec << (uint)_fileEntries[_fileIds[i]].outSize << std::endl;
      std::cerr << "---------------" << std::endl;
    }
  }
}
