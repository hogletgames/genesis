using System;
using System.Runtime.InteropServices;

namespace TestAssembly
{
    public class TestClass
    {
        delegate int ReturnIntDelegate(int value);
        public static int returnInt(int value) { return value; }

        [UnmanagedCallersOnly]
        public static int returnIntUnmanagedOnly(int value)
        {
            return value;
        }
    }
}
