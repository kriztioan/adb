/**
 *  @file   Text.cpp
 *  @brief  Text Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Text.h"

bool Text::Export(Record &record, std::ostream &ostr, Text::Setup &setup) {

  // get type
  std::string type(record.mFields["type"]);

  std::string authors(Coders::LaTeXDecode(record.mFields["author"])), author;

  std::string::size_type begin = 0, end;

  while ((end = authors.find(" and ", begin)) != std::string::npos) {
    author = authors.substr(begin, end - begin);
    begin = end + 5;

    std::string last, middle, first;

    std::string::size_type idx = author.find(","), at;
    if (idx != std::string::npos) {
      last = author.substr(0, idx);
      at = idx;
      idx = author.find(",", idx + 1);
      if (idx != std::string::npos) {
        last = author.substr(0, idx);
        at = idx;
      }
      idx = author.find(".", at);
      if (idx != std::string::npos) {
        first = author.substr(idx - 1, 2);
        if (idx != author.length())
          middle = author.substr(idx + 1);
      }
    } else
      last = author;

    ostr << last << ", " << first << middle << ", ";
  }

  if (!authors.empty()) {

    author = authors.substr(begin);

    std::string last, middle, first;

    std::string::size_type idx = author.find(","), at;
    if (idx != std::string::npos) {
      last = author.substr(0, idx);
      at = idx;
      idx = author.find(",", idx + 1);
      if (idx != std::string::npos) {
        last = author.substr(0, idx);
        at = idx;
      }
      idx = author.find(".", at);
      if (idx != std::string::npos) {
        first = author.substr(idx - 1, 2);
        if (idx != author.length())
          middle = author.substr(idx + 1);
      }
    } else
      last = author;

    ostr << last << ", " << first << middle << ", ";
  }

  ostr << '\"' << Coders::LaTeXDecode(record.mFields["title"]) << "\", "
       << record.mFields["year"] << ", ";

  if (type == "ARTICLE") {

    std::string journal(Coders::LaTeXDecode(record.mFields["journal"]));
    if (type == "PHDTHESIS")
      journal = "PhD Thesis";

    if (type == "ARTICLE" && setup.prefs.mFields["translate"] == "true") {

      if (setup.strings.mFields.size()) {
        auto it = setup.strings.mFields.find(journal);
        if (it != setup.strings.mFields.end()) {
          journal = it->second;
        }
      }
    }
    ostr << journal << ", ";
  }

  if (type == "INPROCEEDINGS") {
    std::string booktitle(record.mFields["booktit]e"]),
        series(record.mFields["series"]);

    if (!booktitle.empty())
      ostr << "in proceedings \"" << Coders::LaTeXDecode(booktitle) << "\", ";

    if (!series.empty())
      ostr << Coders::LaTeXDecode(series);
  }

  if (type == "BOOK" || type == "INPROCEEDINGS") {
    std::string publisher(record.mFields["publish"]);
    if (!publisher.empty())
      ostr << Coders::LaTeXDecode(publisher) << "\n";
  }

  std::string editors(Coders::LaTeXDecode(record.mFields["editor"])), editor;

  if (editors.length() != 0) {

    begin = 0;
    while ((end = editors.find(" and ", begin)) != std::string::npos) {
      editor = editors.substr(begin, end - begin);
      begin = end + 5;

      std::string last, middle, first;

      std::string::size_type idx = editor.find(","), at;
      if (idx != std::string::npos) {
        last = editor.substr(0, idx);
        at = idx;
        idx = editor.find(",", idx + 1);
        if (idx != std::string::npos) {
          last = editor.substr(0, idx);
          at = idx;
        }
        idx = editor.find(".", at);
        if (idx != std::string::npos) {
          first = editor.substr(idx - 1, 2);
          at = idx;
          if (idx != editor.length())
            middle = editor.substr(idx + 1);
        }
      } else
        last = editor;

      ostr << last << ", " << first << middle << ", ";
    }

    if (!editors.empty()) {
      editor = editors.substr(begin);

      std::string last, middle, first;

      std::string::size_type idx = editor.find(","), at;
      if (idx != std::string::npos) {
        last = editor.substr(0, idx);
        at = idx;
        idx = editor.find(",", idx + 1);
        if (idx != std::string::npos) {
          last = editor.substr(0, idx);
          at = idx;
        }
        idx = editor.find(".", at);
        if (idx != std::string::npos) {
          first = editor.substr(idx - 1, 2);
          at = idx;
          if (idx != editor.length())
            middle = editor.substr(idx + 1);
        }
      } else
        last = editor;

      ostr << last << ", " << first << middle << ", ";
    }
  }

  std::string volume(record.mFields["volume"]), pages(record.mFields["pages"]);

  if (!volume.empty())
    ostr << Coders::LaTeXDecode(volume) << ", ";

  if (!pages.empty())
    ostr << Coders::LaTeXDecode(pages);

  std::string doi(record.mFields["doi"]);
  if (!doi.empty())
    ostr << " doi:" << Coders::LaTeXDecode(doi);

  ostr << "\n\n";
  return (true);
}
