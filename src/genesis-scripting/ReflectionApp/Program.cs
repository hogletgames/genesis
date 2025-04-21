using System.Reflection;

namespace ReflectionApp;

public static class Reflection
{
    static void Main(string[] args)
    {
        var assembly = Assembly.LoadFrom(args[0]);
        Console.WriteLine($"Assembly name: {assembly.FullName}");
        foreach (var type in assembly.GetTypes())
        {
            Console.WriteLine($"- {type.FullName}");

            foreach (var method in type.GetMethods())
            {
                Console.WriteLine($"  - {method.Name}");
            }
        }
    }
}