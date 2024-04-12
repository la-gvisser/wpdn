// =============================================================================
//	peeknotify.h
// =============================================================================
//	Copyright (c) 2003-2015 Savvius, Inc. All rights reserved.

#ifndef PEEKNOTIFY_H
#define PEEKNOTIFY_H

#include "heunk.h"

/// \defgroup Notification Notification interfaces and classes implemented by Omni.
/// \brief Notification interfaces and classes.
///
/// These interfaces and classes are implemented by Omni to provide event notifications.

/// \enum PeekSeverity
/// \brief Severity levels for notifications.
enum PeekSeverity
{
	peekSeverityInformational,
	peekSeverityMinor,
	peekSeverityMajor,
	peekSeveritySevere
};


#define Category_Action \
{ 0xDB0B577B, 0x0FD9, 0x448D, {0xA1, 0xB3, 0x51, 0x8B, 0x53, 0x2C, 0x36, 0x90} }


/// \internal
/// \interface ILogMessage
/// \ingroup Notification
/// \brief Add a message to the engine log.
///
/// This interface is implemented by the Omni Engine and allows messages
/// to be added to the engine log.
#define ILogMessage_IID \
{ 0xE3519C0D, 0x5A34, 0x44E2, {0xB1, 0x7F, 0x9A, 0x9A, 0xB5, 0x8D, 0xC7, 0x97} }

class HE_NO_VTABLE ILogMessage : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ILogMessage_IID)

	/// Log a message.
	/// \param context ID associated with the component that is sending this
	/// notification. Usually a capture. May be GUID_NULL.
	/// \param source ID associated with the object sending this notification.
	/// \param nSourceKey Key that can be used by the source.
	/// \param nTimeStamp Timestamp that specifies when the message is created. This parameter
	/// can be zero in which case the engine timestamps the message when the message is added to the log.
	/// \param severity Severity of the notification.
	/// \param pszShortMessage A short text message to be used for the notification.
	/// \param pszLongMessage A long text message to be used for the notification.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks Use the INotify interface if this message should actually
	/// generate an event notification. The timestamp is nanoseconds since
	/// midnight 1/1/1601 UTC.
	HE_IMETHOD LogMessage(/*in*/ const Helium::HeID& context,
		/*in*/ const Helium::HeID& source, /*in*/ UInt32 nSourceKey,
		/*in*/ UInt64 nTimeStamp, /*in*/ PeekSeverity severity,
		/*in*/ const wchar_t* pszShortMessage, /*in*/ const wchar_t* pszLongMessage) = 0;
};


/// \internal
/// \interface INotifyContext
/// \ingroup Notification
/// \brief Get the notification context ID to by used in INotify::Notify().
///
/// Implemented by a Capture or Forensic Search object which may be the
/// site for your object if you've implemented IHeObjectWithSite.
#define INotifyContext_IID \
{ 0x5DC733B6, 0x8350, 0x4F28, {0xAB, 0x94, 0x76, 0xC6, 0x05, 0x96, 0x69, 0xFC} }

class HE_NO_VTABLE INotifyContext : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(INotifyContext_IID)

	HE_IMETHOD GetNotifyContext(/*out*/ Helium::HeID* pContext) = 0;
};

/// \internal
/// \interface INotifyInfo
/// \ingroup Notification
/// \brief Get extra information that can be included in a notification.
///
/// Currently implemented by the Engine object to provide the engine name,
/// IP address and capture name.
#define INotifyInfo_IID \
{ 0x5F4198B0, 0x0DE6, 0x402a, {0x9F, 0xB8, 0x96, 0x67, 0x3F, 0x8C, 0xEE, 0x6A} }

class HE_NO_VTABLE INotifyInfo : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(INotifyInfo_IID)

	HE_IMETHOD GetHeader(/*in*/ const Helium::HeID& context,
		/*in*/ const Helium::HeID& source, /*in*/ UInt32 nSourceKey,
		/*in*/ UInt64 nTimeStamp, /*out*/ Helium::HEBSTR* pbstrHeader) = 0;
};

/// \interface INotify
/// \ingroup Notification
/// \brief Send a notification about an event.
///
/// This interface is implemented by the Omni Engine and allows
/// a notification to be sent when an event occurs. The type of notification depends
/// on the configuration of the engine and could include the saving of the notification in a log
/// file, the sending of an email message, or the execution of a batch file or program.
#define INotify_IID \
{ 0xF0C4FA6C, 0x827E, 0x42A5, {0xBB, 0x7F, 0xF0, 0xCD, 0x09, 0x26, 0x4A, 0x0E} }

class HE_NO_VTABLE INotify : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(INotify_IID)

	/// Send a notification.
	/// \param context ID associated with the component that is sending this
	/// notification. Usually a capture. May be GUID_NULL.
	/// \param source ID associated with the object sending this notification.
	/// \param nSourceKey Key that can be used by the source.
	/// \param nTimeStamp Timestamp that specifies when the event occurred.
	/// This parameter can be zero in which case the engine provides a
	/// timestamp when it sends the notification.
	/// \param severity Severity of the notification.
	/// \param pszShortMessage A short text message to be used for the notification.
	/// \param pszLongMessage A long text message to be used for the notification.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks The timestamp is nanoseconds since midnight 1/1/1601 UTC.
	HE_IMETHOD Notify(/*in*/ const Helium::HeID& context,
		/*in*/ const Helium::HeID& source, /*in*/ UInt32 nSourceKey,
		/*in*/ UInt64 nTimeStamp, /*in*/ PeekSeverity severity,
		/*in*/ const wchar_t* pszShortMessage, /*in*/ const wchar_t* pszLongMessage) = 0;
};


/// \internal
/// \interface IAction
/// \brief An action.
#define IAction_IID \
{ 0x63A1A2E7, 0xD02C, 0x4205, {0xA9, 0x75, 0x05, 0xF6, 0x38, 0x22, 0x0A, 0x1F} }

class HE_NO_VTABLE IAction : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAction_IID)

	/// Get the action ID.
	HE_IMETHOD GetID(/*out*/ Helium::HeID* pID) = 0;
	/// Set the action ID.
	HE_IMETHOD SetID(/*in*/ const Helium::HeID& id) = 0;
	/// Get the action name.
	HE_IMETHOD GetName(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Set the action name.
	HE_IMETHOD SetName(/*in*/ Helium::HEBSTR bstrName) = 0;
	/// Enable a severity level.
	HE_IMETHOD EnableSeverity(/*in*/ PeekSeverity severity, /*in*/ BOOL bEnable) = 0;
	/// Check if a severity level is enabled.
	HE_IMETHOD IsSeverityEnabled(/*in*/ PeekSeverity severity, /*out*/ BOOL* pbEnabled) = 0;
	/// Get the array of disabled notification sources for this action.
	HE_IMETHOD GetDisabledSources(/*out*/ Helium::HeID** ppSources, /*out*/ UInt32* pnSources) = 0;
	/// Set an array of the notification sources that are disabled for this action.
	HE_IMETHOD SetDisabledSources(/*in*/ const Helium::HeID* pSources, /*in*/ UInt32 nSources) = 0;
	/// Check if the source for a notification is enabled.
	HE_IMETHOD IsSourceEnabled(/*in*/ const Helium::HeID& guidSource, /*out*/ BOOL* pbEnabled) = 0;
};


/// \internal
/// \interface IActionCollection
/// \brief A collection of actions.
/// \see IAction
#define IActionCollection_IID \
{ 0x4A1ABCAD, 0x8F04, 0x421E, {0xB8, 0x0B, 0xDE, 0x38, 0x14, 0x4B, 0x14, 0x61} }

class HE_NO_VTABLE IActionCollection : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IActionCollection_IID)

	/// Get an action by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ IAction** ppAction) = 0;
	/// Get the count of items.
	HE_IMETHOD GetCount(/*out*/ SInt32* plCount) = 0;
	/// Get an action by ID.
	HE_IMETHOD ItemFromID(/*in*/ const Helium::HeID& id, /*out*/ IAction** ppAction) = 0;
	/// Add an action.
	HE_IMETHOD Add(/*in*/ IAction* pAction) = 0;
	/// Remove a action.
	HE_IMETHOD Remove(/*in*/ IAction* pAction) = 0;
	/// Remove all actions.
	HE_IMETHOD RemoveAll() = 0;
};


/// \internal
/// \interface IEmailAction
/// \brief Configure email notification parameters.
///
/// This interface is implemented by the Omni Engine and allows for the configuration of the
/// parameters required to send an email message when a notification is required.
#define IEmailAction_IID \
{ 0x5F897878, 0xD41A, 0x44fd, {0x9A, 0x12, 0x36, 0x35, 0xDE, 0x63, 0x62, 0x54} }

class HE_NO_VTABLE IEmailAction : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IEmailAction_IID)

	/// Get the email sender (From:).
	/// \param pbstrSender Pointer to the sender of email notifications.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD GetSender(/*out*/ Helium::HEBSTR* pbstrSender) = 0;
	/// Set the email sender (From:).
	/// \param bstrSender The sender of email notifications.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD SetSender(/*in*/ Helium::HEBSTR bstrSender) = 0;
	/// Get the email recipient (To:).
	/// \param pbstrRecipient Pointer to the recipient of email notifications.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD GetRecipient(/*out*/ Helium::HEBSTR* pbstrRecipient) = 0;
	/// Set the email recipient (To:).
	/// \param bstrRecipient The recipient of email notifications.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD SetRecipient(/*in*/ Helium::HEBSTR bstrRecipient) = 0;
	/// Get the email server IP address or DNS name.
	/// \param pbstrSmtpServer Pointer to the IP address or DNS name of the email server.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD GetServer(/*out*/ Helium::HEBSTR* pbstrSmtpServer) = 0;
	/// Set the email server IP address or DNS name.
	/// \param bstrSmtpServer The IP address or DNS name of the email server.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD SetServer(/*in*/ Helium::HEBSTR bstrSmtpServer) = 0;
	/// Get the email server port (usually 25 by default).
	/// \param pusPort Pointer to the configured port for the email server.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD GetPort(/*out*/ UInt16* pusPort) = 0;
	/// Set the email server port (usually 25 by default).
	/// \param usPort The configured port for the email server.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD SetPort(/*in*/ UInt16 usPort) = 0;
};


/// \internal
/// \interface IExecuteAction
/// \brief Configure a batch file or program to execute when a notification is required.
///
/// This interface is implemented by the Omni Engine and allows for the configuration of a batch file or program that is
/// to be executed when a notification is required.
#define IExecuteAction_IID \
{ 0xB0D232C7, 0xD73A, 0x4F37, {0xB8, 0x73, 0xF8, 0x02, 0x7B, 0x5A, 0x99, 0x16} }

class HE_NO_VTABLE IExecuteAction : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IExecuteAction_IID)

	/// Get the name of the batch file or program to execute when a notification is required.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD GetCommand(/*out*/ Helium::HEBSTR* pbstrCommand) = 0;
	/// Set the name of the batch file or program to execute when a notification is required.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD SetCommand(/*in*/ Helium::HEBSTR bstrCommand) = 0;
	/// Get the arguments to be passed to the batch file or program that is executed when a notification is required.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD GetArgs(/*out*/ Helium::HEBSTR* pbstrArgs) = 0;
	/// Set the arguments to be passed to the batch file or program that is executed when a notification is required.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD SetArgs(/*in*/ Helium::HEBSTR bstrArgs) = 0;
	/// Get the initial directory used by the batch file or program that is executed when a notification is required.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD GetDirectory(/*out*/ Helium::HEBSTR* pbstrDirectory) = 0;
	/// Set the initial directory used by the batch file or program that is executed when a notification is required.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD SetDirectory(/*in*/ Helium::HEBSTR bstrDirectory) = 0;
};

/// \internal
/// \interface ITextLogAction
/// \brief Configure a text based log to be written to every time some event occurs.
///
/// This interface is implemented by the Omni Engine and allows for the configuration of a text file log.
#define ITextLogAction_IID \
{ 0xCDEC55F7, 0x8A25, 0x4E26, {0x99, 0x7B, 0xD6, 0xF7, 0x2C, 0x55, 0x1F, 0xCA} }

class HE_NO_VTABLE ITextLogAction : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ITextLogAction_IID)

	HE_IMETHOD GetFileName(/*out*/ Helium::HEBSTR* pbstrFileName) = 0;
	HE_IMETHOD SetFileName(/*in*/ Helium::HEBSTR bstrFileName) = 0;
	HE_IMETHOD GetFilePath(/*out*/ Helium::HEBSTR* pbstrFilePath) = 0;
	HE_IMETHOD SetFilePath(/*in*/ Helium::HEBSTR bstrFilePath) = 0;
	HE_IMETHOD GetFileSize(/*out*/ UInt64* pullFileSize) = 0;
	HE_IMETHOD SetFileSize(/*in*/ UInt64 ullFileSize) = 0;
	HE_IMETHOD GetTotalFileSizes(/*out*/ UInt64* pullTotalFileSizes) = 0;
	HE_IMETHOD SetTotalFileSizes(/*in*/ UInt64 ullTotalFileSizes) = 0;
	HE_IMETHOD GetFileCount(/*out*/ UInt32* pulFileCount) = 0;
	HE_IMETHOD SetFileCount(/*in*/ UInt32 ulFileCount) = 0;
	HE_IMETHOD GetFileSizesEnabled(/*out*/ BOOL* pbFileSizesEnabled) = 0;
	HE_IMETHOD SetFileSizesEnabled(/*in*/ BOOL bFileSizesEnabled) = 0;
	HE_IMETHOD GetFileCountsEnabled(/*out*/ BOOL* pbFileCountsEnabled) = 0;
	HE_IMETHOD SetFileCountsEnabled(/*in*/ BOOL bFileCountsEnabled) = 0;
};

#define ISnmpTrapAction_IID \
{ 0xFF05801C, 0xB63A, 0x4514, {0x93, 0x5F, 0x3F, 0x71, 0xA8, 0xDD, 0xD2, 0x38} }

class HE_NO_VTABLE ISnmpTrapAction : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISnmpTrapAction_IID)

	HE_IMETHOD GetCommunity(/*out*/ Helium::HEBSTR* pbstrCommunity) = 0;
	HE_IMETHOD SetCommunity(/*in*/ Helium::HEBSTR bstrCommunity) = 0;
	HE_IMETHOD GetDestination(/*out*/ Helium::HEBSTR* pbstrRecipient) = 0;
	HE_IMETHOD SetDestination(/*in*/ Helium::HEBSTR bstrRecipient) = 0;
};

/// \internal
/// \interface ISyslogAction
/// \brief Configure syslog notification parameters.
///
/// This interface is implemented by the Omni Engine and allows for the configuration 
/// of the parameters required to send a syslog message when a notification is required.
#define ISyslogAction_IID \
{ 0x5C78433E, 0xD779, 0x49CC, {0xBF, 0x6B, 0x38, 0x7D, 0x9D, 0x04, 0x01, 0xEC} }

class HE_NO_VTABLE ISyslogAction : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISyslogAction_IID)

	/// Get the syslog destination IP address or DNS name.
	/// \param bstrDest Pointer to the IP address or DNS name of the email server.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD GetDestination(/*out*/ Helium::HEBSTR* pbstrDest) = 0;
	/// Set the syslog destination.
	/// \param pbstrDest The destination IP address or DNS name of the syslog server.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD SetDestination(/*in*/ Helium::HEBSTR bstrDest) = 0;
};

#define NotifyService_CID \
{ 0xBC234522, 0xD476, 0x4384, {0x8C, 0xF3, 0x4F, 0xA4, 0x53, 0xC4, 0xB1, 0x7C} }

#define LogAction_CID \
{ 0xB2DA6C79, 0xC270, 0x4988, {0x8A, 0x20, 0x99, 0x5A, 0x0B, 0xB7, 0xA1, 0xCE} }

#define EmailAction_CID \
{ 0xFF13FDFE, 0x31BA, 0x40DD, {0xAB, 0x8A, 0x77, 0xA8, 0x5C, 0x3A, 0x43, 0x9A} }

#define ExecuteAction_CID \
{ 0x0D8DFB6C, 0x237C, 0x4ADE, {0x8C, 0x37, 0xE2, 0x15, 0xD4, 0xF8, 0xCF, 0x00} }

#define TextLogAction_CID \
{ 0xEF21EA89, 0x80C3, 0x4801, {0x98, 0xF6, 0x54, 0x46, 0x77, 0xCF, 0x12, 0xD0} }

#define SnmpTrapAction_CID \
{ 0xED19F042, 0x0A45, 0x44A6, {0x9C, 0x47, 0xE2, 0xA9, 0xD6, 0x96, 0x88, 0x1C} }

#define SyslogAction_CID \
{ 0x5D3B11A5, 0x993F, 0x4B18, {0xB6, 0x03, 0xF1, 0x79, 0x9A, 0xD3, 0x73, 0x6E} }

#endif /* PEEKNOTIFY_H */
