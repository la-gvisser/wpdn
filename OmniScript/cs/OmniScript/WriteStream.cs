// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.IO;
    using System.Text;

    public class WriteStream
    {
        public static implicit operator byte[](WriteStream stream)
        {
            return stream.Data.GetBuffer();
        }

        public MemoryStream Data;

        public WriteStream(int size)
        {
            this.Data = new MemoryStream(size);
        }

        public void Write(byte[] data)
        {
            this.Data.Write(data, 0, data.Length);
        }

        public void Write(byte value)
        {
            this.Data.WriteByte(value);
        }

        public void Write(short value)
        {
            this.Data.Write(BitConverter.GetBytes(value), 0, sizeof(short));
        }

        public void Write(ushort value)
        {
            this.Data.Write(BitConverter.GetBytes(value), 0, sizeof(ushort));
        }

        public void Write(int value)
        {
            this.Data.Write(BitConverter.GetBytes(value), 0, sizeof(int));
        }

        public void Write(uint value)
        {
            this.Data.Write(BitConverter.GetBytes(value), 0, sizeof(uint));
        }

        public void Write(long value)
        {
            this.Data.Write(BitConverter.GetBytes(value), 0, sizeof(long));
        }

        public void Write(ulong value)
        {
            this.Data.Write(BitConverter.GetBytes(value), 0, sizeof(ulong));
        }

        public void Write(double value)
        {
            this.Data.Write(BitConverter.GetBytes(value), 0, sizeof(double));
        }

        public void Write(OmniId value)
        {
            byte[] bytes = value.GetBytes();
            this.Data.Write(bytes, 0, bytes.Length);
        }

        public void WriteString(String value)
        {
            byte[] bytes = Encoding.Convert(Encoding.Unicode, Encoding.ASCII, Encoding.Unicode.GetBytes(value));
            this.Data.Write(bytes, 0, bytes.Length);
        }

        public void WriteUnicode(String value)
        {
            byte[] bytes = Encoding.Unicode.GetBytes(value);
            this.Data.Write(bytes, 0, bytes.Length);
        }

        public long Seek(long offset, SeekOrigin origin = SeekOrigin.Begin)
        {
            return this.Data.Seek(offset, origin);
        }

        public long Tell()
        {
            return this.Data.Position;
        }
    }
}
