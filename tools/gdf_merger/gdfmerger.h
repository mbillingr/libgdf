#ifndef GDFMERGER_H
#define GDFMERGER_H

//---------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include <iostream>

#include "GDF/Reader.h"
#include "GDF/Writer.h"

//---------------------------------------------------------------------------------------

class gdfMerger
{
  public:
    gdfMerger(const std::vector<std::string>& inputs,
              const std::string&);
    ~gdfMerger();
    void merge();

  private:

    const std::vector<std::string>& input_files_;
    const std::string& output_file_;

    gdf::Reader reader_;
    gdf::Writer writer_;

    gdf::uint32 event_offset_;

};

//---------------------------------------------------------------------------------------

#endif // GDFMERGER_H
