#include "blbarchive.h"

int main(int argc, char **argv) {
  try {
    Blb::BlbArchive a("a.blb");
    // Blb::BlbArchive c("c.blb");
    // Blb::BlbArchive hd("hd.blb");
    // Blb::BlbArchive i("i.blb");
    // Blb::BlbArchive m("m.blb");
    // Blb::BlbArchive s("s.blb");
    // Blb::BlbArchive t("t.blb");

    // a.debug();
    // c.debug();
    // hd.debug();
    // i.debug();
    // m.debug();
    // s.debug();
    // t.debug();

    a.extractAudio(); // a.blb contains all NHOOD audio
  } catch (std::invalid_argument& e) {
    std::cout << e.what() << std::endl;
  }
}
