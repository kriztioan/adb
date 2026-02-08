/**
 *  @file   MSWord.cpp
 *  @brief  MSWord Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "MSWord.h"

bool MSWord::Export(Record &record, std::ostream &ostr, ExportContext &ctx) {

  static std::unordered_map<std::string, std::string> types;

  types["ARTICLE"] = "JournalArticle";
  types["BOOK"] = "Book";
  types["INBOOK"] = "BookSection";
  types["INCOLLECTION"] = "Book";
  types["INPROCEEDINGS"] = "ConferenceProceedings";
  types["PHDTHESIS"] = "Report";
  types["PROCEEDINGS"] = "ConferenceProceedings";

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
  } else {
    type = "Misc";
  }

  std::string tag;
  field_it = record[ctx.key];
  if (field_it != field_end) {
    tag = Encoding::HTML2XML(Encoding::LaTeXDecode(field_it->second, ctx.pool));
  }

  UUID uuid;
  Uuid::UUIDFromURL(&uuid, tag);

  ostr << "\n<b:Source>"
          "\n<b:SourceType>"
       << type
       << "</b:SourceType>"
          "\n<b:Guid>{"
       << uuid.ms
       << "}</b:Guid>"
          "\n<b:LCID>"
       << 1033
       << "</b:LCID>"
          "\n<b:Tag>"
       << tag << "</b:Tag>";

  field_it = record["title"];
  if (field_it != field_end) {
    ostr << "\n<b:Title>"
         << Encoding::HTML2XML(
                Encoding::LaTeXDecode(field_it->second, ctx.pool))
         << "</b:Title>";
  }
  field_it = record["year"];
  if (field_it != field_end) {
    ostr << "\n<b:Year>" << Encoding::LaTeXDecode(field_it->second, ctx.pool)
         << "</b:Year>";
  }
  ostr << "\n<b:Author>"
          "\n<b:Author>"
          "\n<b:NameList>";

  std::string authors;
  field_it = record["author"];
  if (field_it != field_end) {
    authors =
        Encoding::HTML2XML(Encoding::LaTeXDecode(field_it->second, ctx.pool));
  }

  std::string::size_type begin = 0, end;

  std::string author;
  while ((end = authors.find(" and ", begin)) != std::string::npos) {
    author = authors.substr(begin, end - begin);
    begin = end + 5;

    std::string last, middle, first;

    std::string::size_type i = author.find(", "), j;
    if (i != std::string::npos) {
      last = author.substr(0, i);
      j = i + 2;
      i = author.find(", ", j);
      if (i != std::string::npos) {
        last = author.substr(0, i);
        j = i + 2;
      }
      i = author.find(".", j);
      if (i != std::string::npos) {
        first = author.substr(j, i - j + 1);
        if (i != author.length())
          middle = author.substr(i + 1);
      }
    } else {
      last = author;
    }

    ostr << "\n<b:Person>";
    if (!last.empty()) {
      ostr << "\n<b:Last>" << last << "</b:Last>";
    }
    if (!middle.empty()) {
      ostr << "\n<b:Middle>" << middle << "</b:Middle>";
    }
    if (!first.empty()) {
      ostr << "\n<b:First>" << first << "</b:First>";
    }
    ostr << "\n</b:Person>";
  }

  if (!authors.empty()) {

    author = authors.substr(begin);

    std::string last, middle, first;

    std::string::size_type i = author.find(", "), j;
    if (i != std::string::npos) {
      last = author.substr(0, i);
      j = i + 2;
      i = author.find(", ", j);
      if (i != std::string::npos) {
        last = author.substr(0, i);
        j = i + 2;
      }
      i = author.find(".", j);
      if (i != std::string::npos) {
        first = author.substr(j, i - j + 1);
        if (i != author.length())
          middle = author.substr(i + 1);
      }
    } else {
      last = author;
    }

    ostr << "\n<b:Person>";
    if (!last.empty()) {
      ostr << "\n<b:Last>" << last << "</b:Last>";
    }
    if (!middle.empty()) {
      ostr << "\n<b:Middle>" << middle << "</b:Middle>";
    }
    if (!first.empty()) {
      ostr << "\n<b:First>" << first << "</b:First>";
    }
    ostr << "\n</b:Person>";
  }

  ostr << "\n</b:NameList>"
          "\n</b:Author>";

  std::string editors;
  field_it = record["editor"];
  if (field_it != field_end) {
    editors =
        Encoding::HTML2XML(Encoding::LaTeXDecode(field_it->second, ctx.pool));
  }

  if (editors.length() != 0) {
    ostr << "\n<b:Editor>"
            "\n<b:NameList>";

    begin = 0;

    std::string editor;
    while ((end = editors.find(" and ", begin)) != std::string::npos) {
      editor = editors.substr(begin, end - begin);
      begin = end + 5;

      std::string last, middle, first;

      std::string::size_type i = editor.find(", "), j;
      if (i != std::string::npos) {
        last = editor.substr(0, i);
        j = i + 2;
        i = editor.find(", ", j);
        if (i != std::string::npos) {
          last = editor.substr(0, i);
          j = i + 2;
        }
        i = editor.find(".", j);
        if (i != std::string::npos) {
          first = editor.substr(j, i - j + 1);
          if (i != editor.length())
            middle = editor.substr(i + 1);
        }
      } else {
        last = editor;
      }

      ostr << "\n<b:Person>";
      if (!last.empty()) {
        ostr << "\n<b:Last>" << last << "</b:Last>";
      }
      if (!middle.empty()) {
        ostr << "\n<b:Middle>" << middle << "</b:Middle>\n";
      }
      if (!first.empty()) {
        ostr << "\n<b:First>" << first << "</b:First>";
      }
      ostr << "\n</b:Person>";
    }

    if (!editors.empty()) {
      editor = editors.substr(begin);

      std::string last, middle, first;

      std::string::size_type i = editor.find(", "), j;
      if (i != std::string::npos) {
        last = editor.substr(0, i);
        j = i + 2;
        i = editor.find(", ", j);
        if (i != std::string::npos) {
          last = editor.substr(0, i);
          j = i + 2;
        }
        i = editor.find(".", j);
        if (i != std::string::npos) {
          first = editor.substr(j, i - j + 1);
          if (i != editor.length())
            middle = editor.substr(i + 1);
        }
      } else {
        last = editor;
      }

      ostr << "\n<b:Person>";
      if (!last.empty()) {
        ostr << "\n<b:Last>" << last << "</b:Last>";
      }
      if (!middle.empty()) {
        ostr << "\n<b:Middle>" << middle << "</b:Middle>";
      }
      if (!first.empty()) {
        ostr << "<b:First>" << first << "</b:First>";
      }
      ostr << "\n</b:Person>";
    }
    ostr << "\n</b:NameList>"
            "\n</b:Editor>";
  }
  ostr << "\n</b:Author>";

  field_it = record["pages"];
  if (field_it != field_end) {
    ostr << "\n<b:Pages>" << Encoding::LaTeXDecode(field_it->second, ctx.pool)
         << "</b:Pages>";
  }

  field_it = record["volume"];
  if (field_it != field_end) {
    ostr << "\n<b:Volume>" << Encoding::LaTeXDecode(field_it->second, ctx.pool)
         << "</b:Volume>";
  }

  if (type == "JournalArticle") {
    std::string_view journal;
    field_it = record["journal"];
    if (field_it != field_end) {
      if (ctx.strings.mFields.size() != 0) {
        auto strings_it = ctx.strings[field_it->second];
        if (strings_it != ctx.strings.end()) {
          journal = strings_it->second;
        }
      }
      ostr << "\n<b:JournalName>"
           << Encoding::HTML2XML(Encoding::LaTeXDecode(journal, ctx.pool))
           << "</b:JournalName>";
    }
  }

  field_it = record["booktitle"];
  if (field_it != field_end) {
    ostr << "\n<b:BookTitle>"
         << Encoding::HTML2XML(
                Encoding::LaTeXDecode(field_it->second, ctx.pool))
         << "</b:BookTitle>";
  }

  field_it = record["series"];
  if (field_it != field_end) {
    ostr << "\n<b:ConferenceName>"
         << Encoding::HTML2XML(
                Encoding::LaTeXDecode(field_it->second, ctx.pool))
         << "</b:ConferenceName>";
  }

  field_it = record["publisher"];
  if (field_it != field_end) {
    ostr << "\n<b:Publisher>"
         << Encoding::HTML2XML(
                Encoding::LaTeXDecode(field_it->second, ctx.pool))
         << "</b:Publisher>";
  }

  field_it = record["month"];
  if (field_it != field_end) {
    ostr << "\n<b:Month>" << Encoding::LaTeXDecode(field_it->second, ctx.pool)
         << "</b:Month>";
  }

  ostr << "\n</b:Source>";

  return (true);
}

void MSWord::Header(std::ostream &ostr) {

  ostr << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
          "\n<b:Sources SelectedStyle=\"\" "
          "xmlns:b=\"http://schemas.openxmlformats.org/officeDocument/"
          "2006/"
          "bibliography\" "
          "xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/"
          "bibliography\">";
}

void MSWord::Footer(std::ostream &ostr) { ostr << "</b:Sources>"; }
