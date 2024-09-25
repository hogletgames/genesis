using System;
using System.Runtime.CompilerServices;

namespace Ge.Examples {

    public class MessagePrinter {
        public string Message { get; set; }

        public void Print() {
            PrintMessage(Message);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void PrintMessage(string message);
    }

}
