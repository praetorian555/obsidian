// See https://aka.ms/new-console-template for more information
using CommandLine;
using Obsidian;

Console.WriteLine("Hello, World!");

IEnumerable<string> files;

ParserResult<Options> parserResult = Parser.Default.ParseArguments<Options>(args);
parserResult.WithParsed(Generator.RunProgram);

namespace Obsidian
{
    public class Options
    {
        [Option("search-path", Required = true, HelpText = "Directory to recuresively search for C++ include files.")]
        public string SearchDirectory { get; set; } = string.Empty;
    }

    public class Generator
    {
        public static void RunProgram(Options options)
        {
            Generator generator = new(options.SearchDirectory);
            IEnumerable<string> headerFiles = generator.GetHeaderFiles();
        }

        private string _searchDirectory = string.Empty;

        public Generator(string searchDirectory)
        {
            _searchDirectory = searchDirectory;
            if (!Directory.Exists(_searchDirectory))
            {
                throw new Exception("Search directory does not exist!");
            }

            
        }

        public IEnumerable<string> GetHeaderFiles()
        {
            IEnumerable<string> hFiles = Directory.EnumerateFiles(_searchDirectory, "*.h", SearchOption.AllDirectories);
            IEnumerable<string> hppFiles = Directory.EnumerateFiles(_searchDirectory, "*.hpp", SearchOption.AllDirectories);
            return hFiles.Concat(hppFiles);
        }
    }
}
