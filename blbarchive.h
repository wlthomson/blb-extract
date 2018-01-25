#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace Blb {

  static const uint32_t HEADER_ID_1 = 0x02004940;
  static const uint16_t HEADER_ID_2 = 0x0007;

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

  class BlbArchive {
  public:
    BlbArchive(std::string filename);
    ~BlbArchive();
    void debug();
  private:
    std::string _filename;
    std::ifstream _fs;
    BlbHeader _header;
    std::vector<uint32_t> _fileIds;
    std::unordered_map<uint32_t, BlbFileEntry> _fileEntries;
  };
}
