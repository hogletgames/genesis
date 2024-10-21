using System;
using System.Runtime.InteropServices;

namespace DotNetExample
{

    public static class UnmanagedCode
    {
        public delegate void PrintDelegate(string message);

        [UnmanagedCallersOnly]
        public static void HelloWorld()
        {
            Console.WriteLine("Hello from C#!");
        }

        public static void Print(string message)
        {
            Console.WriteLine(message);
        }
    }

}
