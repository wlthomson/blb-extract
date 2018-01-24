#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>

namespace Blb {

  static const uint32_t HEADER_ID_1 = 0x02004940;
  static const uint16_t HEADER_ID_2 = 0x0007;

  struct BlbHeader {
    uint32_t id1;
    uint16_t id2;
    uint16_t shiftSize;
    uint32_t archiveSize;
    uint32_t fileCount;
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
  };
}
