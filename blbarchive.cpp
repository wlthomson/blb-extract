#include "blbarchive.h"

namespace Blb {

  BlbFile::BlbFile(const BlbFileBuffer* fileBuffer, uint32_t fileId, BlbFileEntry fileEntry) {
    // TODO: handle image files
    // TODO: handle video files

    _fileId = fileId;
    _fileBuffer = new BlbFileBuffer(fileBuffer);

    // TODO: handle PKWARE compressed files

    if (fileEntry.shiftVal != DW_SHIFT_DEFAULT) {
      uint32_t fileSizeCompressed = _fileBuffer->size;
      uint32_t fileSizeDecompressed = fileSizeCompressed * 2;

      char* bufferTemp = (char*)malloc(fileSizeDecompressed);

      int16_t currVal = 0;
      for (uint32_t i = 0; i < fileSizeCompressed; i++) {
	currVal += (int8_t)_fileBuffer->data[i];
	((int16_t*)bufferTemp)[i] = currVal << (int8_t)fileEntry.shiftVal;
      }

      _fileBuffer->data = bufferTemp;
      _fileBuffer->size = fileSizeDecompressed;
    }
  }

  BlbFile::~BlbFile() {
    delete _fileBuffer;
  }

  void BlbFile::extract() {
    std::stringstream sstream;
    sstream << std::hex << _fileId;

    std::ofstream fileOut;
    fileOut.open(sstream.str() + ".raw", std::ios::binary);
    fileOut.write(_fileBuffer->data, _fileBuffer->size);
  }

  BlbFileBuffer::BlbFileBuffer(const char* bufferData, uint32_t bufferSize) {
    data = (char*)malloc(bufferSize);
    memcpy(data, bufferData, bufferSize);
    size = bufferSize;

    _ptrRead = data;
    _ptrWrite = data;
  }

  BlbFileBuffer::BlbFileBuffer(const BlbFileBuffer* fileBuffer) {
    data = (char*)malloc(fileBuffer->size);
    memcpy(data, fileBuffer->data, fileBuffer->size);
    size = fileBuffer->size;

    _ptrRead = data;
    _ptrWrite = data;
  }

  void BlbFileBuffer::read(char* out, uint32_t size) {
    memcpy(out, _ptrRead, size);
    _ptrRead += size;
  }

  void BlbFileBuffer::write(const char* in, uint32_t size) {
    memcpy(_ptrWrite, in, size);
    _ptrWrite += size;
  }

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

void BlbArchive::extractFile(uint32_t fileId) {
    BlbFileEntry fileEntry = _fileEntries[fileId];

    char* bufferData = (char*)malloc(fileEntry.diskSize);
    uint32_t bufferSize = fileEntry.diskSize;

    _fs.seekg(fileEntry.offset, std::ios::beg);
    _fs.read(bufferData, bufferSize);
    _fs.seekg(0, std::ios::beg);

    BlbFileBuffer* fileBuffer = new BlbFileBuffer(bufferData, bufferSize);

    BlbFile file(fileBuffer, fileId, fileEntry);
    file.extract();

    free(fileBuffer);
  }

  void BlbArchive::extractAudio() {
    for (uint i = 0; i < _header.fileCount; i++) {
      BlbFileEntry entry = _fileEntries[_fileIds[i]];
      if (entry.type == FILE_TYPE_SOUND || entry.type == FILE_TYPE_MUSIC) {
	// TODO: handle PKWARE compressed audio

	if (entry.compr == COMPR_TYPE_NONE) {
	  uint32_t fileId = _fileIds[i];
	  extractFile(fileId);
	}
      }
    }
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
