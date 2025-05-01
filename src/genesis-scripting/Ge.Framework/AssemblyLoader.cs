namespace Ge.Framework;

using System;
using System.Collections.Concurrent;
using System.IO;
using System.Linq.Expressions;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

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
        if (string.IsNullOrWhiteSpace(path))
        {
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

    public static IntPtr GetAssemblyMethodPtr(string assemblyName, string typeName,
        string methodName)
    {
        assemblyName = Path.GetFullPath(assemblyName);

        if (!_assemblies.TryGetValue(assemblyName, out var managed))
        {
            return IntPtr.Zero;
        }

        var type = managed.Assembly.GetType(typeName);
        if (type == null)
        {
            return IntPtr.Zero;
        }

        var method = type.GetMethod(methodName, BindingFlags.Public | BindingFlags.Static);
        if (method == null)
        {
            return IntPtr.Zero;
        }

        var d = method.CreateDelegate(
            Expression.GetDelegateType(
                Type.EmptyTypes.Concat([method.ReturnType]).ToArray()));

        return Marshal.GetFunctionPointerForDelegate(d);
    }
}