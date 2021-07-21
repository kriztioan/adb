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

  std::string type(record.mFields.at("type")),
      tag(Coders::HTML2XML(
          Coders::LaTeXDecode(record.mFields.at(setup.prefs.mFields.at("key")))));

  // tag.erase(std::remove(tag.begin(), tag.end(), '.'), tag.end());
  // tag.erase(std::remove(tag.begin(), tag.end(), '&'), tag.end());

  for (size_t i = 0; i < type.length(); i++) {
    type[i] = (char)toupper(type[i]);
  }

  GUID guid;
  GUIDCreate(&guid);

  ostr << "<b:Source>\n"
       << "<b:SourceType>" << types[type] << "</b:SourceType>\n"
       << "<b:Guid>{" << guid.ms << "}</b:Guid>\n"
       << "<b:LCID>" << 0 << "</b:LCID>\n"
       << "<b:Tag>" << tag << "</b:Tag>\n"
       << "<b:Title>"
       << Coders::HTML2XML(Coders::LaTeXDecode(record.mFields["title"]))
       << "</b:Title>\n"
       << "<b:Year>" << Coders::LaTeXDecode(record.mFields["year"])
       << "</b:Year>\n"
       << "<b:Author>\n"
       << "<b:Author>\n"
       << "<b:NameList>\n";

  std::string authors(
      Coders::HTML2XML(Coders::LaTeXDecode(record.mFields["author"]))),
      author;

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
    } else {
      last = author;
    }

    ostr << "<b:Person>\n"
         << "<b:Last>" << last << "</b:Last>\n"
         << "<b:Middle>" << middle << "</b:Middle>\n"
         << "<b:First>" << first << "</b:First>\n"
         << "</b:Person>\n";
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
         << "<b:Last>" << last << "</b:Last>\n"
         << "<b:Middle>" << middle << "</b:Middle>\n"
         << "<b:First>" << first << "</b:First>\n"
         << "</b:Person>\n";
  }

  ostr << "</b:NameList>\n"
       << "</b:Author>\n";

  std::string editors(
      Coders::HTML2XML(Coders::LaTeXDecode(record.mFields["editor"]))),
      editor;

  if (editors.length() != 0) {
    ostr << "<b:Editor>\n"
         << "<b:NameList>\n";

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
      ostr << "<b:Person>\n"
           << "<b:Last>" << last << "</b:Last>\n"
           << "<b:Middle>" << middle << "</b:Middle>\n"
           << "<b:First>" << first << "</b:First>\n"
           << "</b:Person>\n";
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
           << "<b:Last>" << last << "</b:Last>\n"
           << "<b:Middle>" << middle << "</b:Middle>\n"
           << "<b:First>" << first << "</b:First>\n"
           << "</b:Person>\n";
    }
    ostr << "</b:NameList>\n"
         << "</b:Editor>\n";
  }
  ostr << "</b:Author>\n"
       << "<b:Pages>" << Coders::LaTeXDecode(record.mFields["pages"])
       << "</b:Pages>\n"
       << "<b:Volume>" << Coders::LaTeXDecode(record.mFields["volume"])
       << "</b:Volume>\n";

  if (type == "ARTICLE") {

    std::string journal(Coders::LaTeXDecode(record.mFields["journal"]));

    if (type == "PHDTHESIS") {
      journal = "PhD Thesis";
    }

    if (type == "ARTICLE" && setup.prefs.mFields["translate"] == "true") {
      if (setup.strings.mFields.size()) {
        auto it = setup.strings.mFields.find(journal);
        if (it != setup.strings.mFields.end()) {
          journal = it->second;
        }
      }
    }

    ostr << "<b:JournalName>" << Coders::HTML2XML(journal)
         << "</b:JournalName>\n";
  }

  if (type == "INPROCEEDINGS") {
    ostr << "<b:BookTitle>"
         << Coders::HTML2XML(Coders::LaTeXDecode(record.mFields["booktitle"]))
         << "</b:BookTitle>\n"
         << "<b:ConferenceName>"
         << Coders::HTML2XML(Coders::LaTeXDecode(record.mFields["series"]))
         << "</b:ConferenceName>\n";
  }

  if (type == "BOOK") {
    ostr << "<b:Publisher>" << Coders::LaTeXDecode(record.mFields["publisher"])
         << "</b:Publisher>\n";
  }

  ostr << "<b:Month>" << Coders::LaTeXDecode(record.mFields["month"])
       << "</b:Month>\n"
       << "</b:Source>\n";

  return (true);
}

void MSWord::Header(std::ostream &ostr) {

  ostr << "<?xml version=\"1.0\"?>\n"
       << "<b:Sources SelectedStyle=\"\" "
          "xmlns:b=\"http://schemas.openxmlformats.org/officeDocument/2006/"
          "bibliography\" "
          "xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/"
          "bibliography\">\n";
}

void MSWord::Footer(std::ostream &ostr) { ostr << "</b:Sources>\n"; }


MSWord::GUID *MSWord::GUIDCreate(MSWord::GUID *guid) {
  uuid_generate(guid->uuid);
  // uuid_unparse(guid->uuid, guid->str);
  snprintf(guid->ms, 37,
           "%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X",
           guid->data1, guid->data2, guid->data3, guid->data4[0],
           guid->data4[1], guid->data4[2], guid->data4[3], guid->data4[4],
           guid->data4[5], guid->data4[6], guid->data4[7]);
  return guid;
}
