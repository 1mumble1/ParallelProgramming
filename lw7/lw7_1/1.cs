class Program
{
    static async Task Main(string[] args)
    {
        try
        {
            Console.WriteLine("Введите имя текстового файла: ");
            string fileName = Console.ReadLine() ?? throw new Exception("Empty file name.");
            string filePath = "C:\\MRT47\\ParallelProgramming\\lw7\\" + fileName;

            if (!File.Exists(filePath))
            {
                Console.WriteLine("Файл не найден.");
                return;
            }

            Console.WriteLine("Введите символы, которые нужно удалить (вводите их слитно, например, abc):");
            string symbolsToRemove = Console.ReadLine();

            string text = await ReadFileAsync(filePath);
            string cleanedText = RemoveSymbols(text, symbolsToRemove);
            await WriteFileAsync(filePath, cleanedText);
            Console.WriteLine("Файл обновлен.");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Произошла ошибка: {ex.Message}");
        }
    }

    static async Task<string> ReadFileAsync(string filePath)
    {
        using (StreamReader reader = new StreamReader(filePath))
        {
            return await reader.ReadToEndAsync();
        }
    }

    static string RemoveSymbols(string text, string? symbolsToRemove)
    {
        if (string.IsNullOrEmpty(symbolsToRemove))
        {
            return text;
        }

        string cleanedText = string.Empty;
        foreach (char c in text)
        {
            if (!symbolsToRemove.Contains(c))
            {
                cleanedText += c;
            }
        }
        return cleanedText;
    }

    static async Task WriteFileAsync(string filePath, string content)
    {
        using (StreamWriter writer = new StreamWriter(filePath, false))
        {
            await writer.WriteAsync(content);
        }
    }
}