// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
//  Copyright (c) 2013-2015 WildPackets, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;

    public class OmniId
    {
        public static bool IsNullOrEmpty(OmniId id)
        {
            if (id == null) return true;
            if (id.Id == OmniId.Empty.Id) return true;
            return false;
        }

        public int CompareTo(OmniId id)
        {
            if (id == null)
            {
                throw new ArgumentException("Object is null", "obj");
            }
            return this.Id.CompareTo(id.Id);
        }

        public int CompareTo(System.Object obj)
        {
            if (obj == null)
            {
                throw new ArgumentException("Object is null", "obj");
            }
            OmniId id = obj as OmniId;
            if (id == null)
            {
                throw new ArgumentException("Object is not an OmniId", "obj");
            }

            return this.Id.CompareTo(id.Id);
        }

        public bool Equals(OmniId id)
        {
            if (id == null)
            {
                return false;
            }
            return (this.Id == id.Id);
        }

        public override bool Equals(System.Object obj)
        {
            if (obj == null)
            {
                return false;
            }
            OmniId id = obj as OmniId;
            if (id == null)
            {
                return false;
            }

            return (this.Id == id.Id);
        }

        public byte[] GetBytes()
        {
            return this.Id.ToByteArray();
        }

        public override int GetHashCode()
        {
            return this.Id.GetHashCode();
        }

        public static readonly OmniId Empty = new OmniId();

        private readonly String preformatted;
        public Guid Id { get; set; }

        public static implicit operator OmniId(Guid value)
        {
            return new OmniId(value);
        }

        public static implicit operator Guid(OmniId value)
        {
            return value.Id;
        }

        public static implicit operator byte[](OmniId id)
        {
            return id.GetBytes();
        }

        public OmniId(Guid id, String formatted)
        {
            this.preformatted = formatted;
            this.Id = id;
        }

        public OmniId(Guid id)
            : this(id, null)
        {
        }

        public OmniId()
            : this(Guid.Empty, null)
        {
        }

        public OmniId(byte[] id)
            : this(new Guid(id), null)
        {
        }

        public OmniId(String id)
            : this(Guid.Parse(id), null)
        {
        }

        public byte[] ToByteArray()
        {
            return this.Id.ToByteArray();
        }

        public String ToString(String format)
        {
            if (((format == "X") || (format == "x")) && (this.preformatted != null))
            {
                return this.preformatted;
            }
            return this.Id.ToString("B").ToUpper();
        }

        public override String ToString()
        {
            return this.ToString(null);
        }
    }
}
