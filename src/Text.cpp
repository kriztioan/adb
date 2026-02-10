/**
 *  @file   Text.cpp
 *  @brief  Text Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Text.h"

bool Text::Export(Record &record, std::ostream &ostr, ExportContext &ctx) {

  auto field_end = record.end();

  std::string authors;
  auto field_it = record["author"];
  if (field_it != field_end) {
    authors = Encoding::LaTeXDecode(field_it->second, ctx.pool);
  }

  std::string::size_type begin = 0, end;

  bool first = true;

  std::string author;
  while ((end = authors.find(" and ", begin)) != std::string::npos) {
    author = authors.substr(begin, end - begin);
    begin = end + 5;

    std::string last, middle, given;

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
        given = author.substr(j, i - j + 1);
        if (i != author.length())
          middle = author.substr(i + 1);
      }
    } else {
      last = author;
    }

    if (!last.empty()) {
      if (!first) {
        ostr << ", " << last;
      } else {
        ostr << last;
        first = false;
      }
    }

    if (!given.empty()) {
      if (!last.empty() || !first) {
        ostr << ", " << given;
      } else {
        ostr << given;
        first = true;
      }
      if (!middle.empty()) {
        ostr << middle;
      }
    }
  }

  if (!authors.empty()) {

    author = authors.substr(begin);

    std::string last, middle, given;

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
        given = author.substr(j, i - j + 1);
        if (i != author.length())
          middle = author.substr(i + 1);
      }
    } else {
      last = author;
    }

    if (!last.empty()) {
      if (!first) {
        ostr << ", " << last;
      } else {
        ostr << last;
        first = false;
      }
    }

    if (!given.empty()) {
      if (!last.empty() || !first) {
        ostr << ", " << given;
      } else {
        ostr << given;
        first = false;
      }
      if (!middle.empty()) {
        ostr << middle;
      }
    }
  }

  field_it = record["title"];
  if (field_it != field_end) {
    ostr << ", \"" << Encoding::LaTeXDecode(field_it->second, ctx.pool) << "\"";
  }

  std::string_view type;
  field_it = record["type"];
  if (field_it != field_end) {
    type = field_it->second;
  }

  std::string journal;
  if (type == "ARTICLE") {
    field_it = record["journal"];
    if (field_it != field_end) {
      journal = Encoding::LaTeXDecode(field_it->second, ctx.pool);
      if (ctx.strings.mFields.size()) {
        auto strings_it = ctx.strings[journal];
        if (strings_it != ctx.strings.end()) {
          journal = strings_it->second;
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
      ostr << " in \"" << Encoding::LaTeXDecode(field_it->second, ctx.pool)
           << "\"";
    }

    field_it = record["series"];
    if (field_it != field_end) {
      ostr << ", " << Encoding::LaTeXDecode(field_it->second, ctx.pool);
    }
  }

  field_it = record["year"];
  if (field_it != field_end) {
    ostr << ", " << Encoding::LaTeXDecode(record.mFields["year"], ctx.pool);
  }

  if (type == "BOOK" || type == "INPROCEEDINGS") {
    field_it = record["publisher"];
    if (field_it != field_end) {
      ostr << ", " << Encoding::LaTeXDecode(field_it->second, ctx.pool) << "\n";
    }
  }

  std::string editors;
  field_it = record["editor"];
  if (field_it != field_end) {
    editors = Encoding::LaTeXDecode(field_it->second, ctx.pool);
  }

  std::string editor;
  if (!editors.empty()) {
    first = true;
    begin = 0;
    while ((end = editors.find(" and ", begin)) != std::string::npos) {
      editor = editors.substr(begin, end - begin);
      begin = end + 5;

      std::string last, middle, given;

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
          given = editor.substr(j, i - j + 1);
          if (i != editor.length())
            middle = editor.substr(i + 1);
        }
      } else {
        last = editor;
      }

      if (!last.empty()) {
        ostr << ", ";
        if (!first) {
          ostr << last;
        } else {
          ostr << "eds. ";
          first = false;
        }
      }

      if (!given.empty()) {
        ostr << ", ";
        if (!first) {
          ostr << first;
        } else {
          ostr << "eds. ";
          first = false;
        }
        if (!middle.empty()) {
          ostr << middle;
        }
      }
    }

    if (!editors.empty()) {
      editor = editors.substr(begin);

      std::string last, middle, given;

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
          given = editor.substr(j, i - j + 1);
          if (i != editor.length())
            middle = editor.substr(i + 1);
        }
      } else {
        last = editor;
      }

      if (!last.empty()) {
        ostr << ", ";
        if (!first) {
          ostr << last;
        } else {
          ostr << "eds. ";
          first = false;
        }
      }

      if (!given.empty()) {
        ostr << ", ";
        if (!first) {
          ostr << first;
        } else {
          ostr << "eds. ";
          first = true;
        }
        if (!middle.empty()) {
          ostr << middle;
        }
      }
    }
  }

  field_it = record["volume"];
  if (field_it != field_end) {
    ostr << ", " << Encoding::LaTeXDecode(field_it->second, ctx.pool);
  }

  field_it = record["number"];
  if (field_it != field_end) {
    ostr << ", " << Encoding::LaTeXDecode(field_it->second, ctx.pool);
  }

  field_it = record["pages"];
  if (field_it != field_end) {
    ostr << ", " << Encoding::LaTeXDecode(field_it->second, ctx.pool);
  }

  field_it = record["doi"];
  if (field_it != field_end) {
    ostr << " doi:" << Encoding::LaTeXDecode(field_it->second, ctx.pool);
  }

  ostr << "\n\n";

  return true;
}
