// =============================================================================
//	Product.h
// =============================================================================
//	Copyright (c) 2000-2006 WildPackets, Inc. All rights reserved.
//	Copyright (c) 1989-2000 AG Group, Inc. All rights reserved.

#ifndef PRODUCT_H
#define PRODUCT_H

// The class of the PRODUCT can be one of the following:
#define CLASS_MASK		0xFF000000
#define	PEEK			0x01000000
#define	SATBROWSER		0x02000000
#define	SATELLITE		0x04000000
#define	SCAN			0x08000000

// The media of the PRODUCT can be one of the following:
#define MEDIA_MASK		0x0000FF00
#define	ETHERNET		0x00000100
#define	LOCALTALK		0x00000200
#define	TOKENRING		0x00000400
#define	WIRELESS		0x00000800
#define WAN				0x00001000
#define GIG				0x00002000

// Subclass of product may be:
#define SUBCLASS_MASK	0x00FF0000
#define SERVER			0x00010000

// Subclasses of PEEK.
#define LANVUE			0x00010000
#define OMNI			0x00020000
#define ANY				0x00040000

// Subclasses of SATBROWSER.
#define SKYLINE			0x00000000
#define NETMETER		0x00010000

// The 4 bits of the low order byte of PRODUCT
// signify deviants (to be explained someday).
#define ANYPEEK			(PEEK | ANY | OMNI | ETHERNET | WIRELESS | GIG | WAN)
#define OPEEK			(PEEK | OMNI | ETHERNET | WIRELESS | GIG | WAN)
#define WPEEK			(WAN | PEEK)
#define GPEEK			(EPEEK | GIG)
#define APEEK			(WIRELESS | PEEK)
#define EPEEK			(ETHERNET | PEEK)
#define TPEEK			(TOKENRING | PEEK)
#define LPEEK			(LOCALTALK | PEEK)
#define RPEEK			(ETHERNET | PEEK | SERVER)
#define LANVUE_EN		(ETHERNET | PEEK | LANVUE)
#define LANVUE_TR		(TOKENRING | PEEK | LANVUE)
#define ESATELLITE		(ETHERNET | SATELLITE)
#define ESKY			(ETHERNET | SATBROWSER | SKYLINE)
#define ENETMETER		(ETHERNET | SATBROWSER | NETMETER)
#define SERVERSCAN		(SCAN | SERVER)

#endif /* PRODUCT_H */
