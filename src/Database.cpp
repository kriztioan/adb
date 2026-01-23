/**
 *  @file   Database.cpp
 *  @brief  Database Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Database.h"

Database::Database(const std::filesystem::path &f) : filename(f) {

  state = false;

  int fd = open(filename.c_str(), O_RDONLY | O_SHLOCK);
  if (fd == -1) {
    return;
  }

  if (!std::filesystem::is_regular_file(f)) {
    close(fd);
    return;
  }

  size = std::filesystem::file_size(f);

  data =
      (char *)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

  if (!data) {
    return;
  }

  std::memcpy(&id, data, sizeof(long));

  vRecords.reserve(256);

  char *p = data + sizeof(long) + 1;
  char *q = p;
  do {
    if (*p == '\n') {
      *p = '\0';
      vRecords.emplace_back(q);
      q = p + 1;
    }
  } while (*++p);

  close(fd);

  state = true;
}

Database::~Database() {
  if (data) {
    munmap(data, size);
  }
}

bool Database::good() { return (state); }

bool Database::Commit() {

  static char tmp[] = "./tmp/adb.XXXXXX";

  if (!mkstemp(tmp)) {
    return (false);
  }

  std::fstream ofstr(tmp);
  if (ofstr.fail()) {
    return (false);
  }

  ofstr.write(reinterpret_cast<char *>(&id), sizeof(long)) << "\n";
  for (auto &r : vRecords) {
    auto f = r.mFields.begin();
    ofstr << f->first << '=' << Coders::URLEncode(f->second);
    for (++f; f != r.mFields.end(); ++f) {
      ofstr << '&' << f->first << '=' << Coders::URLEncode(f->second);
    }
    ofstr << "\n";
  }
  ofstr.close();

  std::string bak = filename.string() + ".bak";
  if (rename(filename.c_str(), bak.c_str()) == -1) {
    return (false);
  }

  if (rename(tmp, filename.c_str()) == -1) {
    return (false);
  }

  return (true);
}

bool Database::SetRecord(Record &record, std::string_view id_str) {
  if (id_str == "-1") {
    record.mFields.at("id") = std::to_string(id++);
    vRecords.push_back(record);
    return (true);
  } else {
    for (auto &r : vRecords) {
      if (r.mFields.at("id") == id_str) {
        r = record;
        return (true);
      }
    }
  }
  return (false);
}

bool Database::RemoveRecord(std::string_view id_str) {
  for (auto it = vRecords.begin(); it < vRecords.end(); it++) {
    if (it->mFields.at("id") == id_str) {
      vRecords.erase(it);
      return (true);
    }
  }
  return (false);
}

void Database::SortRecords(const char *key, bool reverse) {

  if (!reverse) {
    std::sort(vRecords.begin(), vRecords.end(), [key](Record &r1, Record &r2) {
      return (Coders::LaTeXDecode(r1.mFields[key]) <
              Coders::LaTeXDecode(r2.mFields[key]));
    });
  } else {
    std::sort(vRecords.begin(), vRecords.end(), [key](Record &r1, Record &r2) {
      return (Coders::LaTeXDecode(r1.mFields[key]) >
              Coders::LaTeXDecode(r2.mFields[key]));
    });
  }
};

std::vector<std::string>
Database::UniqueValuesForKey(const char *key,
                             std::vector<std::string> (*func)(std::string)) {

  std::vector<std::string> vValues;
  vValues.reserve(64);

  for (auto &r : vRecords) {
    std::vector<std::string> needles(func(r.mFields[key]));
    for (const auto &n : needles) {
      bool found = false;
      for (const auto &v : vValues) {
        if (n == v) {
          found = true;
          break;
        }
      }
      if (!found) {
        vValues.emplace_back(n);
      }
    }
  }

  std::sort(vValues.begin(), vValues.end());

  return (vValues);
}

std::vector<std::vector<Record>>
Database::DuplicateRecordsForKey(const char *key) {

  SortRecords(key);

  std::vector<Record> vDuplicate;

  std::vector<std::vector<Record>> vDuplicates;

  std::string_view value;

  auto res = vRecords.begin(), end = vRecords.end(), sec = res;
  while ((res = std::adjacent_find(res, end, [key](Record &r1, Record &r2) {
            return (r1.mFields[key] == r2.mFields[key]);
          })) != end) {

    if (value.empty()) {
      value = res->mFields[key];
    }

    if (value == res->mFields[key]) {
      vDuplicate.push_back(*res++);
    } else {
      vDuplicate.push_back(*sec);
      vDuplicates.emplace_back(vDuplicate);
      vDuplicate.clear();
      value = res->mFields[key];
      vDuplicate.push_back(*res++);
    }
    sec = res;
  }

  if (!vDuplicate.empty()) {
    vDuplicate.push_back(*sec);
    vDuplicates.emplace_back(vDuplicate);
  }

  return (vDuplicates);
}
