using System.Collections.Concurrent;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

namespace Ge.Framework;

public static class AssemblyManager
{
    private class ManagedAssembly(AssemblyLoadContext context, Assembly assembly)
    {
        public AssemblyLoadContext LoadContext { get; } = context;
        public Assembly Assembly { get; } = assembly;
    }

    private static readonly ConcurrentDictionary<string, ManagedAssembly> _assemblies =
        new(StringComparer.OrdinalIgnoreCase);

    public static bool LoadAssembly(string path)
    {
        Console.WriteLine($"Loading '{path}' assembly...");

        if (string.IsNullOrWhiteSpace(path))
        {
            Console.WriteLine($"Failed to load '{path}' assembly: invalid path ");
            return false;
        }

        path = Path.GetFullPath(path);

        var context =
            new AssemblyLoadContext(Path.GetFileNameWithoutExtension(path), isCollectible: true);
        var assembly = context.LoadFromAssemblyPath(path);
        var assemblyName = assembly.GetName().Name!;

        if (_assemblies.TryRemove(assemblyName, out var old))
        {
            old.LoadContext.Unload();
        }

        _assemblies[assemblyName] = new ManagedAssembly(context, assembly);
        return true;
    }

    public static bool UnloadAssembly(string path)
    {
        path = Path.GetFullPath(path);

        if (!_assemblies.TryRemove(path, out var managed))
        {
            return false;
        }

        managed.LoadContext.Unload();
        GC.Collect();
        GC.WaitForPendingFinalizers();
        return true;
    }

    public static IntPtr GetFunctionPointer(string assemblyName, string typeName,
        string methodName, string? delegateTypeName)
    {
        if (!_assemblies.TryGetValue(assemblyName, out var managed))
        {
            Console.WriteLine($"Failed to find '{assemblyName}' assembly");
            return IntPtr.Zero;
        }

        var type = managed.Assembly.GetType(typeName);
        if (type == null)
        {
            Console.WriteLine($"Failed to find '{typeName}' type");
            return IntPtr.Zero;
        }

        var method = type.GetMethod(methodName, BindingFlags.Public | BindingFlags.Static);
        if (method == null)
        {
            Console.WriteLine($"Failed to find '{methodName}' method");
            return IntPtr.Zero;
        }

        // If no delegate type is specified, we check if the method is marked with
        // UnmanagedCallersOnlyAttribute

        if (delegateTypeName == null)
        {
            if (method.GetCustomAttribute<UnmanagedCallersOnlyAttribute>() == null)
            {
                Console.WriteLine(
                    $"'{methodName}' method is not marked with 'UnmanagedCallersOnlyAttribute'");
                return IntPtr.Zero;
            }

            return method.MethodHandle.GetFunctionPointer();
        }

        // If a delegate type is specified, we create a delegate from the method and return its
        // pointer

        var delegateType = managed.Assembly.GetType(delegateTypeName);
        if (delegateType == null)
        {
            Console.WriteLine($"Failed to find '{delegateTypeName}' delegate type");
            return IntPtr.Zero;
        }

        var methodDelegate = Delegate.CreateDelegate(delegateType, method);
        return Marshal.GetFunctionPointerForDelegate(methodDelegate);
    }
}