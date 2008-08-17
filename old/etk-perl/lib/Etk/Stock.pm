package Etk::Stock;
use strict;
require Exporter;

our @ISA = qw/Exporter/;

our %EXPORT_TAGS = (
    size => [qw/SizeSmall SizeMedium SizeBig/],
    );

use constant
{
    SizeSmall => 0,
    SizeMedium => 1,
    SizeBig => 2,
};

my @all = qw/      
    NoStock
    AddressBookNew
    AppointmentNew
    BookmarkNew
    ContactNew
    DialogApply
    DialogOk
    DialogCancel
    DialogYes
    DialogNo
    DialogClose
    DocumentNew
    DocumentOpen
    DocumentPrint
    DocumentPrintPreview
    DocumentProperties
    DocumentSaveAs
    DocumentSave
    EditClear
    EditCopy
    EditCut
    EditFind
    EditPaste
    EditRedo
    EditUndo
    EditDelete
    EditFindReplace
    FolderNew
    FormatIndentLess
    FormatIndentMore
    FormatJustifyCenter
    FormatJustifyFill
    FormatJustifyLeft
    FormatJustifyRight
    FormatTextBold
    FormatTextItalic
    FormatTextStrikethrough
    FormatTextUnderline
    GoBottom
    GoDown
    GoFirst
    GoHome
    GoJump
    GoLast
    GoNext
    GoPrevious
    GoTop
    GoUp
    ListAdd
    ListRemove
    MailMessageNew
    MailForward
    MailMarkJunk
    MailReplyAll
    MailReplySender
    MailSendReceive
    MediaEject
    MediaPlaybackPause
    MediaPlaybackStart
    MediaPlaybackStop
    MediaRecord
    MediaSeekBackward
    MediaSeekForward
    MediaSkipBackward
    MediaSkipForward
    ProcessStop
    SystemLockScreen
    SystemLogOut
    SystemSearch
    SystemShutdown
    TabNew
    ViewRefresh
    WindowNew
    AccessoriesCalculator
    AccessoriesCharacterMap
    AccessoriesTextEditor
    HelpBrowser
    InternetGroupChat
    InternetMail
    InternetNewsReader
    InternetWebBrowser
    MultimediaVolumeControl
    OfficeCalendar
    PreferencesDesktopAccessibility
    PreferencesDesktopAssistiveTechnology
    PreferencesDesktopFont
    PreferencesDesktopKeyboardShortcuts
    PreferencesDesktopLocale
    PreferencesDesktopRemoteDesktop
    PreferencesDesktopSound
    PreferencesDesktopScreensaver
    PreferencesDesktopTheme
    PreferencesDesktopWallpaper
    PreferencesSystemNetworkProxy
    PreferencesSystemSession
    PreferencesSystemWindows
    SystemFileManager
    SystemInstaller
    SystemSoftwareUpdate
    SystemUsers
    UtilitiesSystemMonitor
    UtilitiesTerminal
    ApplicationsAccessories
    ApplicationsDevelopment
    ApplicationsGames
    ApplicationsGraphics
    ApplicationsInternet
    ApplicationsMultimedia
    ApplicationsOffice
    ApplicationsOther
    ApplicationsSystem
    PreferencesDesktopPeripherals
    PreferencesDesktop
    PreferencesSystem
    AudioCard
    AudioInputMicrophone
    Battery
    CameraPhoto
    CameraVideo
    Computer
    DriveCdrom
    DriveHarddisk
    DriveRemovableMedia
    InputGaming
    InputKeyboard
    InputMouse
    MediaCdrom
    MediaFloppy
    MultimediaPlayer
    Network
    NetworkWireless
    NetworkWired
    Printer
    PrinterRemote
    VideoDisplay
    EmblemFavorite
    EmblemImportant
    EmblemPhotos
    EmblemReadonly
    EmblemSymbolicLink
    EmblemSystem
    EmblemUnreadable
    FaceAngel
    FaceCrying
    FaceDevilGrin
    FaceGlasses
    FaceGrin
    FaceKiss
    FacePlain
    FaceSad
    FaceSmileBig
    FaceSmile
    FaceSurprise
    FaceWink
    ApplicationCertificate
    ApplicationXExecutable
    AudioXGeneric
    FontXGeneric
    ImageXGeneric
    PackageXGeneric
    TextHtml
    TextXGeneric
    TextXGenericTemplate
    TextXScript
    VideoXGeneric
    XDirectoryDesktop
    XDirectoryNormalDragAccept
    XDirectoryNormalHome
    XDirectoryNormalOpen
    XDirectoryNormal
    XDirectoryNormalVisiting
    XDirectoryRemote
    XDirectoryRemoteServer
    XDirectoryRemoteWorkgroup
    XDirectoryTrashFull
    XDirectoryTrash
    XOfficeAddressBook
    XOfficeCalendar
    XOfficeDocument
    XOfficePresentation
    XOfficeSpreadsheet
    PlacesFolder
    PlacesFolderRemote
    PlacesFolderSavedSearch
    PlacesNetworkServer
    PlacesNetworkWorkgroup
    PlacesStartHere
    PlacesUserDesktop
    PlacesUserHome
    PlacesUserTrash
    AudioVolumeHigh
    AudioVolumeLow
    AudioVolumeMedium
    AudioVolumeMuted
    BatteryCaution
    DialogError
    DialogInformation
    DialogWarning
    DialogQuestion
    FolderDragAccept
    FolderOpen
    FolderVisiting
    ImageLoading
    ImageMissing
    MailAttachment
    NetworkError
    NetworkIdle
    NetworkOffline
    NetworkOnline
    NetworkReceive
    NetworkTransmit
    NetworkTransmitReceive
    NetworkWirelessEncrypted
    PrinterError
    UserTrashFull
    /;

my $i = 0;
for (@all) {
	# we'll define inlinable subs here as opposed to constants
	# so we don't need to rewrite the values each time.
	eval "sub $_() { $i }";
	$i++;
}
undef $i;

$EXPORT_TAGS{stock} = \@all;
$EXPORT_TAGS{all} = [@{$EXPORT_TAGS{size}} , @{$EXPORT_TAGS{stock}}];

our @EXPORT_OK = @{$EXPORT_TAGS{all}};

    
1;
