// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.Runtime.InteropServices;
    using System.Runtime.Serialization;

    public class OmniException
         : ExternalException, ISerializable
    {
        public uint Code { get; set; }

        public OmniException()
            : base()
        {
            this.Code = 0;
        }

        public OmniException(String message, uint code=0)
            : base(message)
        {
            this.Code = code;
        }

        public OmniException(String message, Exception inner)
            : base(message, inner)
        {
            this.Code = 0;
        }

        // This constructor is needed for serialization.
        protected OmniException(SerializationInfo info, StreamingContext context)
            : base(info, context)
       {
       }
    }
}
