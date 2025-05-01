namespace Ge.Framework;

using System.Runtime.InteropServices;

public static class NativeExports
{
    [UnmanagedCallersOnly]
    public static int AssemblyManager_LoadAssembly(IntPtr pathPtr)
    {
        var path = Marshal.PtrToStringAnsi(pathPtr)!;
        return AssemblyManager.LoadAssembly(path) ? 0 : 1;
    }

    [UnmanagedCallersOnly]
    public static int AssemblyManager_UnloadAssembly(IntPtr pathPtr)
    {
        var path = Marshal.PtrToStringAnsi(pathPtr)!;
        return AssemblyManager.UnloadAssembly(path) ? 0 : 1;
    }

    [UnmanagedCallersOnly]
    public static IntPtr AssemblyManager_GetFunctionPointer(IntPtr assemblyNamePtr,
        IntPtr typeNamePtr,
        IntPtr methodNamePtr)
    {
        var path = Marshal.PtrToStringAnsi(assemblyNamePtr)!;
        var typeName = Marshal.PtrToStringAnsi(typeNamePtr)!;
        var methodName = Marshal.PtrToStringAnsi(methodNamePtr)!;

        return AssemblyManager.GetFunctionPointer(path, typeName, methodName);
    }
}