// See https://aka.ms/new-console-template for more information
using CommandLine;
using Obsidian;

Console.WriteLine("Hello, World!");

IEnumerable<string> files;

ParserResult<Options> parserResult = Parser.Default.ParseArguments<Options>(args);
parserResult.WithParsed(
    o =>
    {
        if (!Directory.Exists(o.SearchDirectory))
        {
            throw new Exception("Search directory does not exist!");
        }

        IEnumerable<string> hFiles = Directory.EnumerateFiles(o.SearchDirectory, "*.h", SearchOption.AllDirectories);
        IEnumerable<string> hppFiles = Directory.EnumerateFiles(o.SearchDirectory, "*.hpp", SearchOption.AllDirectories);
        files = hFiles.Concat(hppFiles);

        foreach (string file in files)
        {
            Console.WriteLine(file);
        }
    }
);

namespace Obsidian
{
    public class Options
    {
        [Option("search-path", Required = true, HelpText = "Directory to recuresively search for C++ include files.")]
        public string SearchDirectory { get; set; } = string.Empty;
    }
}
