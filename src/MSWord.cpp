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
      tag(Coders::HTML2XML(Coders::LaTeXDecode(
          record.mFields.at(setup.prefs.mFields.at("key")))));

  std::transform(type.begin(), type.end(), type.begin(),
                 [](unsigned char c) { return std::toupper(c); });

  GUID guid;
  GUIDCreate(&guid, tag);

  ostr << "<b:Source>\n"
       << "<b:SourceType>" << types[type] << "</b:SourceType>\n"
       << "<b:Guid>{" << guid.ms << "}</b:Guid>\n"
       << "<b:LCID>" << 1033 << "</b:LCID>\n"
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

MSWord::GUID *MSWord::GUIDCreate(MSWord::GUID *guid, const std::string &value) {

  EVP_MD_CTX *ctx = EVP_MD_CTX_new();

  if (ctx == NULL) {

    return nullptr;
  }

  const EVP_MD *md = EVP_md5();

  EVP_DigestInit(ctx, md);

  uint8_t ns[] = {0x6b, 0xa7, 0xb8, 0x11, 0x9d, 0xad, 0x11, 0xd1,
                  0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8};

  EVP_DigestUpdate(ctx, ns, sizeof(ns));

  EVP_DigestUpdate(ctx, value.data(), value.length());

  uint32_t md_len;

  EVP_DigestFinal(ctx, guid->uuid, &md_len);

  EVP_MD_CTX_free(ctx);

  // version 3
  guid->uuid[6] = (guid->uuid[6] & 0x0F) | 0x30;

  // variant URL
  guid->uuid[8] = (guid->uuid[8] & 0x3F) | 0x80;

  snprintf(guid->ms, GUID_MS_LENGTH,
           "%02X%02X%02X%02X-"
           "%02X%02X-"
           "%02X%02X-"
           "%02X%02X-"
           "%02X%02X%02X%02X%02X%02X",
           guid->uuid[0], guid->uuid[1], guid->uuid[2], guid->uuid[3],
           guid->uuid[4], guid->uuid[5], guid->uuid[6], guid->uuid[7],
           guid->uuid[8], guid->uuid[9], guid->uuid[10], guid->uuid[11],
           guid->uuid[12], guid->uuid[13], guid->uuid[14], guid->uuid[15]);

  return guid;
}
