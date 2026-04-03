#pragma once

#include "opal/container/expected.h"

#include "types.hpp"

struct FileEntry
{
    Opal::StringUtf8 path;
    f64 last_modified;
};

struct Cache
{
    Opal::StringUtf8 app_version;
    u64 arguments_hash;
    Opal::DynamicArray<FileEntry> files;
};

Opal::Expected<Cache, bool> LoadCacheFromDisk();
Cache CreateCache(const ObsidianArguments& args, const Opal::DynamicArray<Opal::StringUtf8>& file_paths);
bool SaveCacheToDisk(const Cache& cache);
bool CompareCaches(const Cache& cached, const Cache& current_state);
