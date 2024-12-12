using System.Diagnostics;
using System.Text.Json;

internal class Program
{
    static async Task Main(string[] args)
    {
        Console.WriteLine("Download images");
        const int numImages = 10;

        //await DownloadImagesParallelAsync(numImages);
        await DownloadImagesSequentAsync(numImages);
    }

    static async Task DownloadImagesParallelAsync(int numImages)
    {
        Stopwatch sw = new Stopwatch();
        sw.Start();

        List<Task> tasks = [];
        for (int i = 0; i < numImages; i++)
        {
            tasks.Add(DownloadImageAsync());
        }

        await Task.WhenAll(tasks);

        sw.Stop();
        Console.WriteLine($"Success! Time: {sw.ElapsedMilliseconds} ms");
    }

    static async Task DownloadImagesSequentAsync(int numImages)
    {
        Stopwatch sw = new Stopwatch();
        sw.Start();

        for (int i = 0; i < numImages; i++)
        {
            await DownloadImageAsync();
        }

        sw.Stop();
        Console.WriteLine($"Success! Time: {sw.ElapsedMilliseconds} ms");
    }

    static async Task DownloadImageAsync()
    {
        string urlAPI = "https://dog.ceo/api/breeds/image/random";

        try
        {
            using (HttpClient client = new HttpClient())
            {
                HttpResponseMessage response = await client.GetAsync(urlAPI);

                if (response.IsSuccessStatusCode)
                {
                    string json = await response.Content.ReadAsStringAsync();
                    Response dogResponse = JsonSerializer.Deserialize<Response>(json);

                    Console.WriteLine($"Starting download from url: {dogResponse.message}");

                    if (dogResponse.status == "success")
                    {
                        Console.WriteLine($"Image from {dogResponse.message} download succesfully!");
                    }
                    else
                    {
                        throw new Exception($"{dogResponse.status}");
                    }
                }
                else
                {
                    throw new Exception($"{response.StatusCode}");
                }
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex.Message);
        }
    }
}

public class Response
{
    public string message { get; set; }
    public string status { get; set; }
}