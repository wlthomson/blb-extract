#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace Blb {

  static const uint32_t HEADER_ID_1 = 0x02004940;
  static const uint16_t HEADER_ID_2 = 0x0007;

  static const uint8_t FILE_TYPE_SOUND = 0x07;
  static const uint8_t FILE_TYPE_MUSIC = 0x08;

  static const uint8_t COMPR_TYPE_NONE = 0x01;

  static const uint8_t DW_SHIFT_DEFAULT = 0xFF;

  struct BlbHeader {
    uint32_t id1;
    uint16_t id2;
    uint16_t shiftSize;
    uint32_t archiveSize;
    uint32_t fileCount;
  };

  struct BlbFileEntry {
    uint8_t type;
    uint8_t compr;
    uint8_t shiftVal;
    uint32_t timeStamp;
    uint32_t offset;
    uint32_t diskSize;
    uint32_t outSize;
  };

  class BlbFileBuffer {
  public:
    BlbFileBuffer(const char* in, uint32_t size);
    BlbFileBuffer(const BlbFileBuffer* fileBuffer);
    void read(char* out, uint32_t size);
    void write(const char* in, uint32_t size);

    char* data;
    uint32_t size;
  private:
    char* _ptrRead;
    char* _ptrWrite;
  };

  class BlbFile {
  public:
    BlbFile(const BlbFileBuffer* fileBuffer, uint32_t fileId, BlbFileEntry fileEntry);
    ~BlbFile();
    void extract();
  private:
    uint32_t _fileId;
    BlbFileBuffer* _fileBuffer;
  };

  class BlbArchive {
  public:
    BlbArchive(std::string filename);
    ~BlbArchive();
    void debug();
    void extractFile(uint32_t fileId);
    void extractAudio();
  private:
    std::string _filename;
    std::ifstream _fs;
    BlbHeader _header;
    std::vector<uint32_t> _fileIds;
    std::unordered_map<uint32_t, BlbFileEntry> _fileEntries;
  };
}
