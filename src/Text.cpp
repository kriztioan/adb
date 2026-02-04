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
  auto field_end = record.end();

  std::string_view type;
  auto field_it = record["type"];
  if (field_it != field_end) {
    type = field_it->second;
  }

  std::string authors;
  field_it = record["author"];
  if (field_it != field_end) {
    authors = Coders::LaTeXDecode(field_it->second);
  }

  std::string::size_type begin = 0, end;

  bool had_first = false;

  std::string author;
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
    } else {
      last = author;
    }

    if (!last.empty()) {
      if (had_first) {
        ostr << ", ";
      } else {
        had_first = true;
      }
      ostr << last;
    }

    if (!first.empty()) {
      if (!last.empty() || had_first) {
        ostr << ", ";
      } else {
        had_first = true;
      }
      ostr << first;
      if (!middle.empty()) {
        ostr << middle;
      }
    }
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
    } else {
      last = author;
    }

    if (!last.empty()) {
      if (had_first) {
        ostr << ", ";
      } else {
        had_first = true;
      }
      ostr << last;
    }

    if (!first.empty()) {
      if (!last.empty() || had_first) {
        ostr << ", ";
      } else {
        had_first = true;
      }
      ostr << first;
      if (!middle.empty()) {
        ostr << middle;
      }
    }
  }

  field_it = record["title"];
  if (field_it != field_end) {
    ostr << ", \"" << Coders::LaTeXDecode(field_it->second) << "\"";
  }

  std::string journal;
  if (type == "ARTICLE") {
    field_it = record["journal"];
    if (field_it != field_end) {
      journal = Coders::LaTeXDecode(field_it->second);
      auto prefs_it = setup.prefs["translate"];
      if (prefs_it != setup.prefs.end() && prefs_it->second == "true") {
        if (setup.strings.mFields.size()) {
          auto strings_it = setup.strings[journal];
          if (strings_it != setup.strings.end()) {
            journal = strings_it->second;
          }
        }
      }
    }
  } else if (type == "PHDTHESIS") {
    journal = "PhD Thesis";
  }

  if (!journal.empty()) {
    ostr << ", " << journal;
  }

  if (type == "INPROCEEDINGS") {
    field_it = record["booktitle"];
    if (field_it != field_end) {
      ostr << " in \"" << Coders::LaTeXDecode(field_it->second) << "\"";
    }

    field_it = record["series"];
    if (field_it != field_end) {
      ostr << ", " << Coders::LaTeXDecode(field_it->second);
    }
  }

  field_it = record["year"];
  if (field_it != field_end) {
    ostr << ", " << Coders::LaTeXDecode(record.mFields["year"]);
  }

  if (type == "BOOK" || type == "INPROCEEDINGS") {
    field_it = record["publisher"];
    if (field_it != field_end) {
      ostr << ", " << Coders::LaTeXDecode(field_it->second) << "\n";
    }
  }

  std::string editors;
  field_it = record["editor"];
  if (field_it != field_end) {
    editors = Coders::LaTeXDecode(field_it->second);
  }

  std::string editor;
  if (!editors.empty()) {
    had_first = false;
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
      } else {
        last = editor;
      }

      if (!last.empty()) {
        ostr << ", ";
        if (!had_first) {
          ostr << "eds. ";
          had_first = true;
        }
        ostr << last;
      }

      if (!first.empty()) {
        ostr << ", ";
        if (!had_first) {
          ostr << "eds. ";
          had_first = true;
        }
        ostr << first;
        if (!middle.empty()) {
          ostr << middle;
        }
      }
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
      } else {
        last = editor;
      }

      if (!last.empty()) {
        ostr << ", ";
        if (!had_first) {
          ostr << "eds. ";
          had_first = true;
        }
        ostr << last;
      }

      if (!first.empty()) {
        ostr << ", ";
        if (!had_first) {
          ostr << "eds. ";
          had_first = true;
        }
        ostr << first;
        if (!middle.empty()) {
          ostr << middle;
        }
      }
    }
  }

  field_it = record["volume"];
  if (field_it != field_end) {
    ostr << ", " << Coders::LaTeXDecode(field_it->second);
  }

  field_it = record["number"];
  if (field_it != field_end) {
    ostr << ", " << Coders::LaTeXDecode(field_it->second);
  }

  field_it = record["pages"];
  if (field_it != field_end) {
    ostr << ", " << Coders::LaTeXDecode(field_it->second);
  }

  field_it = record["doi"];
  if (field_it != field_end) {
    ostr << " doi:" << Coders::LaTeXDecode(field_it->second);
  }

  ostr << "\n\n";

  return (true);
}
