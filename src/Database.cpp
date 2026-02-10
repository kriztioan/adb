/**
 *  @file   Database.cpp
 *  @brief  Database Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Database.h"

Database::Database(const std::filesystem::path &f, Pool &pool)
    : filename(f), pool(pool) {

  state = false;

  if (!std::filesystem::is_regular_file(f)) {
    return;
  }

  size_t page_size = getpagesize();

  size = (std::filesystem::file_size(f) + page_size - 1) & ~(page_size - 1);

  int fd = open(filename.c_str(), O_RDONLY | O_SHLOCK);
  if (fd == -1) {
    return;
  }

  data =
      (char *)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

  if (data == MAP_FAILED) {
    close(fd);
    return;
  }

  std::memcpy(&id, data, sizeof(long));

  vRecords.reserve(1024);

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

bool Database::Commit() {

  static char tmp[] = "./tmp/adb.XXXXXX";

  if (!mkstemp(tmp)) {
    return false;
  }

  std::fstream ofstr(tmp);
  if (ofstr.fail()) {
    return false;
  }

  ofstr.write(reinterpret_cast<char *>(&id), sizeof(long)) << "\n";
  for (const auto &r : vRecords) {
    auto f = r.mFields.begin();
    ofstr << f->first << '=' << Encoding::URLEncode(f->second);
    for (++f; f != r.mFields.end(); ++f) {
      ofstr << '&' << f->first << '=' << Encoding::URLEncode(f->second);
    }
    ofstr << "\n";
  }
  ofstr.close();

  std::string bak = filename.string() + ".bak";
  if (rename(filename.c_str(), bak.c_str()) == -1) {
    return false;
  }

  if (rename(tmp, filename.c_str()) == -1) {
    return false;
  }

  return true;
}

std::vector<Record>::iterator Database::GetRecord(std::string_view id_str) {
  return std::find_if(
      vRecords.begin(), vRecords.end(),
      [id_str](Record &record) { return record.mFields.at("id") == id_str; });
}

bool Database::SetRecord(Record &record, std::string_view id_str) {
  if (id_str == "-1") {
    pool.begin();
    pool << id++;
    record.mFields.at("id") = pool.sv();
    vRecords.push_back(record);
    return true;
  } else {
    for (auto &r : vRecords) {
      if (r.mFields.at("id") == id_str) {
        r = record;
        return true;
      }
    }
  }
  return false;
}

bool Database::RemoveRecord(std::string_view id_str) {
  for (auto record_it = vRecords.begin(); record_it < vRecords.end();
       record_it++) {
    if (record_it->mFields.at("id") == id_str) {
      vRecords.erase(record_it);
      return true;
    }
  }
  return false;
}

void Database::SortRecords(std::string_view key, bool reverse) {
  std::sort(vRecords.begin(), vRecords.end(),
            [&](const Record &r1, const Record &r2) {
              auto field_it1 = r1[key];
              auto field_it2 = r2[key];

              if (field_it1 == r1.end() && field_it2 == r2.end()) {
                return false;
              }

              if (field_it1 == r1.end()) {
                return false;
              }

              if (field_it2 == r2.end()) {
                return true;
              }

              if (!reverse) {
                return field_it1->second < field_it2->second;
              } else {
                return field_it1->second > field_it2->second;
              }
            });
}

std::vector<std::string_view> Database::UniqueValuesForKey(
    std::string_view key,
    std::vector<std::string_view> (*func)(std::string_view)) {

  std::vector<std::string_view> v;
  v.reserve(64);

  for (const auto &record : vRecords) {
    auto field_it = record.mFields.find(key);
    if (field_it != record.mFields.end()) {
      std::vector<std::string_view> needles(func(field_it->second));
      for (const auto &needle : needles) {
        bool found = false;
        for (const auto &value : v) {
          if (needle == value) {
            found = true;
            break;
          }
        }
        if (!found) {
          v.emplace_back(needle);
        }
      }
    }
  }

  std::sort(v.begin(), v.end());

  return v;
}

std::vector<std::vector<Record>>
Database::DuplicateRecordsForKey(std::string_view key) {

  SortRecords(key);

  std::vector<Record> vDuplicate;

  std::vector<std::vector<Record>> vDuplicates;

  std::string_view value;

  auto res = vRecords.begin(), end = vRecords.end(), sec = res;
  while ((res = std::adjacent_find(res, end, [key](Record &r1, Record &r2) {
            return r1.mFields[key] == r2.mFields[key];
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

  return vDuplicates;
}

bool Database::ReindexRecords(
    const std::function<bool(Record &record, long id)> &process) {
  long record_id = 0;
  for (auto &r : vRecords) {
    std::string id_str(std::to_string(record_id)), r_id_str(r.mFields.at("id"));
    if (r_id_str == id_str) {
      ++record_id;
      continue;
    }
    if (!process(r, record_id++)) {
      return false;
    }
    pool.begin();
    pool << id_str;
    r.mFields.at("id") = pool.sv();
  }
  id = record_id;
  return true;
}
