/**
 *  @file   BibTeX.cpp
 *  @brief  BibTeX Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "BibTeX.h"

BibTeXParser::BibTeXParser(std::string_view sv, Pool &pool)
    : sv(sv), pool(pool) {

  bt_initialize();

  static constexpr std::array<const char *, 12> mon = {
      "jan", "feb", "mar", "apr", "may", "jun",
      "jul", "aug", "sep", "oct", "nov", "dec"};

  for (auto m : mon) {
    bt_add_macro_text(const_cast<char *>(m), const_cast<char *>(m), nullptr, 1);
  }

  fmt = bt_create_name_format(const_cast<char *>("vljf"), true);

  bt_set_stringopts(BTE_REGULAR, BTO_PASTE | BTO_COLLAPSE);

  pos = sv.find_first_of('@');
}

BibTeXParser::~BibTeXParser() {
  if (fmt) {
    bt_free_name_format(fmt);
  }
  bt_cleanup();
}

Record BibTeXParser::Parse() {

  std::string_view::size_type end = sv.find_first_of('@', pos + 2);
  std::string str(sv.substr(pos, end - pos));
  pos = end;

  AST *entry =
      bt_parse_entry_s(const_cast<char *>(str.c_str()), nullptr, 1, 0, &status);

  pool.begin();
  pool << "id=-1";

  bt_change_case('u', bt_entry_type(entry), 0);

  pool << "&type=" << Encoding::URLEncode(bt_entry_type(entry))
       << "&biblcode=" << Encoding::URLEncode(bt_entry_key(entry))
       << "&ADSabstract=on&ADSfullpaper=on";

  char *key;

  AST *field = bt_next_field(entry, nullptr, &key);
  while (field) {
    pool << '&' << key << '=';
    if (strcmp(key, "author") == 0 || strcmp(key, "editor") == 0) {
      pool << "%7B";

      bt_stringlist *names = bt_split_list(
          bt_get_text(field), const_cast<char *>("and"), nullptr, 1, 0);

      for (int i = 0; i < names->num_items; i++) {
        bt_name *name = bt_split_name(names->items[i], nullptr, 1, 0);
        if (i > 0) {
          pool << "+and+";
        }
        pool << Encoding::URLEncode(bt_format_name(name, fmt));

        bt_free_name(name);
      }
      pool << "%7D";

      bt_free_list(names);
    } else if (strcmp(key, "keywords") == 0) {
      char *text;
      bt_next_value(field, nullptr, nullptr, &text);
      bt_change_case('l', text, 0);
      pool << '&' << key << "=%7B" << Encoding::URLEncode(text) << "%7D";
    } else {
      bt_nodetype nodetype;
      bt_next_value(field, nullptr, &nodetype, nullptr);
      if (nodetype == BTAST_STRING) {
        pool << '&' << key << "=%7B" << Encoding::URLEncode(bt_get_text(field))
             << "%7D";
      } else {
        pool << '&' << key << '=' << bt_get_text(field);
      }
    }

    field = bt_next_field(entry, field, &key);
  }

  bt_parse_entry_s(nullptr, nullptr, 1, 0, nullptr);

  return Record(const_cast<char *>(pool.sv().data()));
}

bool BibTeX::Export(Record &record, std::ostream &ostr, std::string_view &key) {

  auto field_end = record.end();

  std::string_view type;
  auto field_it = record["type"];
  if (field_it != field_end) {
    type = field_it->second;
  }

  ostr << "@" << type << "{";
  field_it = record[key];
  if (field_it != field_end) {
    ostr << field_it->second;
  }

  static constexpr std::string_view keywords[] = {
      "ADScode", "ADSabstract", "ADSfullpaper", "id",      "keywords",
      "URL",     "type",        "biblcode",     "archive", "doicrossrefstatus"};

  auto mFields = record.mFields;
  for (const auto keyword : keywords) {
    field_it = mFields.find(keyword);
    if (field_it != mFields.end()) {
      mFields.erase(field_it);
    }
  }

  for (const auto &field : mFields) {
    ostr << ",\n"
         << std::setw(10) << std::setiosflags(std::ios::right) << field.first
         << " = " << field.second;
  }

  ostr << "\n}\n\n";

  return true;
}
