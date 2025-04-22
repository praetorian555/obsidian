// See https://aka.ms/new-console-template for more information
using CommandLine;
using CppAst;
using Obsidian;

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

            string generatedFile = File.ReadAllText("reflection-header.template");
            foreach (CppNamespace ns in compilation.Namespaces)
            {
                foreach (CppEnum e in ns.Enums)
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
                    string generatedCpp = generator.GenerateReflectionInfo(e);
                    generatedFile += generatedCpp;
                    generatedFile += "\n";
                }
            }
            generator.WriteToFile("reflection.hpp", generatedFile);
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

        public string GenerateReflectionInfo(CppEnum e)
        {
            string template = File.ReadAllText("enum.template");
            template = template.Replace("__name_to_replace__", e.Name);
            template = template.Replace("__full_name_to_replace__", e.FullName);
            template = template.Replace("__enum_path__", $"\"{e.SourceFile}\"");
            string enumToNameCases = string.Empty;
            foreach (CppEnumItem item in e.Items)
            {
                enumToNameCases += $"\t\t\tcase {e.FullName}::{item.Name}: return \"{item.Name}\";\n";
            }
            template = template.Replace("__insert_name_mapping__", enumToNameCases);
            if (e.Items.Count > 0)
            {
                template = template.Replace("__first_entry__", $"{e.FullName}::{e.Items[0].Name}");
                template = template.Replace("__last_enum__", $"{e.FullName}::{e.Items.Last().Name}");
            }
            else
            {
                throw new Exception("Enumeration can't be empty!");
            }
            return template;
        }

        public void WriteToFile(string fileName, string contents)
        {
            string fullPath = Path.Combine(_destinationDirectory, fileName);
            File.WriteAllText(fullPath, contents);
        }

    }
}
