/**
 *  @file   Database.h
 *  @brief  Database Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#pragma once

#include "Pool.h"
#include "Record.h"

#include <algorithm>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <string_view>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>

class Database {
public:
  Database() = delete;
  Database(const std::filesystem::path &f, Pool &pool);
  ~Database();

  bool Good() { return (state); }

  bool Commit();

  std::vector<Record>::iterator GetRecord(std::string_view id_str);
  bool SetRecord(Record &record, std::string_view id_str = "-1");
  bool RemoveRecord(std::string_view id_str);

  template <typename T>
  bool ExportRecord(std::string_view id_str, std::ostream &ostr, T &userdata,
                    bool (*parser)(Record &record, std::ostream &ostr,
                                   T &user_data)) {
    for (auto &r : vRecords) {
      if (r.mFields.at("id") == id_str) {
        if (!parser(r, ostr, userdata)) {
          return (false);
        }
        return (true);
      }
    }
    return (false);
  }

  bool
  ReindexRecords(const std::function<bool(Record &record, long id)> &process);

  void SortRecords(std::string_view key, bool reverse = false);
  std::vector<std::string>
  UniqueValuesForKey(std::string_view key,
                     std::vector<std::string> (*func)(std::string_view));
  std::vector<std::vector<Record>> DuplicateRecordsForKey(std::string_view key);

  std::vector<Record> vRecords;

private:
  std::filesystem::path filename;
  Pool &pool;
  off_t size;
  long id;
  bool state;
  char *data = nullptr;
};
