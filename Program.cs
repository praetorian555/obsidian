// See https://aka.ms/new-console-template for more information
using CommandLine;
using CppAst;
using Obsidian;
using System.Reflection.Emit;
using System.Text.RegularExpressions;

ParserResult<Options> parserResult = Parser.Default.ParseArguments<Options>(args);
parserResult.WithParsed(Generator.RunProgram);

namespace Obsidian
{
    public enum CppStandard
    {
        Std20
    };

    public class Options
    {
        [Option("search-path", Required = true, HelpText = "Directory to recuresively search for C++ header files.")]
        public string SearchDirectory { get; set; } = string.Empty;

        [Option("destination-path", Required = true, HelpText = "Folder in which header files with generated reflection code will be placed.")]
        public string DestinationDirectory { get; set; } = string.Empty;

        [Option("include-folders", Separator = ';', Required = false, HelpText = "Which folders to use to resolve includes in your header files.")]
        public IEnumerable<string> IncludeFolders { get; set; } = new List<string>();

        [Option("include-system-folders", Separator = ';', Required = false, HelpText = "Which folders to use to resolve system includes in your header files.")]
        public IEnumerable<string> IncludeSystemFolders { get; set; } = new List<string>();

        [Option("defines", Separator = ';', Required = false, HelpText = "List of defines separated by ; that you use to compile your header files.")]
        public IEnumerable<string> Defines { get; set; } = new List<string>();

        [Option("cpp-std", Required = false, HelpText = "What C++ standard to use.")]
        public CppStandard CppStandard { get; set; } = CppStandard.Std20;
    }

    public class Generator
    {
        public static void RunProgram(Options options)
        {
            Generator generator = new(options);
            IEnumerable<string> headerFiles = generator.GetHeaderFiles();
            CppCompilation compilation = generator.CompileHeaders(headerFiles);
            if (compilation.HasErrors)
            {
                foreach (var message in compilation.Diagnostics.Messages)
                {
                    Console.WriteLine(message);
                }
                throw new Exception("Parsing of header files failed!");
            }

            string generatedFileContent = generator.ProcessAst(compilation);
            generator.WriteToFile("reflection.hpp", generatedFileContent);
        }

        private Options _options;

        private CppStandard _cppStandard { get; set; }

        public Generator(Options options)
        {
            _options = options;
            if (!Directory.Exists(_options.SearchDirectory))
            {
                throw new Exception($"Search directory '{_options.SearchDirectory}' does not exist!");
            }
            if (!Directory.Exists(_options.DestinationDirectory))
            {
                Directory.CreateDirectory(_options.DestinationDirectory);
            }

            if (options.IncludeFolders.Any() || options.IncludeSystemFolders.Any())
            {
                Console.WriteLine("We will try to resolve includes in your header files using following directories:");
                foreach (string dir in options.IncludeFolders)
                {
                    Console.WriteLine($"\t{dir}");
                }
                foreach (string dir in options.IncludeSystemFolders)
                {
                    Console.WriteLine($"\t{dir}");
                }
            }
            if (options.Defines.Any())
            {
                Console.WriteLine("We will use following defines when compiling headers:");
                foreach (string define in options.Defines)
                {
                    Console.WriteLine($"\t{define}");
                }
            }
        }

        public IEnumerable<string> GetHeaderFiles()
        {
            IEnumerable<string> hFiles = Directory.EnumerateFiles(_options.SearchDirectory, "*.h", SearchOption.AllDirectories);
            IEnumerable<string> hppFiles = Directory.EnumerateFiles(_options.SearchDirectory, "*.hpp", SearchOption.AllDirectories);
            return hFiles.Concat(hppFiles);
        }

        private static string GetCppStandardCompilerArgument(CppStandard std)
        {
            switch (std)
            {
                case CppStandard.Std20: return "-std=c++20";
            }
            return "";
        }

        public CppCompilation CompileHeaders(IEnumerable<string> headerFiles)
        {
            CppParserOptions options = new CppParserOptions();
            options.ParseTokenAttributes = true;
            options.ParseSystemIncludes = false;
            options.AdditionalArguments.Add(GetCppStandardCompilerArgument(_cppStandard));
            options.IncludeFolders.AddRange(_options.IncludeFolders);
            options.SystemIncludeFolders.AddRange(_options.IncludeSystemFolders);
            options.Defines.AddRange(_options.Defines);
            // Make sure that on Linux builds we include all interfaces for system headers
            options.Defines.Add("__STRICT_ANSI__");
            options.Defines.Add("_GNU_SOURCE");
            return CppParser.ParseFiles(headerFiles.ToList(), options);
        }

        private static string GetDescription(CppEnumItem? prevItem, CppEnumItem item)
        {
            if (item.Comment == null) return "";
            if (prevItem == null || prevItem.Comment == null) return item.Comment.ToString();
            if (prevItem.Comment.ToString() == item.Comment.ToString()) return "";
            return item.Comment.ToString();
        }

        public string GenerateReflectionInfo(ref string enumEntriesContent, List<string> outHeadersToInclude, CppEnum e)
        {
            string template = File.ReadAllText("enum.template");
            template = template.Replace("__enum_name__", e.Name);
            template = template.Replace("__enum_full_name__", e.FullName);
            string enumDesc = e.Comment != null ? e.Comment.ToString() : string.Empty;
            template = template.Replace("__enum_comment__", enumDesc);
            outHeadersToInclude.Add(e.SourceFile);
            string enumToNameCases = string.Empty;
            string nameToEnumCases = string.Empty;
            string enumToDescCases = string.Empty;
            CppEnumItem? prevItem = null;
            foreach (CppEnumItem item in e.Items)
            {
                enumToNameCases += $"\t\t\tcase {e.FullName}::{item.Name}: return \"{item.Name}\";\n";
                nameToEnumCases += $"\t\tif (strcmp(name, \"{item.Name}\") == 0) return {e.FullName}::{item.Name};\n";
                string itemDesc = GetDescription(prevItem, item);
                prevItem = item;
                enumToDescCases += $"\t\t\tcase {e.FullName}::{item.Name}: return \"{itemDesc}\";\n";
            }
            template = template.Replace("__enum_value_to_name_switch__", enumToNameCases);
            template = template.Replace("__enum_name_to_value_switch__", nameToEnumCases);
            template = template.Replace("__enum_value_to_description_switch__", enumToDescCases);
            if (e.Items.Count > 0)
            {
                template = template.Replace("__enum_last_entry__", $"{e.FullName}::{e.Items.Last().Name}");
            }
            else
            {
                throw new Exception("Enumeration can't be empty!");
            }

            enumEntriesContent += $"\t\t\t{{\n";
            enumEntriesContent += $"\t\t\t\t.name = \"{e.Name}\",\n";
            enumEntriesContent += $"\t\t\t\t.full_name = \"{e.FullName}\",\n";
            enumEntriesContent += $"\t\t\t\t.description = \"{enumDesc}\",\n";
            enumEntriesContent += $"\t\t\t\t.underlying_type_size = {e.IntegerType.SizeOf},\n";
            enumEntriesContent += $"\t\t\t\t.items = {{\n";
            prevItem = null;
            foreach (CppEnumItem item in e.Items)
            {
                enumEntriesContent += $"\t\t\t\t\t{{\n";
                enumEntriesContent += $"\t\t\t\t\t\t.name = \"{item.Name}\",\n";
                string itemDesc = GetDescription(prevItem, item);
                prevItem = item;
                enumEntriesContent += $"\t\t\t\t\t\t.description = \"{itemDesc}\",\n";
                enumEntriesContent += $"\t\t\t\t\t\t.value = static_cast<uint64_t>({item.Value})\n";
                enumEntriesContent += $"\t\t\t\t\t}},\n";
            }
            enumEntriesContent += $"\t\t\t\t}}\n";
            enumEntriesContent += $"\t\t\t}},\n";

            return template;
        }

        private string GetTypeEnum(CppType type)
        {
            if (type.FullName == "std::basic_string<char, std::char_traits<char>, std::allocator<char>>")
            {
                return "String";
            }
            if (type.FullName.EndsWith("*"))
            {
                return "Pointer";
            }
            return "POD";
        }

        public string GenerateReflectionInfo(ref string classEntriesContent, List<string> outHeadersToInclude, CppClass c)
        {
            outHeadersToInclude.Add(c.SourceFile);
            string template = File.ReadAllText("class.template");
            template = template.Replace("__class_name__", c.Name);
            template = template.Replace("__class_scope__", c.FullParentName);
            template = template.Replace("__class_scoped_name__", c.FullName);
            template = template.Replace("__class_description__", c.Comment != null ? c.Comment.ToString() : "");

            string initProperties = "\n\t\t{\n";
            foreach (CppField f in c.Fields)
            {
                if (!HasReflAttribute(f.TokenAttributes)) continue;
                initProperties += "\t\t\t{\n";
                initProperties += $"\t\t\t\t.name = \"{f.Name}\",\n";
                initProperties += $"\t\t\t\t.type_name = \"{f.Type.FullName}\",\n";
                initProperties += $"\t\t\t\t.type_enum = Type::{GetTypeEnum(f.Type)},\n";
                initProperties += $"\t\t\t\t.offset = offsetof({c.FullName}, {f.Name}),\n";
                initProperties += $"\t\t\t\t.size = sizeof({c.FullName}::{f.Name})\n";
                initProperties += "\t\t\t},\n";

            }
            initProperties += "\t\t}";
            template = template.Replace("__class_init_properties__", initProperties);

            return template;
        }

        private bool HasReflAttribute(List<CppAttribute> attributes)
        {
            foreach (CppAttribute attribute in attributes)
            {
                if (attribute.Name == "refl")
                {
                    return true;
                }
            }
            return false;
        }

        public void WriteToFile(string fileName, string contents)
        {
            string fullPath = Path.Combine(_options.DestinationDirectory, fileName);
            File.WriteAllText(fullPath, contents);
        }

        public void AppendEnumReflections(ref string outStr, ref string enumEntriesContent, List<string> outHeadersToInclude, CppContainerList<CppEnum> enums)
        {
            foreach (CppEnum e in enums)
            {
                if (!HasReflAttribute(e.TokenAttributes))
                {
                    continue;
                }
                string generatedCpp = GenerateReflectionInfo(ref enumEntriesContent, outHeadersToInclude, e);
                outStr += generatedCpp;
                outStr += "\n";
            }
        }

        public void AppendClassReflections(ref string outStr, ref string classEntriesContent, List<string> outHeadersToInclude, CppContainerList<CppClass> classes)
        {
            foreach (CppClass c in classes)
            {
                if (!HasReflAttribute(c.TokenAttributes))
                {
                    continue;
                }
                string generatedCpp = GenerateReflectionInfo(ref classEntriesContent, outHeadersToInclude, c);
                outStr += generatedCpp;
                outStr += "\n";
            }
        }

        public string ProcessAst(CppCompilation compilation)
        {
            string outReflectionContent = File.ReadAllText("reflection-header.template");
            string enumReflectionContent = string.Empty;
            string enumEntriesContent = string.Empty;
            string classReflectionContent = string.Empty;
            string classEntriesContent = string.Empty;
            List<string> headersToInclude = new List<string>();

            AppendEnumReflections(ref enumReflectionContent, ref enumEntriesContent, headersToInclude, compilation.Enums);
            Queue<CppNamespace> namespaces = new Queue<CppNamespace>();
            foreach (CppNamespace ns in compilation.Namespaces)
            {
                namespaces.Enqueue(ns);
            }
            while (namespaces.Count() > 0)
            {
                CppNamespace ns = namespaces.Dequeue();

                AppendEnumReflections(ref enumReflectionContent, ref enumEntriesContent, headersToInclude, ns.Enums);
                AppendClassReflections(ref classReflectionContent, ref classEntriesContent, headersToInclude, ns.Classes);

                // Check through all the classes and their nested enums.
                Queue<CppClass> classes = new Queue<CppClass>();
                foreach (CppClass c in ns.Classes)
                {
                    classes.Enqueue(c);
                }
                while (classes.Count() > 0)
                {
                    CppClass c = classes.Dequeue();
                    AppendEnumReflections(ref enumReflectionContent, ref enumEntriesContent, headersToInclude, c.Enums);
                    foreach (CppClass c2 in c.Classes)
                    {
                        classes.Enqueue(c2);
                    }
                }
                foreach (CppNamespace nestedNamespace in ns.Namespaces)
                {
                    namespaces.Enqueue(nestedNamespace);
                }
            }

            string[] uniqueIncludes = headersToInclude.Select(NormalizePath).Distinct().ToArray();
            string headersToIncludeContent = string.Empty;
            foreach (string headerPath in uniqueIncludes)
            {
                headersToIncludeContent += $"#include \"{headerPath}\"\n";
            }
            outReflectionContent = outReflectionContent.Replace("__refl_enum_collection_entries__", enumEntriesContent);
            outReflectionContent = outReflectionContent.Replace("__refl_includes__", headersToIncludeContent);
            outReflectionContent = outReflectionContent.Replace("__refl_enum__", enumReflectionContent);
            outReflectionContent = outReflectionContent.Replace("__refl_class__", classReflectionContent);

            return outReflectionContent;
        }

        private static string NormalizePath(string path)
        {
            // 1) Switch all backslashes to forward slashes
            string p = path.Replace("\\", "/");
            // 2) Collapse any run of multiple slashes into a single slash
            p = Regex.Replace(p, "/{2,}", "/");
            return p;
        }

    }
}
