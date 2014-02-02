#include <iostream>

#include "poppler/cpp/poppler-document.h"
#include "poppler/cpp/poppler-page.h"
#include "poppler/cpp/poppler-rectangle.h"
#include "poppler/cpp/poppler-global.h"

#include "parseargs.h"

static int dpi = 100;
static int firstPage = -1;
static GBool reverse = gFalse;
static GBool printHelp = gFalse;
static GBool printVersion = gFalse;

static const ArgDesc argDesc[] = {
  {"-d", argInt, &dpi, 0,
   "dpi for pixel calculation, default 100"},
  {"-f", argInt, &firstPage, 0,
   "first page of search"},
  {"-r", argFlag, &reverse, 0,
   "reverse search"},
  {"-v", argFlag, &printVersion, 0,
   "print copyright and version info"},
  {"-h", argFlag, &printHelp, 0,
   "print usage information"},
  {NULL}
};

using namespace poppler;


void output_rect(int page, rectf box) {
  double m = dpi / 72.0f;
  std::cout << page << ","
    // in points (1pt = 1/72 inch)
            << box.x() << "," << box.y() << ","
            << box.width() << "," << box.height() << ","
    // in pixel
            << (m*box.x()) << "," << (m*box.y()) << ","
            << (m*box.width()) << "," << (m*box.height()) << std::endl;
}

int main(int argc, char * argv[]) {
  GBool ok = parseArgs(argDesc, &argc, argv);
  if (printVersion) {  
    std::cout << "VERSION" << std::endl;
    return 0;
  } else if (!ok or argc != 3 or printHelp) {
    printUsage(argv[0], "<filename> <search-term>", argDesc);
    std::cerr << std::endl << "The output format is:" << std::endl;
    std::cerr << "  <page-no>,<x>,<y>,<width>,<height>,"
              << "<x_px>,<y_px>,<width_px>,<height_px>" << std::endl;
    std::cerr << std::endl << "for every match on a page. " 
              << argv[0] << " will search until at least one "
              << "match on a match is found." << std::endl;
    return 1;
  } else {
    std::string filename(argv[1]);
    ustring searchU = ustring::from_latin1(argv[2]);
    document * doc = document::load_from_file(filename);
    int no_pages = doc->pages();
    int i;
    if (firstPage == -1) {
      i = 0;
    } else {
      i = firstPage - 1;
    }
    while (i >= 0 and i < no_pages) {
      page * page = doc->create_page(i);
      rectf rect;

      bool foundOnPage = false;
      while (1) {
        if (page->search(searchU, 
                         rect, 
                         page::search_next_result,
                         case_insensitive)) {
          foundOnPage = true;
          output_rect((i+1), rect);
        } else {
          break;
        }
      }
      delete page;
      if (foundOnPage) break;
      // else swith to next/previous page
      if (reverse) {
        i--;
      } else {
        i++;
      }
    }
    delete doc;
    return 0;
  }
}
