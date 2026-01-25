/**
 *  @file   Database.h
 *  @brief  Database Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef DATABASE_H
#define DATABASE_H

#include "Record.h"
#include <algorithm>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>
// #include <memory_resource>

class Database {
public:
  Database(const std::filesystem::path &f);
  ~Database();

  bool good();

  bool Commit();

  std::vector<Record>::iterator GetRecord(std::string_view id_str);
  bool SetRecord(Record &record, std::string_view id_str = "-1");
  bool RemoveRecord(std::string_view id_str);

  template <typename T>
  bool ExportRecord(std::string_view id_str, std::ostream &ostr, T &user_data,
                    bool (*parser)(Record &record, std::ostream &ostr,
                                   T &user_data)) {
    for (auto &r : vRecords) {
      if (r.mFields.at("id") == id_str) {
        if (!parser(r, ostr, user_data)) {
          return (false);
        }
        return (true);
      }
    }
    return (false);
  }

  bool
  ReindexRecords(const std::function<bool(Record &record, long id)> &process);

  void SortRecords(const char *key, bool reverse = false);
  std::vector<std::string>
  UniqueValuesForKey(const char *key,
                     std::vector<std::string> (*func)(std::string));

  std::vector<std::vector<Record>> DuplicateRecordsForKey(const char *key);

  std::vector<Record> vRecords;

  // std::pmr::vector<Record> vRecords{&pool};

private:
  long id;
  std::filesystem::path filename;
  bool state;
  char *data = nullptr;
  off_t size;

  /*std::array<std::byte, 4096> stack_buffer;
  std::pmr::monotonic_buffer_resource pool{std::data(stack_buffer),
                                           std::size(stack_buffer)};*/
};

#endif // end of DATABASE_H
