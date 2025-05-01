using System.Runtime.InteropServices;

namespace Ge.Framework;

public static class NativeExports
{
    #region AssemblyManager exports

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
        IntPtr methodNamePtr,
        IntPtr delegateTypeNamePtr)
    {
        var path = Marshal.PtrToStringAnsi(assemblyNamePtr);
        var typeName = Marshal.PtrToStringAnsi(typeNamePtr);
        var methodName = Marshal.PtrToStringAnsi(methodNamePtr);
        var delegateTypeName = Marshal.PtrToStringAnsi(delegateTypeNamePtr);

        if (path == null || typeName == null || methodName == null)
        {
            Console.WriteLine("Invalid parameters passed to AssemblyManager_GetFunctionPointer");
            return IntPtr.Zero;
        }

        return AssemblyManager.GetFunctionPointer(path, typeName, methodName, delegateTypeName);
    }

    #endregion
}