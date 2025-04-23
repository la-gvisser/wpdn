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
    using System.Linq;
    using System.Text;

    public class ReadStream
    {
        public static implicit operator byte[](ReadStream stream)
        {
            return stream.Data.GetBuffer();
        }

        public MemoryStream Data;

        public ReadStream(byte[] data)
        {
            this.Data = new MemoryStream(data);
        }

        public ReadStream(String data)
        {
            byte[] bytes = new byte[data.Length * sizeof(char)];
            System.Buffer.BlockCopy(data.ToCharArray(), 0, bytes, 0, bytes.Length);
            this.Data = new MemoryStream(bytes);
        }

        public long Find(byte[] target)
        {
            long position = this.Data.Position;
            long current = 0;
            long location = -1;

            byte[] value = new byte[target.Length];
            this.Data.Seek(current, SeekOrigin.Begin);
            int length = this.Data.Read(value, 0, value.Length);
            while (length == value.Length)
            {
                if (value.SequenceEqual(target))
                {
                    location = this.Data.Position - value.Length;
                    break;
                }
                current++;
                this.Data.Seek(current, SeekOrigin.Begin);
                length = this.Data.Read(value, 0, value.Length);
            }

            this.Data.Seek(position, SeekOrigin.Begin);
            return location;
        }

        public long Find(OmniId id)
        {
            return this.Find(id.ToByteArray());
        }

        public long Length()
        {
            return this.Data.Length;
        }

        public byte[] Read(int length)
        {
            byte[] value = new byte[length];
            this.Data.Read(value, 0, length);
            return value;
        }

        public byte ReadByte()
        {
            return (byte)this.Data.ReadByte();
        }

        public short ReadShort()
        {
            byte[] value = new byte[2];
            this.Data.Read(value, 0, 2);
            return BitConverter.ToInt16(value, 0);
        }

        public ushort ReadUShort()
        {
            byte[] value = new byte[2];
            this.Data.Read(value, 0, 2);
            return BitConverter.ToUInt16(value, 0);
        }

        public int ReadInt()
        {
            byte[] value = new byte[4];
            this.Data.Read(value, 0, 4);
            return BitConverter.ToInt32(value, 0);
        }

        public uint ReadUInt()
        {
            byte[] value = new byte[4];
            this.Data.Read(value, 0, 4);
            return BitConverter.ToUInt32(value, 0);
        }

        public long ReadLong()
        {
            byte[] value = new byte[8];
            this.Data.Read(value, 0, 8);
            return BitConverter.ToInt64(value, 0);
        }

        public ulong ReadULong()
        {
            byte[] value = new byte[8];
            this.Data.Read(value, 0, 8);
            return BitConverter.ToUInt64(value, 0);
        }

        public double ReadDouble()
        {
            byte[] value = new byte[8];
            this.Data.Read(value, 0, 8);
            return BitConverter.ToDouble(value, 0);
        }

        public OmniId ReadGuid()
        {
            byte[] value = new byte[16];
            this.Data.Read(value, 0, 16);
            return new OmniId(value);
        }

        public String ReadString()
        {
            short length = this.ReadShort();
            if (length > 0)
            {
                byte[] data = this.Read(length);
                return System.Text.Encoding.Default.GetString(data);
            }
            return "";
        }

        public String ReadString(int length)
        {
            if (length > 0)
            {
                byte[] data = this.Read(length);
                return System.Text.Encoding.Default.GetString(data);
            }
            return "";
        }

        public String ReadUnicode()
        {
            int length = this.ReadInt();
            if ((length <= 2) || ((length % 2) != 0)) return "";
            byte[] bytes = new byte[length - 2];
            this.Data.Read(bytes, 0, (length - 2));
            this.ReadUShort();  // Read the null terminator.
            return Encoding.Unicode.GetString(bytes);
        }

        public String ReadUnicode(int length, bool nullTerminator=true)
        {
            return (nullTerminator) ? ReadUnicodeTerminated(length) : ReadUnicodeUnterminated(length);
        }

        public String ReadUnicodeTerminated(int length)
        {
            if ((length <= 2) || ((length % 2) != 0)) return "";
            byte[] bytes = new byte[length - 2];
            this.Data.Read(bytes, 0, (length - 2));
            this.ReadUShort();  // Read the null terminator.
            return Encoding.Unicode.GetString(bytes);
        }

        public String ReadUnicodeUnterminated(int length)
        {
            if ((length == 0) || ((length % 2) != 0)) return "";
            byte[] bytes = new byte[length];
            this.Data.Read(bytes, 0, (length));
            return Encoding.Unicode.GetString(bytes);
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
