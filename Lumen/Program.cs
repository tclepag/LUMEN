using Lumen.Core;
using System.Runtime.InteropServices;

class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("Hello, World!");

        CWNINFO info = new()
        {
            className = "LumenWindow",
            windowName = "Window",
            posX = 0,
            posY = 0,
            width = 800,
            height = 600,
            winStyle = 0x00CF0000
        };

        Window window = new(info);
        int result = 1;
        while (result != 0)
        {
            result = window.Update();
            Thread.Sleep(1);
        }

        Console.WriteLine("Goodbye, World!");

        window.Dispose();
    }
}