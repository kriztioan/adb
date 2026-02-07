/**
 *  @file   Actions.h
 *  @brief  Actions Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-21
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#pragma once

#include "BibTeX.h"
#include "Database.h"
#include "HTML.h"
#include "HTTP.h"
#include "Javascript.h"
#include "MSWord.h"
#include "Pool.h"
#include "Preferences.h"
#include "Record.h"
#include "Strings.h"
#include "Text.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cmath>
#include <iostream>

#include "config.h"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

std::string list_themes(std::filesystem::path &directory);

void WriteHTMLHeader(Preferences &prefs);
void WriteOutput();

void DisplayToolsForm(HTTP &http, [[maybe_unused]] Preferences &prefs);
void DisplayData(HTTP &http, Preferences &prefs, Record &record,
                 Record &strings);
void DisplayDataList(HTTP &http, Preferences &prefs);
void WriteHTMLFooter();
void DisplayMenu(HTTP &http, Preferences &prefs);
void DisplayFooter(const std::chrono::steady_clock::time_point &t0);
void DisplayRecord(HTTP &http, Preferences &prefs);
std::vector<std::string> split_on_and(std::string str);

void DisplayRecordForm(HTTP &http, Preferences &prefs);

void Update(HTTP &http, Preferences &prefs);

void QueryADS(HTTP &http, Preferences &prefs);

void QueryDOI(HTTP &http, Preferences &prefs);

void DisplayInfo([[maybe_unused]] HTTP &http, Preferences &prefs);
void DisplaySplash([[maybe_unused]] HTTP &http, Preferences &prefs);

void DisplayConfigForm(HTTP &http, Preferences &prefs);
void DisplayImportForm(HTTP &http, [[maybe_unused]] Preferences &prefs);
void DisplayReindexForm([[maybe_unused]] HTTP &http, Preferences &prefs);

void DisplayKeywords(HTTP &http, Preferences &prefs);
void DisplayAuthors(HTTP &http, Preferences &prefs);
void DisplayDuplicatesForm(HTTP &http, Preferences &prefs);
void DisplayDOICrossrefForm(HTTP &http, Preferences &prefs);
void Delete(HTTP &http, Preferences &prefs);
void Import(HTTP &http, Preferences &prefs);

void Export(HTTP &http, Preferences &prefs);

using act = void (*)(HTTP &, Preferences &);
static constexpr const std::array<std::pair<std::string_view, act>, 22> actions{
    {{"authors", DisplayAuthors},
     {"config", DisplayConfigForm},
     {"delete", Delete},
     {"doicrossref", DisplayDOICrossrefForm},
     {"duplicates", DisplayDuplicatesForm},
     {"edit", DisplayRecordForm},
     {"exportBibTeX", Export},
     {"exportMSWord", Export},
     {"exportText", Export},
     {"import", DisplayImportForm},
     {"importBibTeX", Import},
     {"info", DisplayInfo},
     {"keywords", DisplayKeywords},
     {"list", DisplayDataList},
     {"open", DisplayRecord},
     {"queryADS", QueryADS},
     {"queryDOI", QueryDOI},
     {"reindex", DisplayReindexForm},
     {"search", DisplayDataList},
     {"splash", DisplaySplash},
     {"tools", DisplayToolsForm},
     {"update", Update}}};
