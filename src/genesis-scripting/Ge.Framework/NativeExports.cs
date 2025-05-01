namespace Ge.Framework;

using System.Runtime.InteropServices;

public static class NativeExports
{
    [UnmanagedCallersOnly(EntryPoint = "LoadAssembly")]
    public static bool LoadAssembly(IntPtr pathPtr)
    {
        var path = Marshal.PtrToStringAnsi(pathPtr)!;
        return AssemblyManager.LoadAssembly(path);
    }

    [UnmanagedCallersOnly(EntryPoint = "UnloadAssembly")]
    public static bool UnloadAssembly(IntPtr pathPtr)
    {
        var path = Marshal.PtrToStringAnsi(pathPtr)!;
        return AssemblyManager.UnloadAssembly(path);
    }

    [UnmanagedCallersOnly(EntryPoint = "GetAssemblyMethodPtr")]
    public static IntPtr GetAssemblyMethodPtr(IntPtr assemblyNamePtr, IntPtr typeNamePtr,
        IntPtr methodNamePtr)
    {
        var path = Marshal.PtrToStringAnsi(assemblyNamePtr)!;
        var typeName = Marshal.PtrToStringAnsi(typeNamePtr)!;
        var methodName = Marshal.PtrToStringAnsi(methodNamePtr)!;

        return AssemblyManager.GetAssemblyMethodPtr(path, typeName, methodName);
    }
}