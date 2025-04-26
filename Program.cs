// See https://aka.ms/new-console-template for more information
using CommandLine;
using CppAst;
using Obsidian;
using System.Reflection.Emit;

ParserResult<Options> parserResult = Parser.Default.ParseArguments<Options>(args);
parserResult.WithParsed(Generator.RunProgram);

namespace Obsidian
{
    public enum CppStandard
    {
        Std11,
        Std14,
        Std17,
        Std20
    };

    public class Options
    {
        [Option("search-path", Required = true, HelpText = "Directory to recuresively search for C++ header files.")]
        public string SearchDirectory { get; set; } = string.Empty;

        [Option("destination-path", Required = true, HelpText = "Folder in which header files with generated reflection code will be placed.")]
        public string DestinationDirectory { get; set; } = string.Empty;

        [Option("include-folders", Required = false, HelpText = "Which folders to use to resolve includes in your header files.")]
        public IEnumerable<string>? IncludeFolders { get; set; } = null;

        [Option("cpp-std", Required = false, HelpText = "What C++ standard to use.")]
        public CppStandard CppStandard { get; set; } = CppStandard.Std11;
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

        private string _searchDirectory = string.Empty;
        private string _destinationDirectory = string.Empty;
        private IEnumerable<string>? _includeDirectories = null;
        private CppStandard _cppStandard { get; set; }

        public Generator(Options options)
        {
            _searchDirectory = options.SearchDirectory;
            if (!Directory.Exists(_searchDirectory))
            {
                throw new Exception($"Search directory '{_searchDirectory}' does not exist!");
            }
            _destinationDirectory = options.DestinationDirectory;
            if (!Directory.Exists(_destinationDirectory))
            {
                Directory.CreateDirectory(_destinationDirectory);
            }
            _includeDirectories = options.IncludeFolders?.ToList();
            _cppStandard = options.CppStandard;
        }

        public IEnumerable<string> GetHeaderFiles()
        {
            IEnumerable<string> hFiles = Directory.EnumerateFiles(_searchDirectory, "*.h", SearchOption.AllDirectories);
            IEnumerable<string> hppFiles = Directory.EnumerateFiles(_searchDirectory, "*.hpp", SearchOption.AllDirectories);
            return hFiles.Concat(hppFiles);
        }

        private static string GetCppStandardCompilerArgument(CppStandard std)
        {
            switch (std)
            {
                case CppStandard.Std11: return "-std=c++11";
                case CppStandard.Std14: return "-std=c++14";
                case CppStandard.Std17: return "-std=c++17";
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
            if (_includeDirectories != null)
            {
                options.IncludeFolders.AddRange(_includeDirectories);
            }
            return CppParser.ParseFiles(headerFiles.ToList(), options);
        }

        public string GenerateReflectionInfo(ref string enumEntriesContent, List<string> outHeadersToInclude, CppEnum e)
        {
            string template = File.ReadAllText("enum.template");
            template = template.Replace("__enum_name__", e.Name);
            template = template.Replace("__enum_full_name__", e.FullName);
            outHeadersToInclude.Add(e.SourceFile);
            string enumToNameCases = string.Empty;
            string nameToEnumCases = string.Empty;
            foreach (CppEnumItem item in e.Items)
            {
                enumToNameCases += $"\t\t\tcase {e.FullName}::{item.Name}: return \"{item.Name}\";\n";
                nameToEnumCases += $"\t\tif (strcmp(name, \"{item.Name}\") == 0) return {e.FullName}::{item.Name};\n";
            }
            template = template.Replace("__enum_value_to_name_switch__", enumToNameCases);
            template = template.Replace("__enum_name_to_value_switch__", nameToEnumCases);
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
            enumEntriesContent += $"\t\t\t\t.underlying_type_size = {e.IntegerType.SizeOf},\n";
            enumEntriesContent += $"\t\t\t\t.items = {{\n";
            foreach (CppEnumItem item in e.Items)
            {
                enumEntriesContent += $"\t\t\t\t\t{{\n";
                enumEntriesContent += $"\t\t\t\t\t\t.name = \"{item.Name}\",\n";
                enumEntriesContent += $"\t\t\t\t\t\t.value = static_cast<uint64_t>({item.Value})\n";
                enumEntriesContent += $"\t\t\t\t\t}},\n";
            }
            enumEntriesContent += $"\t\t\t\t}}\n";
            enumEntriesContent += $"\t\t\t}},\n";

            return template;
        }

        public void WriteToFile(string fileName, string contents)
        {
            string fullPath = Path.Combine(_destinationDirectory, fileName);
            File.WriteAllText(fullPath, contents);
        }

        public void AppendEnumReflections(ref string outStr, ref string enumEntriesContent, List<string> outHeadersToInclude, CppContainerList<CppEnum> enums)
        {
            foreach (CppEnum e in enums)
            {
                bool needsReflection = false;
                foreach (CppAttribute attribute in e.TokenAttributes)
                {
                    if (attribute.Name == "refl")
                    {
                        needsReflection = true;
                        break;
                    }
                }
                if (!needsReflection)
                {
                    continue;
                }
                string generatedCpp = GenerateReflectionInfo(ref enumEntriesContent, outHeadersToInclude, e);
                outStr += generatedCpp;
                outStr += "\n";
            }
        }

        public string ProcessAst(CppCompilation compilation)
        {
            string outReflectionContent = File.ReadAllText("reflection-header.template");
            string enumReflectionContent = string.Empty;
            string enumEntriesContent = string.Empty;
            List<string> headersToInclude = new List<string>();

            // Find all enums, only look for enums in namespaces, not nested in classes
            // TODO: Add support for nested enums
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
                foreach (CppNamespace nestedNamespace in ns.Namespaces)
                {
                    namespaces.Enqueue(nestedNamespace);
                }
            }

            string headersToIncludeContent = string.Empty;
            foreach (string headerPath in headersToInclude)
            {
                headersToIncludeContent += $"#include \"{headerPath}\"\n";
            }
            outReflectionContent = outReflectionContent.Replace("__refl_enum_collection_entries__", enumEntriesContent);
            outReflectionContent = outReflectionContent.Replace("__refl_includes__", headersToIncludeContent);
            outReflectionContent = outReflectionContent.Replace("__refl_enum__", enumReflectionContent);

            return outReflectionContent;
        }

    }
}
