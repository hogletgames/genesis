/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Dmitry Shilnenkov
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

namespace Ge.Tests {

    public class MethodTest {
        static public void ExistingMethod() {}

        public sbyte ReturnSByte() {
            return 42;
        }

        public sbyte SetSByte(sbyte value) {
            return value;
        }

        public byte ReturnByte() {
            return 42;
        }

        public byte SetByte(byte value) {
            return value;
        }

        public short ReturnInt16()
        {
            return 42;
        }

        public short SetInt16(short value)
        {
            return value;
        }

        public ushort ReturnUInt16()
        {
            return 42;
        }

        public ushort SetUInt16(ushort value)
        {
            return value;
        }

        public int ReturnInt32()
        {
            return 42;
        }

        public int SetInt32(int value)
        {
            return value;
        }

        public uint ReturnUInt32()
        {
            return 42;
        }

        public uint SetUInt32(uint value)
        {
            return value;
        }

        public long ReturnInt64()
        {
            return 42;
        }

        public long SetInt64(long value)
        {
            return value;
        }

        public ulong ReturnUInt64()
        {
            return 42;
        }

        public ulong SetUInt64(ulong value)
        {
            return value;
        }

        public float ReturnFloat()
        {
            return 42f;
        }

        public float SetFloat(float value)
        {
            return value;
        }

        public double ReturnDouble()
        {
            return 42;
        }

        public double SetDouble(double value)
        {
            return value;
        }

        public bool ReturnBoolean()
        {
            return true;
        }

        public bool SetBoolean(bool value)
        {
            return value;
        }

        public char ReturnChar()
        {
            return '*'; // 42
        }

        public char SetChar(char value)
        {
            return value;
        }

        public string ReturnString()
        {
            return "42";
        }

        public string SetString(string value)
        {
            return value;
        }
    }

}
