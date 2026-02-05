/**
 *  @file   MSWord.cpp
 *  @brief  MSWord Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "MSWord.h"

bool MSWord::Export(Record &record, std::ostream &ostr, Setup &setup) {

  static std::unordered_map<std::string, std::string> types;

  types["ARTICLE"] = "JournalArticle";
  types["INPROCEEDINGS"] = "ConferenceProceedings";
  types["BOOK"] = "Book";

  auto field_end = record.end();
  std::string type;
  auto field_it = record["type"];
  if (field_it != field_end) {
    type = field_it->second;
    std::transform(type.begin(), type.end(), type.begin(),
                   [](unsigned char c) { return std::toupper(c); });
  }

  if (types.find(type) != types.end()) {
    type = types[type];
  }

  auto prefs_end = setup.prefs.end();
  std::string tag;
  auto prefs_it = setup.prefs["key"];
  if (prefs_it != prefs_end) {
    field_it = record[prefs_it->second];
    if (field_it != field_end) {
      tag = Coders::HTML2XML(Coders::LaTeXDecode(field_it->second));
    }
  }

  UUID uuid;
  Uuid::UUIDFromURL(&uuid, tag);

  ostr << "<b:Source>\n"
          "<b:SourceType>"
       << type
       << "</b:SourceType>\n"
          "<b:Guid>{"
       << uuid.ms
       << "}</b:Guid>\n"
          "<b:LCID>"
       << 1033
       << "</b:LCID>\n"
          "<b:Tag>"
       << tag
       << "</b:Tag>\n"
          "<b:Title>";
  field_it = record["title"];
  if (field_it != field_end) {
    ostr << Coders::HTML2XML(Coders::LaTeXDecode(field_it->second));
  }
  ostr << "</b:Title>\n";
  field_it = record["year"];
  if (field_it != field_end) {
    ostr << "<b:Year>" << Coders::LaTeXDecode(field_it->second);
  }
  ostr << "</b:Year>\n"
          "<b:Author>\n"
          "<b:Author>\n"
          "<b:NameList>\n";

  std::string authors;
  field_it = record["author"];
  if (field_it != field_end) {
    authors = Coders::HTML2XML(Coders::LaTeXDecode(field_it->second));
  }

  std::string::size_type begin = 0, end;

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

    ostr << "<b:Person>\n"
            "<b:Last>";
    if (!last.empty()) {
      ostr << last;
    }
    ostr << "</b:Last>\n" << "<b:Middle>";
    if (!middle.empty()) {
      ostr << middle;
    }
    ostr << "</b:Middle>\n"
            "<b:First>";
    if (!first.empty()) {
      ostr << first;
    }
    ostr << "</b:First>\n"
            "</b:Person>\n";
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

    ostr << "<b:Person>\n"
            "<b:Last>";
    if (!last.empty()) {
      ostr << last;
    }
    ostr << "</b:Last>\n" << "<b:Middle>";
    if (!middle.empty()) {
      ostr << middle;
    }
    ostr << "</b:Middle>\n"
            "<b:First>";
    if (!first.empty()) {
      ostr << first;
    }
    ostr << "</b:First>\n"
            "</b:Person>\n";
  }

  ostr << "</b:NameList>\n"
          "</b:Author>\n";

  std::string editors;
  field_it = record["editor"];
  if (field_it != field_end) {
    editors = Coders::HTML2XML(Coders::LaTeXDecode(field_it->second));
  }

  if (editors.length() != 0) {
    ostr << "<b:Editor>\n"
            "<b:NameList>\n";

    begin = 0;

    std::string editor;
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

      ostr << "<b:Person>\n"
              "<b:Last>";
      if (!last.empty()) {
        ostr << last;
      }
      ostr << "</b:Last>\n" << "<b:Middle>";
      if (!middle.empty()) {
        ostr << middle;
      }
      ostr << "</b:Middle>\n"
           << "<b:First>";
      if (!first.empty()) {
        ostr << first;
      }
      ostr << "</b:First>\n"
              "</b:Person>\n";
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
      ostr << "<b:Person>\n"
              "<b:Last>";
      if (!last.empty()) {
        ostr << last;
      }
      ostr << "</b:Last>\n" << "<b:Middle>";
      if (!middle.empty()) {
        ostr << middle;
      }
      ostr << "</b:Middle>\n"
              "<b:First>";
      if (!first.empty()) {
        ostr << first;
      }
      ostr << "</b:First>\n"
              "</b:Person>\n";
    }
    ostr << "</b:NameList>\n"
            "</b:Editor>\n";
  }
  ostr << "</b:Author>\n"
          "<b:Pages>"
       << Coders::LaTeXDecode(record.mFields["pages"])
       << "</b:Pages>\n"
          "<b:Volume>"
       << Coders::LaTeXDecode(record.mFields["volume"]) << "</b:Volume>\n";

  if (type == "ARTICLE") {

    std::string journal;
    field_it = record["journal"];
    if (field_it != field_end) {
      journal = Coders::LaTeXDecode(field_it->second);
    }

    if (type == "PHDTHESIS") {
      journal = "PhD Thesis";
    }

    prefs_it = setup.prefs["translate"];
    if (type == "ARTICLE" && prefs_it != prefs_end &&
        prefs_it->second == "true") {
      if (setup.strings.mFields.size()) {
        auto strings_it = setup.strings[journal];
        if (strings_it != setup.strings.end()) {
          journal = strings_it->second;
        }
      }
    }

    ostr << "<b:JournalName>" << Coders::HTML2XML(journal)
         << "</b:JournalName>\n";
  }

  if (type == "INPROCEEDINGS") {
    ostr << "<b:BookTitle>";
    field_it = record["booktitle"];
    if (field_it != field_end) {
      ostr << Coders::HTML2XML(Coders::LaTeXDecode(field_it->second));
    }
    ostr << "</b:BookTitle>\n"
            "<b:ConferenceName>";
    field_it = record["series"];
    if (field_it != field_end) {
      ostr << Coders::HTML2XML(Coders::LaTeXDecode(field_it->second));
    }
    ostr << "</b:ConferenceName>\n";
  }

  if (type == "BOOK") {
    ostr << "<b:Publisher>";
    field_it = record["publisher"];
    if (field_it != field_end) {
      ostr << Coders::LaTeXDecode(field_it->second);
    }
    ostr << "</b:Publisher>\n";
  }

  ostr << "<b:Month>";
  field_it = record["month"];
  if (field_it != field_end) {
    ostr << Coders::LaTeXDecode(field_it->second);
  }
  ostr << "</b:Month>\n"
          "</b:Source>\n";

  return (true);
}

void MSWord::Header(std::ostream &ostr) {

  ostr << "<?xml version=\"1.0\"?>\n"
          "<b:Sources SelectedStyle=\"\" "
          "xmlns:b=\"http://schemas.openxmlformats.org/officeDocument/2006/"
          "bibliography\" "
          "xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/"
          "bibliography\">\n";
}

void MSWord::Footer(std::ostream &ostr) { ostr << "</b:Sources>\n"; }
