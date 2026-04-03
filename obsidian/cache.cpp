#include "cache.hpp"

#include "opal/container/json-writer.h"
#include "opal/container/string-format.h"
#include "opal/file-system.h"
#include "opal/paths.h"
#include "opal/time.h"

Cache CreateCache(const ObsidianArguments& args, const Opal::DynamicArray<Opal::StringUtf8>& file_paths)
{
    Opal::StringUtf8 args_combined;
    args_combined.Reserve(1024);
    for (const auto& input_file : args.input_files)
    {
        args_combined.Append(input_file);
        args_combined.Append('\0');
    }
    for (const auto& input_dir : args.input_dirs)
    {
        args_combined.Append(input_dir);
        args_combined.Append('\0');
    }
    args_combined.Append(args.output_dir);
    args_combined.Append('\0');
    args_combined.Append(args.standard_version);
    args_combined.Append('\0');
    for (const auto& option : args.compile_options)
    {
        args_combined.Append(option);
        args_combined.Append('\0');
    }
    for (const auto& include_dir : args.include_directories)
    {
        args_combined.Append(include_dir);
        args_combined.Append('\0');
    }
    constexpr Opal::Hasher<Opal::StringUtf8> hasher;
    const u64 hash = hasher(args_combined);
    Cache cache;
    cache.app_version = Opal::Format("{}.{}.{}", OBS_VERSION_MAJOR, OBS_VERSION_MINOR, OBS_VERSION_PATCH);
    cache.arguments_hash = hash;
    cache.files.Reserve(file_paths.GetSize());
    for (const auto& file_path : file_paths)
    {
        FileEntry entry;
        entry.path = Opal::Paths::NormalizePath(file_path);
        entry.last_modified = Opal::GetLastFileModifiedTimeInSeconds(file_path);
        cache.files.PushBack(std::move(entry));
    }
    return cache;
}

bool SaveCacheToDisk(const Cache& cache)
{
    auto root = Opal::JsonValue::MakeObject();
    root.Insert("app_version", Opal::JsonValue::MakeString(cache.app_version));
    root.Insert("arguments_hash", Opal::JsonValue::MakeNumber(cache.arguments_hash));
    auto files_array = Opal::JsonValue::MakeArray();
    for (const auto& file : cache.files)
    {
        auto file_obj = Opal::JsonValue::MakeObject();
        file_obj.Insert("path", Opal::JsonValue::MakeString(file.path));
        file_obj.Insert("last_modified", Opal::JsonValue::MakeNumber(file.last_modified));
        files_array.PushBack(std::move(file_obj));
    }
    root.Insert("files", std::move(files_array));
    auto content = Opal::JsonWriter::Serialize(root, {.pretty = true});
    Opal::WriteStringToFile("obs.cache", content);
    return true;
}

Opal::Expected<Cache, bool> LoadCacheFromDisk()
{
    if (!Opal::Exists("obs.cache"))
    {
        return Opal::Expected<Cache, bool>{false};
    }
    Opal::StringUtf8 content = Opal::ReadFileAsString("obs.cache");
    auto reader = Opal::JsonReader::Parse(std::move(content));
    Cache cache;
    cache.app_version = reader.GetRoot()["app_version"].GetString().ToString();
    cache.arguments_hash = reader.GetRoot()["arguments_hash"].GetNumberAs<u64>();
    for (auto& file_entry : reader.GetRoot()["files"])
    {
        FileEntry entry;
        auto path_view = file_entry["path"].GetString();
        entry.path = Opal::StringUtf8(path_view.GetData(), path_view.GetSize());
        entry.last_modified = file_entry["last_modified"].GetNumberAs<f64>();
        cache.files.PushBack(std::move(entry));
    }
    return Opal::Expected<Cache, bool>{std::move(cache)};
}

bool CompareCaches(const Cache& cached, const Cache& current_state)
{
    if (cached.app_version != current_state.app_version)
    {
        Opal::GetLogger().Info("Obsidian", "Cache is stale because app version has changed.");
        return false;
    }
    if (cached.arguments_hash != current_state.arguments_hash)
    {
        Opal::GetLogger().Info("Obsidian", "Cache is stale because input arguments changed.");
        return false;
    }
    if (cached.files.GetSize() != current_state.files.GetSize())
    {
        return false;
    }
    for (const auto& cached_file : cached.files)
    {
        bool found = false;
        for (const auto& current_file_state : current_state.files)
        {
            if (current_file_state.path == cached_file.path && current_file_state.last_modified > cached_file.last_modified)
            {
                Opal::GetLogger().Info("Obsidian", "Cache is stale since a header file has been modified or was missing previously.");
                found = true;
                break;
            }
        }
        if (found)
        {
            return false;
        }
    }
    return true;
}
