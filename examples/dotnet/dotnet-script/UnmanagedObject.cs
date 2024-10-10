using System;
using System.Runtime.InteropServices;

namespace GeExamples
{

    public static class UnmanagedCode
    {
        [UnmanagedCallersOnly]
        public static int HelloWorld()
        {
            Console.WriteLine("Hello, World!");
            return 123;
        }

        public static IntPtr Object_Ctr()
        {
            var obj = new Object();
            GCHandle handle = GCHandle.Alloc(obj);
            return GCHandle.ToIntPtr(handle);
        }

        public static void Object_Dtr(IntPtr objPtr)
        {
            GCHandle handle = GCHandle.FromIntPtr(objPtr);
            if (handle.IsAllocated)
            {
                handle.Free();
            }
        }
    }

}
