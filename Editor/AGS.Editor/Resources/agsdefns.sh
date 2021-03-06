// CHECKME: what does following commentary mean, and is it still applicable?
// >>>
// temporarily removed palette functions cos the compiler
// doesnt support typedefs

// The available script API is determined by two bounds: upper bound
// determines which new API parts will be enabled, and lower bound
// determines which obsolete API parts will be disabled.
//
// Upper bound depends on defined SCRIPT_API_vXXX macros. For every macro
// defined the corresponding API contents should be enabled. If certain
// macro is not defined, then those API version contents stay disabled.
//
// Lower bound depends on defined SCRIPT_COMPAT_vXXX macros. For every
// macro defined the deprecated API contents that were still active in
// corresponding version are kept enabled; otherwise these are disabled.

/*
*/
#define function int  // $AUTOCOMPLETEIGNORE$
// CursorMode isn't actually defined yet, but int will do
#define CursorMode int
#define FontType int
#define AudioType int
#define MAX_INV 301
#define MAX_ROOM_OBJECTS    40
#define MAX_LEGACY_GLOBAL_VARS  50
#define MAX_LISTBOX_SAVED_GAMES 50
#define PALETTE_SIZE       256
#define FOLLOW_EXACTLY 32766
#define NARRATOR -1
#define OPT_WALKONLOOK       2
#define OPT_DIALOGOPTIONSGUI 3
#define OPT_ANTIGLIDE        4   // $AUTOCOMPLETEIGNORE$
#define OPT_DIALOGOPTIONSGAP 6
#define OPT_WHENGUIDISABLED  8
#define OPT_ALWAYSSPEECH     9
#define OPT_PIXELPERFECT    11
#define OPT_NOWALKMODE      12
#define OPT_FIXEDINVCURSOR  14
#define OPT_DONTLOSEINV     15
#define OPT_TURNBEFOREWALK  18
#define OPT_HANDLEINVCLICKS 20
#define OPT_MOUSEWHEEL      21
#define OPT_DIALOGNUMBERED  22
#define OPT_DIALOGUPWARDS   23
#define OPT_CROSSFADEMUSIC  24
#define OPT_ANTIALIASFONTS  25
#define OPT_THOUGHTGUI      26
#define OPT_TURNWHENFACING  27
#define OPT_RIGHTTOLEFT     28
#define OPT_MULTIPLEINV     29
#define OPT_SAVEGAMESCREENSHOTS 30
#define OPT_PORTRAITPOSITION 31
#define OPT_LIPSYNCTEXT     99
#define COLOR_TRANSPARENT -1
#define DIALOG_PARSER_SELECTED -3053
#define RUN_DIALOG_RETURN        -1
#define RUN_DIALOG_STOP_DIALOG   -2
#define RUN_DIALOG_GOTO_PREVIOUS -4

#define SCR_NO_VALUE   31998   // $AUTOCOMPLETEIGNORE$

enum bool {
  false = 0,
  true = 1
};

enum eOperatingSystem {
  eOSDOS = 1,
  eOSWindows,
  eOSLinux,
  eOSMacOS,
  eOSAndroid,
  eOSiOS,
  eOSPSP,
  eOSWeb
};

enum TransitionStyle {
  eTransitionFade=0,
  eTransitionInstant,
  eTransitionDissolve,
  eTransitionBoxout,
  eTransitionCrossfade
};

enum MouseButton {
  eMouseLeft = 1,
  eMouseRight = 2,
  eMouseMiddle = 3,
  eMouseLeftInv = 5,
  eMouseRightInv = 6,
  eMouseMiddleInv = 7,
  eMouseWheelNorth = 8,
  eMouseWheelSouth = 9
};

enum RoundDirection {
  eRoundDown = 0,
  eRoundNearest = 1,
  eRoundUp = 2
};

enum RepeatStyle {
  eOnce = 0,
  eRepeat = 1
};

#ifdef SCRIPT_API_v350
enum Alignment {
  eAlignNone         = 0,

  eAlignTopLeft       = 1,
  eAlignTopCenter     = 2,
  eAlignTopRight      = 4,
  eAlignMiddleLeft    = 8,
  eAlignMiddleCenter  = 16,
  eAlignMiddleRight   = 32,
  eAlignBottomLeft    = 64,
  eAlignBottomCenter  = 128,
  eAlignBottomRight   = 256,

  // Masks are helping to determine whether alignment parameter contains
  // particular horizontal or vertical component (for example: left side
  // or bottom side)
  eAlignHasLeft       = 73,
  eAlignHasRight      = 292,
  eAlignHasTop        = 7,
  eAlignHasBottom     = 448,
  eAlignHasHorCenter  = 146,
  eAlignHasVerCenter  = 56
};

enum HorizontalAlignment {
  // eq eAlignTopLeft
  eAlignLeft = 1,
  // eq eAlignTopCenter
  eAlignCenter = 2,
#ifdef SCRIPT_COMPAT_v341
  eAlignCentre = 2,
#endif
  // eq eAlignTopRight
  eAlignRight = 4
};
#endif
#ifndef SCRIPT_API_v350
enum Alignment {
  eAlignLeft = 1,
  eAlignCentre = 2,
  eAlignRight = 3
};
#endif

enum LocationType {
  eLocationNothing = 0,
  eLocationHotspot = 1,
  eLocationCharacter = 2,
  eLocationObject = 3
};

enum CutsceneSkipType {
  eSkipESCOnly = 1,
  eSkipAnyKey = 2,
  eSkipMouseClick = 3,
  eSkipAnyKeyOrMouseClick = 4,
  eSkipESCOrRightButton = 5,
  eSkipScriptOnly = 6
};

enum DialogOptionState {
  eOptionOff = 0,
  eOptionOn = 1,
  eOptionOffForever = 2
};

enum eSpeechStyle {
  eSpeechLucasarts = 0,
  eSpeechSierra = 1,
  eSpeechSierraWithBackground = 2,
  eSpeechFullScreen = 3
};

enum eVoiceMode {
  eSpeechTextOnly = 0,
  eSpeechVoiceAndText = 1,
  eSpeechVoiceOnly = 2
};

enum eFlipDirection {
  eFlipLeftToRight = 1,
  eFlipUpsideDown = 2,
  eFlipBoth = 3
};

enum DialogOptionSayStyle
{
  eSayUseOptionSetting = 1,
  eSayAlways = 2,
  eSayNever = 3
};

enum VideoSkipStyle
{
  eVideoSkipNotAllowed = 0,
  eVideoSkipEscKey = 1,
  eVideoSkipAnyKey = 2,
  eVideoSkipAnyKeyOrMouse = 3
};

enum eKeyCode
{
  eKeyNone  = 0,
  eKeyCtrlA = 1,
  eKeyCtrlB = 2,
  eKeyCtrlC = 3,
  eKeyCtrlD = 4,
  eKeyCtrlE = 5,
  eKeyCtrlF = 6,
  eKeyCtrlG = 7,
  eKeyCtrlH = 8,
  eKeyBackspace = 8,
  eKeyCtrlI = 9,
  eKeyTab = 9,
  eKeyCtrlJ = 10,
  eKeyCtrlK = 11,
  eKeyCtrlL = 12,
  eKeyCtrlM = 13,
  eKeyReturn = 13,
  eKeyCtrlN = 14,
  eKeyCtrlO = 15,
  eKeyCtrlP = 16,
  eKeyCtrlQ = 17,
  eKeyCtrlR = 18,
  eKeyCtrlS = 19,
  eKeyCtrlT = 20,
  eKeyCtrlU = 21,
  eKeyCtrlV = 22,
  eKeyCtrlW = 23,
  eKeyCtrlX = 24,
  eKeyCtrlY = 25,
  eKeyCtrlZ = 26,
  eKeyEscape = 27,
  eKeySpace = 32,
  eKeyExclamationMark = 33,
  eKeyDoubleQuote = 34,
  eKeyHash = 35,
  eKeyDollar = 36,
  eKeyPercent = 37,
  eKeyAmpersand = 38,
  eKeySingleQuote = 39,
  eKeyOpenParenthesis = 40,
  eKeyCloseParenthesis = 41,
  eKeyAsterisk = 42,
  eKeyPlus = 43,
  eKeyComma = 44,
  eKeyHyphen = 45,
  eKeyPeriod = 46,
  eKeyForwardSlash = 47,
  eKey0 = 48,
  eKey1 = 49,
  eKey2 = 50,
  eKey3 = 51,
  eKey4 = 52,
  eKey5 = 53,
  eKey6 = 54,
  eKey7 = 55,
  eKey8 = 56,
  eKey9 = 57,
  eKeyColon = 58,
  eKeySemiColon = 59,
  eKeyLessThan = 60,
  eKeyEquals = 61,
  eKeyGreaterThan = 62,
  eKeyQuestionMark = 63,
  eKeyAt = 64,
  eKeyA = 65,
  eKeyB = 66,
  eKeyC = 67,
  eKeyD = 68,
  eKeyE = 69,
  eKeyF = 70,
  eKeyG = 71,
  eKeyH = 72,
  eKeyI = 73,
  eKeyJ = 74,
  eKeyK = 75,
  eKeyL = 76,
  eKeyM = 77,
  eKeyN = 78,
  eKeyO = 79,
  eKeyP = 80,
  eKeyQ = 81,
  eKeyR = 82,
  eKeyS = 83,
  eKeyT = 84,
  eKeyU = 85,
  eKeyV = 86,
  eKeyW = 87,
  eKeyX = 88,
  eKeyY = 89,
  eKeyZ = 90,
  eKeyOpenBracket = 91,
  eKeyBackSlash = 92,
  eKeyCloseBracket = 93,
  eKeyUnderscore = 95,
  eKeyF1 = 359,
  eKeyF2 = 360,
  eKeyF3 = 361,
  eKeyF4 = 362,
  eKeyF5 = 363,
  eKeyF6 = 364,
  eKeyF7 = 365,
  eKeyF8 = 366,
  eKeyF9 = 367,
  eKeyF10 = 368,
  eKeyHome = 371,
  eKeyUpArrow = 372,
  eKeyPageUp = 373,
  eKeyLeftArrow = 375,
  eKeyNumPad5 = 376,
  eKeyRightArrow = 377,
  eKeyEnd = 379,
  eKeyDownArrow = 380,
  eKeyPageDown = 381,
  eKeyInsert = 382,
  eKeyDelete = 383,
#ifdef SCRIPT_API_v399
  eKeyShiftLeft = 403, 
  eKeyShiftRight = 404, 
  eKeyCtrlLeft = 405, 
  eKeyCtrlRight = 406,
#endif
  eKeyF11 = 433,
  eKeyF12 = 434
};

#ifdef SCRIPT_API_v3507
managed struct Point {
	int x, y;
};
#endif

#define CHARID int  // $AUTOCOMPLETEIGNORE$
builtin struct ColorType {
  char r,g,b;
  char filler;  // $AUTOCOMPLETEIGNORE$
  };

enum AudioFileType {
  eAudioFileOGG = 1,
  eAudioFileMP3 = 2,
  eAudioFileWAV = 3,
  eAudioFileVOC = 4,
  eAudioFileMIDI = 5,
  eAudioFileMOD = 6
};

enum AudioPriority {
  eAudioPriorityVeryLow = 1,
  eAudioPriorityLow = 25,
  eAudioPriorityNormal = 50,
  eAudioPriorityHigh = 75,
  eAudioPriorityVeryHigh = 100
};

enum ChangeVolumeType {
  eVolChangeExisting = 1678,
  eVolSetFutureDefault = 1679,
  eVolExistingAndFuture = 1680
};

enum CharacterDirection {
  eDirectionDown = 0,
  eDirectionLeft,
  eDirectionRight,
  eDirectionUp,
  eDirectionDownRight,
  eDirectionUpRight,
  eDirectionDownLeft,
  eDirectionUpLeft,
  eDirectionNone = SCR_NO_VALUE
};

#ifdef SCRIPT_API_v350
enum StringCompareStyle
{
  eCaseInsensitive = 0,
  eCaseSensitive = 1
};

enum SortStyle
{
  eNonSorted = 0,
  eSorted = 1
};
#endif

#ifdef SCRIPT_API_v399
enum LogLevel
{
	eLogAlert = 1,
	eLogFatal = 2,
	eLogError = 3,
	eLogWarn = 4,
	eLogInfo = 5,
	eLogDebug = 6
};
#endif

#ifdef SCRIPT_API_v399
enum BlendMode {
    eBlendNormal = 0,
    eBlendAdd,
    eBlendDarken,
    eBlendLighten,
    eBlendMultiply,
    eBlendScreen,
    eBlendBurn,
    eBlendSubtract,
    eBlendExclusion,
    eBlendDodge
};
#endif

internalstring autoptr builtin managed struct String {
  /// Creates a formatted string using the supplied parameters.
  import static String Format(const string format, ...);    // $AUTOCOMPLETESTATICONLY$
  /// Checks whether the supplied string is null or empty.
  import static bool IsNullOrEmpty(String stringToCheck);  // $AUTOCOMPLETESTATICONLY$
  /// Returns a new string with the specified string appended to this string.
  import String  Append(const string appendText);
  /// Returns a new string that has the extra character appended.
  import String  AppendChar(char extraChar);
  import int     Contains(const string needle);   // $AUTOCOMPLETEIGNORE$
  /// Creates a copy of the string.
  import String  Copy();
  /// Returns the index of the first occurrence of the needle in this string.
  import int     IndexOf(const string needle);
  /// Returns a lower-cased version of this string.
  import String  LowerCase();
  /// Returns a new string, with the specified character changed.
  import String  ReplaceCharAt(int index, char newChar);
  /// Returns a portion of the string.
  import String  Substring(int index, int length);
  /// Truncates the string down to the specified length by removing characters from the end.
  import String  Truncate(int length);
  /// Returns an upper-cased version of this string.
  import String  UpperCase();
#ifdef SCRIPT_API_v350
  /// Compares this string to the other string.
  import int     CompareTo(const string otherString, StringCompareStyle style = eCaseInsensitive);
  /// Checks whether this string ends with the specified text.
  import bool    EndsWith(const string endsWithText, StringCompareStyle style = eCaseInsensitive);
  /// Returns a copy of this string with all occurrences of LookForText replaced with ReplaceWithText
  import String  Replace(const string lookForText, const string replaceWithText, StringCompareStyle style = eCaseInsensitive);
  /// Checks whether this string starts with the specified text.
  import bool    StartsWith(const string startsWithText, StringCompareStyle style = eCaseInsensitive);
#endif
#ifndef SCRIPT_API_v350
  /// Compares this string to the other string.
  import int     CompareTo(const string otherString, bool caseSensitive = false);
  /// Checks whether this string ends with the specified text.
  import bool    EndsWith(const string endsWithText, bool caseSensitive = false);
  /// Returns a copy of this string with all occurrences of LookForText replaced with ReplaceWithText
  import String  Replace(const string lookForText, const string replaceWithText, bool caseSensitive = false);
  /// Checks whether this string starts with the specified text.
  import bool    StartsWith(const string startsWithText, bool caseSensitive = false);
#endif
  /// Converts the string to a float.
  readonly import attribute float AsFloat;
  /// Converts the string to an integer.
  readonly import attribute int AsInt;
  /// Accesses individual characters of the string.
  readonly import attribute char Chars[];
  /// Returns the length of the string.
  readonly import attribute int Length;
};

#ifdef SCRIPT_API_v350
builtin managed struct Dictionary
{
  /// Creates a new empty Dictionary of the given properties.
  import static Dictionary* Create(SortStyle sortStyle = eNonSorted, StringCompareStyle compareStyle = eCaseInsensitive); // $AUTOCOMPLETESTATICONLY$

  /// Removes all items from the dictionary.
  import void Clear();
  /// Tells if given key is in the dictionary.
  import bool Contains(const string key);
  /// Gets value by the key; returns null if such key does not exist.
  import String Get(const string key);
  /// Removes key/value pair from the dictionary, fails if there was no such key.
  import bool Remove(const string key);
  /// Assigns a value to the given key, adds this key if it did not exist yet.
  import bool Set(const string key, const string value);

  /// Gets if this dictionary is case-sensitive.
  import readonly attribute StringCompareStyle CompareStyle;
  /// Gets the method items are arranged in this dictionary.
  import readonly attribute SortStyle SortStyle;
  /// Gets the number of key/value pairs currently in the dictionary.
  import readonly attribute int ItemCount;
  /// Creates a dynamic array filled with keys in same order as they are stored in the Dictionary.
  import String[] GetKeysAsArray();
  /// Creates a dynamic array filled with values in same order as they are stored in the Dictionary.
  import String[] GetValuesAsArray();
};

builtin managed struct Set
{
  /// Creates a new empty Set of the given properties.
  import static Set* Create(SortStyle sortStyle = eNonSorted, StringCompareStyle compareStyle = eCaseInsensitive); // $AUTOCOMPLETESTATICONLY$

  /// Adds item to the set, fails if such item was already existing.
  import bool Add(const string item);
  /// Removes all items from the set.
  import void Clear();
  /// Tells if given item is in the set.
  import bool Contains(const string item);
  /// Removes item from the set, fails if there was no such item.
  import bool Remove(const string item);

  /// Gets if this set is case-sensitive.
  import readonly attribute StringCompareStyle CompareStyle;
  /// Gets the method items are arranged in this set.
  import readonly attribute SortStyle SortStyle;
  /// Gets the number of items currently in the set.
  import readonly attribute int ItemCount;
  /// Creates a dynamic array filled with items in same order as they are stored in the Set.
  import String[] GetItemsAsArray();
};
#endif

builtin managed struct AudioClip;

builtin managed struct ViewFrame {
  /// Gets whether this frame is flipped.
  readonly import attribute bool Flipped;
  /// Gets the frame number of this frame.
  readonly import attribute int Frame;
  /// Gets/sets the sprite that is displayed by this frame.
  import attribute int Graphic;
  /// Gets/sets the audio that is played when this frame comes around.
  import attribute AudioClip* LinkedAudio;
  /// Gets the loop number of this frame.
  readonly import attribute int Loop;
  /// Gets/sets the sound that is played when this frame comes around.
  import attribute int Sound;    // $AUTOCOMPLETEIGNORE$
  /// Gets the delay of this frame.
  readonly import attribute int Speed;
  /// Gets the view number that this frame is part of.
  readonly import attribute int View;
};

builtin managed struct DrawingSurface {
  /// Clears the surface to the specified colour, or transparent if you do not specify a colour.
  import void Clear(int colour=-SCR_NO_VALUE);
  /// Creates a copy of the surface.
  import DrawingSurface* CreateCopy();
  /// Draws a circle onto the surface with its centre at (x,y).
  import void DrawCircle(int x, int y, int radius);
#ifdef SCRIPT_API_v399
  /// Draws a sprite onto the surface with its top-left corner at (x,y).
  import void DrawImage(int x, int y, int spriteSlot, int transparency=0, int width=SCR_NO_VALUE, int height=SCR_NO_VALUE,
						int part_x=0, int part_y=0, int part_width=SCR_NO_VALUE, int part_height=SCR_NO_VALUE);
  /// Blends a sprite onto the surface with its top-left corner at (x,y).
  import void BlendImage(int x, int y, int spriteSlot, BlendMode mode, int transparency=0, int width=SCR_NO_VALUE, int height=SCR_NO_VALUE,
						int part_x=0, int part_y=0, int part_width=SCR_NO_VALUE, int part_height=SCR_NO_VALUE);
#endif
#ifndef SCRIPT_API_v399
  /// Draws a sprite onto the surface with its top-left corner at (x,y).
  import void DrawImage(int x, int y, int spriteSlot, int transparency=0, int width=SCR_NO_VALUE, int height=SCR_NO_VALUE);
#endif
  /// Draws a straight line between the two points on the surface.
  import void DrawLine(int x1, int y1, int x2, int y2, int thickness=1);
  /// Draws a message from the Room Message Editor, wrapping at the specified width.
  import void DrawMessageWrapped(int x, int y, int width, FontType, int messageNumber);
  /// Changes the colour of a single pixel on the surface.
  import void DrawPixel(int x, int y);
  /// Draws a filled rectangle to the surface.
  import void DrawRectangle(int x1, int y1, int x2, int y2);
  /// Draws the specified text to the surface.
  import void DrawString(int x, int y, FontType, const string text, ...);
#ifdef SCRIPT_API_v350
  /// Draws the text to the surface, wrapping it at the specified width.
  import void DrawStringWrapped(int x, int y, int width, FontType, HorizontalAlignment, const string text);
#endif
#ifndef SCRIPT_API_v350
  /// Draws the text to the surface, wrapping it at the specified width.
  import void DrawStringWrapped(int x, int y, int width, FontType, Alignment, const string text);
#endif
#ifdef SCRIPT_API_v399
  /// Draws the specified surface onto this surface.
  import void DrawSurface(DrawingSurface *surfaceToDraw, int transparency=0, int x=0, int y=0, int width=SCR_NO_VALUE, int height=SCR_NO_VALUE,
						int part_x=0, int part_y=0, int part_width=SCR_NO_VALUE, int part_height=SCR_NO_VALUE);
  /// Blends the specified surface onto this surface.
  import void BlendSurface(DrawingSurface *surfaceToDraw, BlendMode mode, int transparency=0, int x=0, int y=0, int width=SCR_NO_VALUE, int height=SCR_NO_VALUE,
						int part_x=0, int part_y=0, int part_width=SCR_NO_VALUE, int part_height=SCR_NO_VALUE);
#endif
#ifndef SCRIPT_API_v399
  /// Draws the specified surface onto this surface.
  import void DrawSurface(DrawingSurface *surfaceToDraw, int transparency=0);
#endif
  /// Draws a filled triangle onto the surface.
  import void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
  /// Gets the colour of a single pixel on the surface.
  import int  GetPixel(int x, int y);
  /// Tells AGS that you have finished drawing onto the surface.
  import void Release();
  /// Gets/sets the current AGS Colour Number that will be used for drawing onto this surface.
  import attribute int DrawingColor;
  /// Gets the height of this surface.
  readonly import attribute int Height;
  /// Gets the width of the surface.
  readonly import attribute int Width;
};

#ifdef SCRIPT_API_v3507
builtin managed struct Camera;
builtin managed struct Viewport;
#endif

builtin struct Room {
  /// Gets a custom text property associated with this room.
  import static String GetTextProperty(const string property);
  /// Gets a drawing surface that allows you to manipulate the room background.
  import static DrawingSurface* GetDrawingSurfaceForBackground(int backgroundNumber=SCR_NO_VALUE);
  /// Gets the Y co-ordinate of the bottom edge of the room.
  readonly import static attribute int BottomEdge;
  /// Gets the colour depth of the room background.
  readonly import static attribute int ColorDepth;
  /// Gets the height of the room background.
  readonly import static attribute int Height;
  /// Gets the X co-ordinate of the left edge of the room.
  readonly import static attribute int LeftEdge;
  /// Accesses room messages, as set up in the Room Message Editor.
  readonly import static attribute String Messages[];
  /// Gets the music that is played when the player enters this room.
  readonly import static attribute int MusicOnLoad;
  /// Gets the number of objects in this room.
  readonly import static attribute int ObjectCount;
  /// Gets the X co-ordinate of the right edge of the room.
  readonly import static attribute int RightEdge;
  /// Gets the Y co-ordinate of the top edge of the room.
  readonly import static attribute int TopEdge;
  /// Gets the width of the room background.
  readonly import static attribute int Width;
  /// Gets a Custom Property associated with this room.
  import static int GetProperty(const string property);
  /// Sets an integer custom property associated with this room.
  import static bool SetProperty(const string property, int value);
  /// Sets a text custom property associated with this room.
  import static bool SetTextProperty(const string property, const string value);
  /// Performs default processing of a mouse click at the specified co-ordinates.
  import static void ProcessClick(int x, int y, CursorMode);
#ifdef SCRIPT_API_v399
  /// Checks if the specified room exists
  import static bool Exists(int room);   // $AUTOCOMPLETESTATICONLY$
#endif
};

builtin struct Parser {
  /// Returns the parser dictionary word ID for the specified word
  import static int    FindWordID(const string wordToFind);
  /// Stores the supplied user text for later use with Said
  import static void   ParseText(const string text);
  /// Checks whether the player's input matched this text.
  import static bool   Said(const string text);
  /// Gets any word that the player typed that was not in the game dictionary.
  import static String SaidUnknownWord();
};

// standard functions
/// Displays the text in a standard text window.
import void Display(const string message, ...);
/// Displays the text in a standard text window at the specified location.
import void DisplayAt(int x, int y, int width, const string message, ...);
/// Displays the text in a standard text window at the specified y-coordinate.
import void DisplayAtY (int y, const string message);
/// Displays a message from the Room Message Editor.
import void DisplayMessage(int messageNumber);
/// Displays a message from the Room Message Editor at the specified y-coordinate.
import void DisplayMessageAtY(int messageNumber, int y);
/// Displays a message in a text window with a title, used for speech in SCI0 games.
import void DisplayTopBar(int y, int textColor, int backColor, const string title, const string text, ...);
/// Displays a Room Message Editor message in a text window with a title, used for speech in SCI0 games.
import void DisplayMessageBar(int y, int textColor, int backColor, const string title, int message);
/// Resets the room state back to how it was initially set up in the editor.
import void ResetRoom(int roomNumber);
/// Checks whether the player has been in the specified room yet.
import int  HasPlayerBeenInRoom(int roomNumber);
/// Exits the game with an error message.
import void AbortGame(const string message, ...);
/// Quits the game, optionally showing a confirmation dialog.
import void QuitGame(int promptUser);
/// Changes the current game speed.
import void SetGameSpeed(int framesPerSecond);
/// Gets the current game speed.
import int  GetGameSpeed();
/// Changes a game option; see the manual for details.
import int  SetGameOption(int option, int value);
/// Gets the current value of a game option.
import int  GetGameOption(int option);
/// Performs various debugging commands.
import void Debug(int command, int data);
/// Calls the on_call function in the current room.
import void CallRoomScript(int value);
/// Transfers gameplay into a separate AGS game.
import int  RunAGSGame(const string filename, int mode, int data);
/// Gets the translated version of the specified text.
import const string GetTranslation (const string originalText);
/// Checks if a translation is currently in use.
import int  IsTranslationAvailable ();
/// Displays the default built-in Restore Game dialog.
import void RestoreGameDialog();
/// Displays the default built-in Save Game dialog.
import void SaveGameDialog();
/// Restarts the game from the restart point.
import void RestartGame();
/// Saves the current game position to the specified slot.
import void SaveGameSlot(int slot, const string description);
/// Restores the game saved to the specified game slot.
import void RestoreGameSlot(int slot);
/// Deletes the specified save game.
import void DeleteSaveSlot(int slot);
/// Sets this as the point at which the game will be restarted.
import void SetRestartPoint();
/// Gets what type of thing is in the room at the specified co-ordinates.
import LocationType GetLocationType(int x, int y);
#ifdef SCRIPT_COMPAT_v350
/// Returns which walkable area is at the specified position on screen.
import int  GetWalkableAreaAt(int screenX, int screenY);
#endif
#ifdef SCRIPT_API_v3507
/// Returns which walkable area is at the specified position on screen.
import int  GetWalkableAreaAtScreen(int screenX, int screenY);
/// Returns which walkable area is at the specified position within the room.
import int  GetWalkableAreaAtRoom(int roomX, int roomY);
#endif
#ifdef SCRIPT_API_v399
/// Gets the drawing surface for the 8-bit walkable mask
import DrawingSurface* GetDrawingSurfaceForWalkableArea();
/// Gets the drawing surface for the 8-bit walk-behind mask
import DrawingSurface* GetDrawingSurfaceForWalkbehind();
#endif
/// Returns the scaling level at the specified position within the room.
import int  GetScalingAt (int x, int y);
#ifdef SCRIPT_COMPAT_v350
/// Locks the viewport to stop the screen scrolling automatically.
import void SetViewport(int x, int y);
/// Allows AGS to scroll the screen automatically to follow the player character.
import void ReleaseViewport();
/// Gets the current X offset of the scrolled viewport.
import int  GetViewportX();
/// Gets the current Y offset of the scrolled viewport.
import int  GetViewportY();
#endif
/// Returns whether the game is currently paused.
import int  IsGamePaused();
import int  GetGraphicalVariable (const string variableName);
import void SetGraphicalVariable (const string variableName, int value);
/// Disables the player interface and activates the Wait cursor.
import void DisableInterface();
/// Re-enables the player interface.
import void EnableInterface();
/// Checks whether the player interface is currently enabled.
import int  IsInterfaceEnabled();

struct Mouse {
  /// Changes the sprite for the specified mouse cursor.
  import static void ChangeModeGraphic(CursorMode, int slot);
  /// Changes the active hotspot for the specified mouse cursor.
  import static void ChangeModeHotspot(CursorMode, int x, int y);
  /// Changes the view used to animate the specified mouse cursor.
  import static void ChangeModeView(CursorMode, int view);
  /// Disables the specified cursor mode.
  import static void DisableMode(CursorMode);
  /// Re-enables the specified cursor mode.
  import static void EnableMode(CursorMode);
  /// Gets the sprite used for the specified mouse cursor.
  import static int  GetModeGraphic(CursorMode);
  /// Checks whether the specified mouse button is currently pressed.
  import static bool IsButtonDown(MouseButton);
  /// Remembers the current mouse cursor and restores it when the mouse leaves the current area.
  import static void SaveCursorUntilItLeaves();
  /// Cycles to the next available mouse cursor.
  import static void SelectNextMode();
  /// Cycles to the previous available mouse cursor.
  import static void SelectPreviousMode();
  /// Restricts the mouse movement to the specified area.
  import static void SetBounds(int left, int top, int right, int bottom);
  /// Moves the mouse cursor to the specified location.
  import static void SetPosition(int x, int y);
  /// Updates the X and Y co-ordinates to match the current mouse position.
  import static void Update();
  /// Changes the current mouse cursor back to the default for the current mode.
  import static void UseDefaultGraphic();
  /// Changes the mouse cursor to use the graphic for a different non-active cursor mode.
  import static void UseModeGraphic(CursorMode);
  /// Gets/sets the current mouse cursor mode.
  import static attribute CursorMode Mode;
  /// Gets/sets whether the mouse cursor is visible.
  import static attribute bool Visible;
  /// Gets/sets whether the user-defined factors are applied to mouse movement
  import static attribute bool ControlEnabled;
  /// Gets/sets the mouse speed
  import static attribute float Speed;
  /// Fires mouse click event at current mouse position.
  import static void Click(MouseButton);
  /// Returns whether the specified mouse cursor is currently enabled.
  import static bool IsModeEnabled(CursorMode);
  /// Gets the current mouse position.
  readonly int  x,y;
};

import int  Said (const string text);

/// Gets the width of the specified text in the specified font
import int  GetTextWidth(const string text, FontType);
/// Gets the height of the specified text in the specified font when wrapped at the specified width
import int  GetTextHeight(const string text, FontType, int width);
/// Gets the font's height, in pixels
import int  GetFontHeight(FontType);
/// Gets the default step between two lines of text for the specified font
import int  GetFontLineSpacing(FontType);
/// Adds to the player's score and plays the score sound, if set.
import void GiveScore(int points);
/// Refreshes the on-screen inventory display.
import void UpdateInventory();
/// From within dialog_request, tells AGS not to return to the dialog after this function ends.
import void StopDialog();
/// Determines whether two objects or characters are overlapping each other.
import int  AreThingsOverlapping(int thing1, int thing2);

#ifdef SCRIPT_COMPAT_v321
/// Sets whether voice and/or text are used in the game.
import void SetVoiceMode(eVoiceMode);
/// Sets how the player can skip speech lines.
import void SetSkipSpeech(int skipFlag);
/// Changes the style in which speech is displayed.
import void SetSpeechStyle(eSpeechStyle);
#endif

/// Starts a timer, which will expire after the specified number of game loops.
import void SetTimer(int timerID, int timeout);
/// Returns true the first time this is called after the timer expires.
import bool IsTimerExpired(int timerID);
/// Sets whether the game can continue to run in the background if the player switches to another application.
import void SetMultitaskingMode (int mode);
/// Converts a floating point value to an integer.
import int  FloatToInt(float value, RoundDirection=eRoundDown);
/// Converts an integer to a floating point number.
import float IntToFloat(int value);

// File I/O
enum FileMode {
  eFileRead = 1,
  eFileWrite = 2,
  eFileAppend = 3
};

enum FileSeek {
  eSeekBegin = 0,
  eSeekCurrent = 1,
  eSeekEnd = 2
};

builtin managed struct File {
  /// Delets the specified file from the disk.
  import static bool Delete(const string filename);   // $AUTOCOMPLETESTATICONLY$
  /// Checks if the specified file exists on the disk.
  import static bool Exists(const string filename);   // $AUTOCOMPLETESTATICONLY$
  /// Opens the specified file in order to read from or write to it.
  import static File *Open(const string filename, FileMode);   // $AUTOCOMPLETESTATICONLY$
  /// Closes the file.
  import void Close();
  /// Reads an integer value from the file.
  import int  ReadInt();
  /// Reads the next raw byte from the file.
  import int  ReadRawChar();
  /// Reads the next raw 32-bit int from the file.
  import int  ReadRawInt();
  /// Reads the next raw line of text from the file.
  import String ReadRawLineBack();
  /// Reads the next string from the file.
  import String ReadStringBack();
  /// Writes an integer to the file.
  import void WriteInt(int value);
  /// Writes a raw byte to the file.
  import void WriteRawChar(int value);
  /// Writes a raw line of text to the file.
  import void WriteRawLine(const string text);
  /// Writes a string to the file.
  import void WriteString(const string text);
  /// Gets whether you have reached the end of the file.
  readonly import attribute bool EOF;
  /// Gets whether any errors occurred reading or writing the file.
  readonly import attribute bool Error;
  /// Moves file cursor by specified offset, returns new position.
  import int Seek(int offset, FileSeek origin = eSeekCurrent);
  /// Gets current cursor position inside the file.
  readonly import attribute int Position;
  readonly int reserved[2];   // $AUTOCOMPLETEIGNORE$
};

builtin managed struct InventoryItem {
  /// Returns the inventory item at the specified location.
  import static InventoryItem* GetAtScreenXY(int x, int y);    // $AUTOCOMPLETESTATICONLY$
  /// Gets an integer custom property for this item.
  import int  GetProperty(const string property);
  /// Gets a text custom property for this item.
  import String GetTextProperty(const string property);
  /// Checks whether an event handler has been registered for clicking on this item in the specified cursor mode.
  import int  IsInteractionAvailable(CursorMode);
  /// Runs the registered event handler for this item.
  import void RunInteraction(CursorMode);
  /// Gets/sets the sprite used as the item's mouse cursor.
  import attribute int  CursorGraphic;
  /// Gets/sets the sprite used to display the inventory item.
  import attribute int  Graphic;
  /// Gets the ID number of the inventory item.
  readonly import attribute int ID;
  /// Gets/sets the name of the inventory item.
  import attribute String Name;
  /// Sets an integer custom property for this item.
  import bool SetProperty(const string property, int value);
  /// Sets a text custom property for this item.
  import bool SetTextProperty(const string property, const string value);
  readonly int reserved[2];   // $AUTOCOMPLETEIGNORE$
};

builtin managed struct Overlay {
  /// Creates an overlay that displays a sprite.
  import static Overlay* CreateGraphical(int x, int y, int slot, bool transparent);  // $AUTOCOMPLETESTATICONLY$
  /// Creates an overlay that displays some text.
  import static Overlay* CreateTextual(int x, int y, int width, FontType, int colour, const string text, ...);  // $AUTOCOMPLETESTATICONLY$
  /// Changes the text on the overlay.
  import void SetText(int width, FontType, int colour, const string text, ...);
  /// Removes the overlay from the screen.
  import void Remove();
  /// Checks whether this overlay is currently valid.
  readonly import attribute bool Valid;
  /// Gets/sets the X position on the screen where this overlay is displayed.
  import attribute int X;
  /// Gets/sets the Y position on the screen where this overlay is displayed.
  import attribute int Y;
#ifdef SCRIPT_API_v399
  /// Gets/sets the blending mode of this overlay.
  import attribute BlendMode BlendMode;
  /// Gets/sets the overlay's image rotation in degrees.
  import attribute float Rotation;
  /// Gets/sets the transparency of this overlay.
  import attribute int Transparency;
  /// Gets/sets the overlay's z-order relative to other overlays and on-screen objects.
  import attribute int ZOrder;
#endif
};

builtin managed struct DynamicSprite {
  /// Creates a blank dynamic sprite of the specified size.
  import static DynamicSprite* Create(int width, int height, bool hasAlphaChannel=false);    // $AUTOCOMPLETESTATICONLY$
  /// Creates a dynamic sprite as a copy of a room background.
  import static DynamicSprite* CreateFromBackground(int frame=SCR_NO_VALUE, int x=SCR_NO_VALUE, int y=SCR_NO_VALUE, int width=SCR_NO_VALUE, int height=SCR_NO_VALUE);    // $AUTOCOMPLETESTATICONLY$
  /// Creates a dynamic sprite as a copy of a drawing surface.
  import static DynamicSprite* CreateFromDrawingSurface(DrawingSurface* surface, int x, int y, int width, int height);    // $AUTOCOMPLETESTATICONLY$
  /// Creates a dynamic sprite as a copy of an existing sprite.
  import static DynamicSprite* CreateFromExistingSprite(int slot, bool preserveAlphaChannel=0);    // $AUTOCOMPLETESTATICONLY$
  /// Creates a dynamic sprite from a BMP or PCX file.
  import static DynamicSprite* CreateFromFile(const string filename);              // $AUTOCOMPLETESTATICONLY$
  /// Creates a dynamic sprite from a save game screenshot.
  import static DynamicSprite* CreateFromSaveGame(int slot, int width, int height);  // $AUTOCOMPLETESTATICONLY$
  /// Creates a dynamic sprite as a copy of the current screen.
  import static DynamicSprite* CreateFromScreenShot(int width=0, int height=0);  // $AUTOCOMPLETESTATICONLY$
  /// Enlarges the size of the sprite, but does not resize the image.
  import void ChangeCanvasSize(int width, int height, int x, int y);
  /// Copies the transparency mask and/or alpha channel from the specified sprite onto this dynamic sprite.
  import void CopyTransparencyMask(int fromSpriteSlot);
  /// Reduces the size of the sprite, but does not resize the image.
  import void Crop(int x, int y, int width, int height);
  /// Deletes the dynamic sprite from memory when you no longer need it.
  import void Delete();
  /// Flips the sprite in the specified direction.
  import void Flip(eFlipDirection);
  /// Gets a drawing surface that can be used to manually draw onto the sprite.
  import DrawingSurface* GetDrawingSurface();
  /// Resizes the sprite.
  import void Resize(int width, int height);
  /// Rotates the sprite by the specified number of degrees.
  import void Rotate(int angle, int width=SCR_NO_VALUE, int height=SCR_NO_VALUE);
  /// Saves the sprite to a BMP or PCX file.
  import int  SaveToFile(const string filename);
  /// Permanently tints the sprite to the specified colour.
  import void Tint(int red, int green, int blue, int saturation, int luminance);
  /// Gets the colour depth of this sprite.
  readonly import attribute int ColorDepth;
  /// Gets the sprite number of this dynamic sprite, which you can use to display it in the game.
  readonly import attribute int Graphic;
  /// Gets the height of this sprite.
  readonly import attribute int Height;
  /// Gets the width of this sprite.
  readonly import attribute int Width;
};

// Palette FX
/// Fades the screen in from black to the normal palette.
import void FadeIn(int speed);
/// Fades the screen out to the current fade colour.
import void FadeOut(int speed);
/// Cycles the palette entries between start and end. (8-bit games only)
import void CyclePalette(int start, int end);
/// Changes the RGB colour of a palette slot. (8-bit games only)
import void SetPalRGB(int slot, int r, int g, int b);
/// Updates the screen with manual changes to the palette. (8-bit games only)
import void UpdatePalette();
/// Tints the whole screen to the specified colour.
import void TintScreen (int red, int green, int blue);
/// Sets an ambient tint that affects all objects and characters in the room.
import void SetAmbientTint(int red, int green, int blue, int saturation, int luminance);
/// Returns a random number between 0 and MAX, inclusive.
import int  Random(int max);
/// Locks the current room to the specified background.
import void SetBackgroundFrame(int frame);
/// Gets the current background frame number.
import int  GetBackgroundFrame();
/// Shakes the screen by the specified amount.
import void ShakeScreen(int amount);
/// Shakes the screen but does not pause the game while it does so.
import void ShakeScreenBackground(int delay, int amount, int length);
/// Changes the room transition style.
import void SetScreenTransition(TransitionStyle);
/// Changes the room transition style for the next room change only.
import void SetNextScreenTransition(TransitionStyle);
/// Changes the colour to which the screen fades out with a FadeOut call.
import void SetFadeColor(int red, int green, int blue);
/// Checks whether an event handler is registered to handle clicking at the specified location on the screen.
import int  IsInteractionAvailable (int x, int y, CursorMode);
/// Removes the specified walkable area from the room.
import void RemoveWalkableArea(int area);
/// Brings back a previously removed walkable area.
import void RestoreWalkableArea(int area);
/// Changes the specified walkable area's scaling level.
import void SetAreaScaling(int area, int min, int max);
/// Disables all region events, and optionally light levels and tints.
import void DisableGroundLevelAreas(int disableTints);
/// Re-enables region events, light levels and tints.
import void EnableGroundLevelAreas();
/// Changes the baseline of the specified walk-behind area.
import void SetWalkBehindBase(int area, int baseline);
/// Plays a FLI/FLC animation.
import void PlayFlic(int flcNumber, int options);
/// Plays an AVI/MPG video.
import void PlayVideo(const string filename, VideoSkipStyle, int flags);
/// Sets an ambient light level that affects all objects and characters in the room.
import void SetAmbientLightLevel(int light_level);

import int  IsVoxAvailable();
/// Changes the voice speech volume.
import void SetSpeechVolume(int volume);
/// Checks whether a MUSIC.VOX file was found.
import int  IsMusicVoxAvailable();
/// Saves a screenshot of the current game position to a file.
import int  SaveScreenShot(const string filename);
/// Pauses the game, which stops all animations and movement.
import void PauseGame();
/// Resumes the game after it was paused earlier.
import void UnPauseGame();
/// Blocks the script for the specified number of game loops.
import void Wait(int waitLoops);
/// Blocks the script for the specified number of game loops, unless a key is pressed.
import int  WaitKey(int waitLoops = -1);
/// Blocks the script for the specified number of game loops, unless a key is pressed or the mouse is clicked.
import int  WaitMouseKey(int waitLoops = -1);
#ifdef SCRIPT_API_v399
/// Blocks the script for the specified number of game loops, unless the mouse is clicked.
import int  WaitMouse(int waitLoops = -1);
/// Cancels current Wait function, regardless of its type, if one was active at the moment.
import void SkipWait();
#endif
/// Checks whether the specified key is currently held down.
import bool IsKeyPressed(eKeyCode);
import void SetGlobalInt(int globalInt, int value);
import int  GetGlobalInt(int globalInt);
import void FlipScreen(int way);
/// Fast-forwards the game until the specified character finishes moving.
import void SkipUntilCharacterStops(CHARID);
/// Specifies the start of a skippable cutscene.
import void StartCutscene(CutsceneSkipType);
/// Specifies the end of a skippable cutscene.
import int  EndCutscene();
/// Prevents further event handlers running for this event.
import void ClaimEvent();
// Changes the GUI used to render standard game text windows.
import void SetTextWindowGUI (int gui);
import int  FindGUIID(const string);  // $AUTOCOMPLETEIGNORE$

#ifdef SCRIPT_API_v3507
/// Skip current cutscene (if one is currently in progress)
import void SkipCutscene();
#endif

enum EventType {
  eEventLeaveRoom = 1,
  eEventEnterRoomBeforeFadein = 2,
  eEventGotScore = 4,
  eEventGUIMouseDown = 5,
  eEventGUIMouseUp = 6,
  eEventAddInventory = 7,
  eEventLoseInventory = 8,
  eEventRestoreGame = 9
};

#ifdef SCRIPT_API_v350
enum GUIPopupStyle {
  eGUIPopupNormal = 0,
  eGUIPopupMouseYPos = 1,
  eGUIPopupModal = 2,
  eGUIPopupPersistent = 3
};
#endif

// forward-declare these so that they can be returned by GUIControl class
builtin managed struct GUI;
builtin managed struct Label;
builtin managed struct Button;
builtin managed struct Slider;
builtin managed struct TextBox;
builtin managed struct InvWindow;
builtin managed struct ListBox;
builtin managed struct Character;
#ifdef SCRIPT_API_v350
builtin managed struct TextWindowGUI;
#endif

builtin managed struct GUIControl {
  /// Brings this control to the front of the z-order, in front of all other controls.
  import void BringToFront();
  /// Gets the GUI Control that is visible at the specified location on the screen, or null.
  import static GUIControl* GetAtScreenXY(int x, int y);    // $AUTOCOMPLETESTATICONLY$  $AUTOCOMPLETENOINHERIT$
  /// Sends this control to the back of the z-order, behind all other controls.
  import void SendToBack();
  /// Moves the control to the specified position within the GUI.
  import void SetPosition(int x, int y);
  /// Changes the control to the specified size.
  import void SetSize(int width, int height);
  /// If this control is a button, returns the Button interface; otherwise null.
  readonly import attribute Button*  AsButton;   // $AUTOCOMPLETENOINHERIT$
  /// If this control is a inventory window, returns the InvWindow interface; otherwise null.
  readonly import attribute InvWindow* AsInvWindow;  // $AUTOCOMPLETENOINHERIT$
  /// If this control is a label, returns the Label interface; otherwise null.
  readonly import attribute Label*   AsLabel;    // $AUTOCOMPLETENOINHERIT$
  /// If this control is a list box, returns the ListBox interface; otherwise null.
  readonly import attribute ListBox* AsListBox;  // $AUTOCOMPLETENOINHERIT$
  /// If this control is a slider, returns the Slider interface; otherwise null.
  readonly import attribute Slider*  AsSlider;   // $AUTOCOMPLETENOINHERIT$
  /// If this control is a text box, returns the TextBox interface; otherwise null.
  readonly import attribute TextBox* AsTextBox;  // $AUTOCOMPLETENOINHERIT$
  /// Gets/sets whether this control can be clicked on or whether clicks pass straight through it.
  import attribute bool Clickable;
  /// Gets/sets whether this control is currently enabled.
  import attribute bool Enabled;
  /// Gets/sets the height of the control.
  import attribute int  Height;
  /// Gets the ID number of the control within its owning GUI.
  readonly import attribute int  ID;
  /// Gets the GUI that this control is placed onto.
  readonly import attribute GUI* OwningGUI;
  /// Gets/sets whether this control is currently visible.
  import attribute bool Visible;
  /// Gets/sets the width of the control.
  import attribute int  Width;
  /// Gets/sets the X position of the control's top-left corner.
  import attribute int  X;
  /// Gets/sets the Y position of the control's top-left corner.
  import attribute int  Y;
  /// Gets/sets the control's z-order relative to other controls within the same owning GUI.
  import attribute int  ZOrder;
};

builtin managed struct Label extends GUIControl {
  /// Gets/sets the font that is used to draw the label text.
  import attribute FontType Font;
  /// Gets/sets the text that is shown on the label.
  import attribute String Text;
  /// Gets/sets the colour in which the label text is drawn.
  import attribute int  TextColor;
#ifdef SCRIPT_API_v350
  /// Gets/sets label's text alignment.
  import attribute HorizontalAlignment TextAlignment;
#endif
};

builtin managed struct Button extends GUIControl {
  /// Animates the button graphic using the specified view loop.
  import void Animate(int view, int loop, int delay, RepeatStyle);
  /// Gets/sets whether the image is clipped to the size of the control.
  import attribute bool ClipImage;
  /// Gets/sets the font used to display text on the button.
  import attribute FontType Font;
  /// Gets the currently displayed sprite number.
  readonly import attribute int  Graphic;
  /// Gets/sets the image that is shown when the player moves the mouse over the button (-1 if none)
  import attribute int  MouseOverGraphic;
  /// Gets/sets the image that is shown when the button is not pressed or mouse-overed
  import attribute int  NormalGraphic;
  /// Gets/sets the image that is shown when the button is pressed
  import attribute int  PushedGraphic;
  /// Gets/sets the colour in which the button text is drawn.
  import attribute int  TextColor;
  /// Gets/sets the text to be drawn on the button.
  import attribute String Text;
  /// Runs the OnClick event handler for this Button.
  import void Click(MouseButton);
  /// Gets whether the button is currently animating.
  readonly import attribute bool Animating;
  /// Gets the current frame number during an animation.
  readonly import attribute int  Frame;
  /// Gets the current loop number during an animation.
  readonly import attribute int  Loop;
  /// Gets the current view number during an animation.
  readonly import attribute int  View;
#ifdef SCRIPT_API_v350
  /// Gets/sets text alignment inside the button.
  import attribute Alignment TextAlignment;
#endif
};

builtin managed struct Slider extends GUIControl {
  /// Gets/sets the image that is tiled to make up the background of the slider.
  import attribute int  BackgroundGraphic;
  /// Gets/sets the image used for the 'handle' that represents the current slider position.
  import attribute int  HandleGraphic;
  /// Gets/sets the pixel offset at which the handle is drawn.
  import attribute int  HandleOffset;
  /// Gets/sets the maximum value that the slider can have.
  import attribute int  Max;
  /// Gets/sets the minimum value that the slider can have.
  import attribute int  Min;
  /// Gets/sets the current value of the slider.
  import attribute int  Value;
};

builtin managed struct TextBox extends GUIControl {
  /// Gets/sets the font used to draw the text in the text box.
  import attribute FontType Font;
  /// Gets/sets the text that is currently in the text box.
  import attribute String Text;
  /// Gets/sets the color of the text in the text box.
  import attribute int TextColor;
#ifdef SCRIPT_API_v350
  /// Gets/sets whether the border around the text box is shown.
  import attribute bool ShowBorder;
#endif
};

builtin managed struct InvWindow extends GUIControl {
  /// Scrolls the inventory window down one row.
  import void ScrollDown();
  /// Scrolls the inventory window up one row.
  import void ScrollUp();
  /// Gets/sets which character's inventory is displayed in this window.
  import attribute Character* CharacterToUse;
  /// Gets the inventory item at the specified index in the window.
  readonly import attribute InventoryItem* ItemAtIndex[];
  /// Gets the number of inventory items currently shown in the window.
  readonly import attribute int ItemCount;
  /// Gets the height of each row of items.
  import attribute int ItemHeight;
  /// Gets the width of each column of items.
  import attribute int ItemWidth;
  /// Gets the index of the first visible item in the window.
  import attribute int TopItem;
  /// Gets the number of items shown per row in this inventory window.
  readonly import attribute int ItemsPerRow;
  /// Gets the number of visible rows in this inventory window.
  readonly import attribute int RowCount;
};

builtin managed struct ListBox extends GUIControl {
	/// Adds a new item to the bottom of the list with the specified text.
	import bool AddItem(const string text);
	/// Removes all the items from the list.
	import void Clear();
	/// Fills the list box with all the filenames that match the specified file mask.
	import void FillDirList(const string fileMask);
	/// Fills the list box with all the current user's saved games.
	import int  FillSaveGameList();
	/// Gets the item index at the specified screen co-ordinates, if they lie within the list box.
	import int  GetItemAtLocation(int x, int y);
	/// Inserts a new item before the specified index.
	import bool InsertItemAt(int listIndex, const string text);
	/// Removes the specified item from the list.
	import void RemoveItem(int listIndex);
	/// Scrolls the list down one row.
	import void ScrollDown();
	/// Scrolls the list up one row.
	import void ScrollUp();
	/// Gets/sets the font used to draw the list items.
	import attribute FontType Font;
#ifdef SCRIPT_COMPAT_v341
	/// Gets/sets whether the border around the list box is hidden.
	import attribute bool HideBorder;
	/// Gets/sets whether the clickable scroll arrows are hidden.
	import attribute bool HideScrollArrows;
#endif
	/// Gets the number of items currently in the list.
	readonly import attribute int ItemCount;
	/// Accesses the text for the items in the list.
	import attribute String Items[];
	/// Gets the number of visible rows that the listbox can display.
	readonly import attribute int RowCount;
	/// Gets the save game number that each row in the list corresponds to, after using FillSaveGameList.
	readonly import attribute int SaveGameSlots[];
	/// Gets/sets the currently selected item.
	import attribute int  SelectedIndex;
	/// Gets/sets the first visible item in the list.
	import attribute int  TopItem;
#ifdef SCRIPT_API_v350
	/// Gets/sets whether the border around the list box is shown.
	import attribute bool ShowBorder;
	/// Gets/sets whether the clickable scroll arrows are shown.
	import attribute bool ShowScrollArrows;
	/// Gets/sets color of the list item's selection
	import attribute int  SelectedBackColor;
	/// Gets/sets selected list item's text color
	import attribute int  SelectedTextColor;
	/// Gets/sets list item's text alignment.
	import attribute HorizontalAlignment TextAlignment;
	/// Gets/sets regular list item's text color
	import attribute int  TextColor;
#endif
};

builtin managed struct GUI {
  /// Moves the GUI to be centred on the screen.
  import void Centre();
  /// Gets the topmost GUI visible on the screen at the specified co-ordinates.
  import static GUI* GetAtScreenXY(int x, int y);    // $AUTOCOMPLETESTATICONLY$
  /// Moves the GUI to have its top-left corner at the specified position.
  import void SetPosition(int x, int y);
  /// Changes the size of the GUI.
  import void SetSize(int width, int height);
  /// Gets/sets the sprite used to draw the GUI's background image.
  import attribute int  BackgroundGraphic;
  /// Gets/sets whether the GUI can be clicked on, or whether clicks pass straight through.
  import attribute bool Clickable;
  /// Accesses the controls that are on this GUI.
  readonly import attribute GUIControl *Controls[];
  /// Gets the number of controls on this GUI.
  readonly import attribute int  ControlCount;
  /// Gets/sets the height of the GUI.
  import attribute int  Height;
  /// Gets the ID number of the GUI.
  readonly import attribute int  ID;
  /// Gets/sets the transparency of the GUI.
  import attribute int  Transparency;
  /// Gets/sets whether the GUI is visible.
  import attribute bool Visible;
  /// Gets/sets the width of the GUI.
  import attribute int  Width;
  /// Gets/sets the X co-ordinate of the GUI's top-left corner.
  import attribute int  X;
  /// Gets/sets the Y co-ordinate of the GUI's top-left corner.
  import attribute int  Y;
  /// Gets/sets the GUI's z-order relative to other GUIs.
  import attribute int  ZOrder;
  /// Runs the OnClick event handler for this GUI.
  import void Click(MouseButton);
  /// Performs default processing of a mouse click at the specified co-ordinates.
  import static void ProcessClick(int x, int y, MouseButton);
  readonly int reserved[2];   // $AUTOCOMPLETEIGNORE$
#ifdef SCRIPT_API_v350
  /// Gets/sets the background color.
  import attribute int  BackgroundColor;
  /// Gets/sets the border color. Not applicable to TextWindow GUIs.
  import attribute int  BorderColor;
  /// If this GUI is a TextWindow, returns the TextWindowGUI interface; otherwise null.
  import readonly attribute TextWindowGUI* AsTextWindow; // $AUTOCOMPLETENOINHERIT$
  /// Gets the style of GUI behavior on screen.
  import readonly attribute GUIPopupStyle PopupStyle;
  /// Gets/sets the Y co-ordinate at which the GUI will appear when using MouseYPos popup style.
  import attribute int  PopupYPos;
#endif
#ifdef SCRIPT_API_v351
  /// Gets if this GUI is currently active on screen. In certain cases this is different than reading Visible property.
  import readonly attribute bool Shown;
#endif
#ifdef SCRIPT_API_v399
  /// Gets/sets the blending mode for this GUI.
  import attribute BlendMode BlendMode;
  /// Gets/sets the GUI's image rotation in degrees.
  import attribute float Rotation;
#endif
};

#ifdef SCRIPT_API_v350
builtin managed struct TextWindowGUI extends GUI {
  /// Gets/sets the text color.
  import attribute int  TextColor;
  /// Gets/sets the amount of padding, in pixels, surrounding the text in the TextWindow.
  import attribute int  TextPadding;
};
#endif

builtin managed struct Hotspot {
  /// Gets the hotspot that is at the specified position on the screen.
  import static Hotspot* GetAtScreenXY(int x, int y);    // $AUTOCOMPLETESTATICONLY$
  /// Gets an integer Custom Property for this hotspot.
  import int  GetProperty(const string property);
  /// Gets a text Custom Property for this hotspot.
  import String GetTextProperty(const string property);
  /// Runs the specified event handler for this hotspot.
  import void RunInteraction(CursorMode);
  /// Gets/sets whether this hotspot is enabled.
  import attribute bool Enabled;
  /// Gets the ID of the hotspot.
  readonly import attribute int ID;
  /// Gets the name of the hotspot.
  readonly import attribute String Name;
  /// Gets the X co-ordinate of the walk-to point for this hotspot.
  readonly import attribute int WalkToX;
  /// Gets the Y co-ordinate of the walk-to point for this hotspot.
  readonly import attribute int WalkToY;
  /// Checks whether an event handler has been registered for clicking on this hotspot in the specified cursor mode.
  import bool IsInteractionAvailable(CursorMode);
  /// Sets an integer custom property for this hotspot.
  import bool SetProperty(const string property, int value);
  /// Sets a text custom property for this hotspot.
  import bool SetTextProperty(const string property, const string value);
#ifdef SCRIPT_API_v3507
  /// Returns the hotspot at the specified position within this room.
  import static Hotspot* GetAtRoomXY(int x, int y);      // $AUTOCOMPLETESTATICONLY$
#endif
#ifdef SCRIPT_API_v399
  /// Gets the drawing surface for the 8-bit hotspots mask
  import static DrawingSurface* GetDrawingSurface();     // $AUTOCOMPLETESTATICONLY$
#endif
  int reserved[2];   // $AUTOCOMPLETEIGNORE$
};

builtin managed struct Region {
  /// Returns the region at the specified position within this room.
  import static Region* GetAtRoomXY(int x, int y);    // $AUTOCOMPLETESTATICONLY$
  /// Runs the event handler for the specified event for this region.
  import void RunInteraction(int event);
  /// Sets the region tint which will apply to characters that are standing on the region.
  import void Tint(int red, int green, int blue, int amount, int luminance = 100);
  /// Gets/sets whether this region is enabled.
  import attribute bool Enabled;
  /// Gets the ID number for this region.
  readonly import attribute int ID;
  /// Gets/sets the light level for this region.
  import attribute int  LightLevel;
  /// Gets whether a colour tint is set for this region.
  readonly import attribute bool TintEnabled;
  /// Gets the Blue component of this region's colour tint.
  readonly import attribute int  TintBlue;
  /// Gets the Green component of this region's colour tint.
  readonly import attribute int  TintGreen;
  /// Gets the Red component of this region's colour tint.
  readonly import attribute int  TintRed;
  /// Gets the Saturation of this region's colour tint.
  readonly import attribute int  TintSaturation;
  /// Gets the Luminance of this region's colour tint.
  readonly import attribute int  TintLuminance;
#ifdef SCRIPT_API_v3507
  /// Returns the region at the specified position on the screen.
  import static Region* GetAtScreenXY(int x, int y);    // $AUTOCOMPLETESTATICONLY$
#endif
#ifdef SCRIPT_API_v399
  /// Gets the drawing surface for the 8-bit regions mask
  import static DrawingSurface* GetDrawingSurface();  // $AUTOCOMPLETESTATICONLY$
#endif
  int reserved[2];   // $AUTOCOMPLETEIGNORE$
};

builtin managed struct Dialog {
  /// Displays the options for this dialog and returns which one the player selected.
  import int DisplayOptions(DialogOptionSayStyle = eSayUseOptionSetting);
  /// Gets the enabled state for the specified option in this dialog.
  import DialogOptionState GetOptionState(int option);
  /// Gets the text of the specified option in this dialog.
  import String GetOptionText(int option);
  /// Checks whether the player has chosen this option before.
  import bool HasOptionBeenChosen(int option);
  /// Sets the enabled state of the specified option in this dialog.
  import void SetOptionState(int option, DialogOptionState);
  /// Runs the dialog interactively.
  import void Start();
  /// Gets the dialog's ID number for interoperating with legacy code.
  readonly import attribute int ID;
  /// Gets the number of options that this dialog has.
  readonly import attribute int OptionCount;
  /// Gets whether this dialog allows the player to type in text.
  readonly import attribute bool ShowTextParser;
  /// Manually marks whether the option was chosen before or not.
  import void SetHasOptionBeenChosen(int option, bool chosen);
  
  readonly int reserved[2];   // $AUTOCOMPLETEIGNORE$
};

#define IsSpeechVoxAvailable IsVoxAvailable
//import int IsSpeechVoxAvailable();  // make autocomplete recognise

builtin struct Maths {
  /// Calculates the Arc Cosine of the specified value.
  import static float ArcCos(float value);
  /// Calculates the Arc Sine of the specified value.
  import static float ArcSin(float value);
  /// Calculates the Arc Tan of the specified value.
  import static float ArcTan(float value);
  /// Calculates the Arc Tan of y/x.
  import static float ArcTan2(float y, float x);
  /// Calculates the cosine of the specified angle.
  import static float Cos(float radians);
  /// Calculates the hyperbolic cosine of the specified angle.
  import static float Cosh(float radians);
  /// Converts the angle from degrees to radians.
  import static float DegreesToRadians(float degrees);
  /// Calculates the value of e to the power x.
  import static float Exp(float x);
  /// Calculates the natural logarithm (base e) of x.
  import static float Log(float x);
  /// Calculates the base-10 logarithm of x.
  import static float Log10(float x);
  /// Converts the angle from radians to degrees.
  import static float RadiansToDegrees(float radians);
  /// Calculates the base raised to the power of the exponent.
  import static float RaiseToPower(float base, float exponent);
  /// Calculates the sine of the angle.
  import static float Sin(float radians);
  /// Calculates the hyperbolic sine of the specified angle.
  import static float Sinh(float radians);
  /// Calculates the square root of the value.
  import static float Sqrt(float value);
  /// Calculates the tangent of the angle.
  import static float Tan(float radians);
  /// Calculates the hyperbolic tangent of the specified angle.
  import static float Tanh(float radians);
  /// Gets the value of PI
  readonly import static attribute float Pi;
};

builtin managed struct DateTime {
  /// Gets the current date and time on the player's system.
  readonly import static attribute DateTime* Now;   // $AUTOCOMPLETESTATICONLY$
  /// Gets the Year component of the date.
  readonly import attribute int Year;
  /// Gets the Month (1-12) component of the date.
  readonly import attribute int Month;
  /// Gets the DayOfMonth (1-31) component of the date.
  readonly import attribute int DayOfMonth;
  /// Gets the Hour (0-23) component of the time.
  readonly import attribute int Hour;
  /// Gets the Minute (0-59) component of the time.
  readonly import attribute int Minute;
  /// Gets the Second (0-59) component of the time.
  readonly import attribute int Second;
  /// Gets the raw time value, useful for calculating elapsed time periods.
  readonly import attribute int RawTime;
};

builtin managed struct DialogOptionsRenderingInfo {
  /// The option that the mouse is currently positioned over
  import attribute int ActiveOptionID;
  /// The dialog that is to have its options rendered
  readonly import attribute Dialog* DialogToRender;
  /// The height of the dialog options
  import attribute int Height;
  /// The width of the text box for typing parser input, if enabled
  import attribute int ParserTextBoxWidth;
  /// The X co-ordinate of the top-left corner of the text box for typing input
  import attribute int ParserTextBoxX;
  /// The Y co-ordinate of the top-left corner of the text box for typing input
  import attribute int ParserTextBoxY;
  /// The surface that the dialog options need to be rendered to
  readonly import attribute DrawingSurface* Surface;
  /// The width of the dialog options
  import attribute int Width;
  /// The X co-ordinate of the top-left corner of the dialog options
  import attribute int X;
  /// The Y co-ordinate of the top-left corner of the dialog options
  import attribute int Y;
  /// Should the drawing surface have alpha channel
  import attribute bool HasAlphaChannel;
  /// Runs the active dialog option
  import bool RunActiveOption();
  /// Forces dialog options to redraw itself
  import void Update();
};

builtin managed struct AudioChannel {
  /// Changes playback to continue from the specified position into the sound.
  import void Seek(int position);
  /// Sets the audio to have its location at (x,y); it will get quieter the further away the player is.
  import void SetRoomLocation(int x, int y);
  /// Stops the sound currently playing on this channel.
  import void Stop();
  /// The channel ID of this channel (for use with legacy script).
  readonly import attribute int ID;
  /// Whether this channel is currently playing something.
  readonly import attribute bool IsPlaying;
  /// The length of the currently playing audio clip, in milliseconds.
  readonly import attribute int LengthMs;
  /// The stereo panning of the channel, from -100 to 100.
  import attribute int Panning;
  /// The audio clip that is currently being played on this channel, or null if none.
  readonly import attribute AudioClip* PlayingClip;
  /// The current offset into the sound. What this represents depends on the audio type.
  readonly import attribute int Position;
  /// The current offset into the sound, in milliseconds.
  readonly import attribute int PositionMs;
  /// The volume of this sound channel, from 0 to 100.
  import attribute int Volume;
  /// The speed of playing, in clip milliseconds per second (1000 is default).
  import attribute int Speed;
};

builtin managed struct AudioClip {
  /// Plays this audio clip.
  import AudioChannel* Play(AudioPriority=SCR_NO_VALUE, RepeatStyle=SCR_NO_VALUE);
  /// Plays this audio clip, starting from the specified offset.
  import AudioChannel* PlayFrom(int position, AudioPriority=SCR_NO_VALUE, RepeatStyle=SCR_NO_VALUE);
  /// Plays this audio clip, or queues it if all channels are busy.
  import AudioChannel* PlayQueued(AudioPriority=SCR_NO_VALUE, RepeatStyle=SCR_NO_VALUE);
  /// Stops all currently playing instances of this audio clip.
  import void Stop();
  /// Gets the file type of the sound.
  readonly import attribute AudioFileType FileType;
  /// Checks whether this audio file is available on the player's system.
  readonly import attribute bool IsAvailable;
  /// Gets the type of audio that this clip contains.
  readonly import attribute AudioType Type;
#ifdef SCRIPT_API_v350
  /// Gets the clip's ID number.
  readonly import attribute int ID;
#endif
};

builtin struct System {
#ifdef SCRIPT_COMPAT_v350
  readonly int  screen_width,screen_height;
  readonly int  color_depth;
  readonly int  os;
  readonly int  windowed;
  int  vsync;
  readonly int  viewport_width, viewport_height;
#endif
  /// Gets whether Caps Lock is currently on.
  readonly import static attribute bool CapsLock;
  /// Gets a specific audio channel instance.
  readonly import static attribute AudioChannel *AudioChannels[];   // $AUTOCOMPLETESTATICONLY$
  /// Gets the number of audio channels supported by AGS.
  readonly import static attribute int  AudioChannelCount;   // $AUTOCOMPLETESTATICONLY$
  /// Gets the colour depth that the game is running at.
  readonly import static attribute int  ColorDepth;
  /// Gets/sets the gamma correction level.
  import static attribute int  Gamma;
  /// Gets whether the game is running with 3D Hardware Acceleration.
  readonly import static attribute bool HardwareAcceleration;
  /// Gets whether Num Lock is currently on.
  readonly import static attribute bool NumLock;
  /// Gets which operating system the game is running on.
  readonly import static attribute eOperatingSystem OperatingSystem;
#ifdef SCRIPT_COMPAT_v350
  /// Gets the screen height of the current resolution.
  readonly import static attribute int  ScreenHeight;
  /// Gets the screen width of the current resolution.
  readonly import static attribute int  ScreenWidth;
#endif
  /// Gets whether Scroll Lock is currently on.
  readonly import static attribute bool ScrollLock;
  /// Gets whether the player's system supports gamma adjustment.
  readonly import static attribute bool SupportsGammaControl;
  /// Gets the AGS engine version number.
  readonly import static attribute String Version;
#ifdef SCRIPT_COMPAT_v350
  /// Gets the height of the visible area in which the game is displayed.
  readonly import static attribute int  ViewportHeight;
  /// Gets the width of the visible area in which the game is displayed.
  readonly import static attribute int  ViewportWidth;
#endif
  /// Gets/sets the audio output volume, from 0-100.
  import static attribute int  Volume;
  /// Gets/sets whether waiting for the vertical sync is enabled.
  import static attribute bool VSync;
  /// Gets/sets whether the game runs in a window or fullscreen.
  import static attribute bool Windowed;
  /// Gets whether the game window has input focus
  readonly import static attribute bool HasInputFocus;
  /// Gets a report about the runtime engine the game is running under.
  readonly import static attribute String RuntimeInfo;
#ifdef SCRIPT_API_v341
  /// Gets/sets whether sprites are rendered at screen resolution or native game resolution.
  import static attribute bool RenderAtScreenResolution;
#endif
#ifdef SCRIPT_API_v351
  /// Saves current runtime settings into configuration file
  import static void SaveConfigToFile();
#endif
#ifdef SCRIPT_API_v399
  /// Prints message
  import static void Log(LogLevel level, const string format, ...);    // $AUTOCOMPLETESTATICONLY$
#endif
};

enum BlockingStyle {
  eBlock = 919,
  eNoBlock = 920
};
enum Direction {
  eForwards = 1062,
  eBackwards = 1063
};
enum WalkWhere {
  eAnywhere = 304,
  eWalkableAreas = 305
};

builtin managed struct Object {
#ifdef SCRIPT_API_v3507
  /// Animates the object using its current view.
  import function Animate(int loop, int delay, RepeatStyle=eOnce, BlockingStyle=eBlock, Direction=eForwards, int frame=0);
#endif
#ifndef SCRIPT_API_v3507
  /// Animates the object using its current view.
  import function Animate(int loop, int delay, RepeatStyle=eOnce, BlockingStyle=eBlock, Direction=eForwards);
#endif
  /// Gets the object that is on the screen at the specified co-ordinates.
  import static Object* GetAtScreenXY(int x, int y);    // $AUTOCOMPLETESTATICONLY$
  /// Gets an integer Custom Property for this object.
  import function GetProperty(const string property);
  /// Gets a text Custom Property for this object.
  import String   GetTextProperty(const string property);
  /// Checks whether this object is colliding with another.
  import bool IsCollidingWithObject(Object*);
  /// Merges the object's image into the room background, and disables the object.
  import function MergeIntoBackground();
  /// Starts the object moving towards the specified co-ordinates.
  import function Move(int x, int y, int speed, BlockingStyle=eNoBlock, WalkWhere=eWalkableAreas);
  /// Removes a specific object tint, and returns the object to using the ambient room tint.
  import function RemoveTint();
  /// Runs the event handler for the specified event.
  import function RunInteraction(CursorMode);
  /// Instantly moves the object to have its bottom-left at the new co-ordinates.
  import function SetPosition(int x, int y);
#ifdef SCRIPT_API_v399
  /// Sets the object to use the specified view, ahead of doing an animation.
  import function SetView(int view, int loop=0, int frame=0);
#endif
#ifndef SCRIPT_API_v399
  /// Sets the object to use the specified view, ahead of doing an animation.
  import function SetView(int view, int loop=-1, int frame=-1);
#endif
  /// Stops any currently running animation on the object.
  import function StopAnimating();
  /// Stops any currently running move on the object.
  import function StopMoving();
  /// Tints the object to the specified colour.
  import function Tint(int red, int green, int blue, int saturation, int luminance);
  /// Gets whether the object is currently animating.
  readonly import attribute bool Animating;
  /// Gets/sets the object's baseline. This can be 0 to use the object's Y position as its baseline.
  import attribute int  Baseline;
  /// Allows you to manually specify the blocking height of the base of the object.
  import attribute int  BlockingHeight;
  /// Allows you to manually specify the blocking width of the base of the object.
  import attribute int  BlockingWidth;
  /// Gets/sets whether the mouse can be clicked on this object or whether it passes straight through.
  import attribute bool Clickable;
  /// Gets the current frame number during an animation.
  readonly import attribute int  Frame;
  /// Gets/sets the sprite number that is currently displayed on the object.
  import attribute int  Graphic;
  /// Gets the object's ID number.
  readonly import attribute int ID;
#ifdef SCRIPT_COMPAT_v3507
  /// Gets/sets whether the object ignores walkable area scaling.
  import attribute bool IgnoreScaling;
#endif
#ifdef SCRIPT_COMPAT_v340
  /// Gets/sets whether the object ignores walk-behind areas.
  import attribute bool IgnoreWalkbehinds;
#endif
  /// Gets the current loop number during an animation.
  readonly import attribute int  Loop;
  /// Gets whether the object is currently moving.
  readonly import attribute bool Moving;
  /// Gets the object's description.
  readonly import attribute String Name;
  /// Gets/sets whether other objects and characters can move through this object.
  import attribute bool Solid;
  /// Gets/sets the object's transparency.
  import attribute int  Transparency;
  /// Gets the current view number during an animation.
  readonly import attribute int View;
  /// Gets/sets whether the object is currently visible.
  import attribute bool Visible;
  /// Gets/sets the X co-ordinate of the object's bottom-left hand corner.
  import attribute int  X;
  /// Gets/sets the Y co-ordinate of the object's bottom-left hand corner.
  import attribute int  Y;
  /// Checks whether an event handler has been registered for clicking on this object in the specified cursor mode.
  import bool     IsInteractionAvailable(CursorMode);
  /// Sets the individual light level for this object.
  import function SetLightLevel(int light_level);
  /// Sets an integer custom property for this object.
  import bool SetProperty(const string property, int value);
  /// Sets a text custom property for this object.
  import bool SetTextProperty(const string property, const string value);
  /// Gets whether the object has an explicit light level set.
  readonly import attribute bool HasExplicitLight;
  /// Gets whether the object has an explicit tint set.
  readonly import attribute bool HasExplicitTint;
  /// Gets the individual light level for this object.
  readonly import attribute int  LightLevel;
  /// Gets the Blue component of this object's colour tint.
  readonly import attribute int  TintBlue;
  /// Gets the Green component of this object's colour tint.
  readonly import attribute int  TintGreen;
  /// Gets the Red component of this object's colour tint.
  readonly import attribute int  TintRed;
  /// Gets the Saturation of this object's colour tint.
  readonly import attribute int  TintSaturation;
  /// Gets the Luminance of this object's colour tint.
  readonly import attribute int  TintLuminance;
#ifdef SCRIPT_API_v3507
  /// Returns the object at the specified position within this room.
  import static Object* GetAtRoomXY(int x, int y);      // $AUTOCOMPLETESTATICONLY$
#endif
#ifdef SCRIPT_API_v399
  /// Gets/sets whether the object uses manually specified scaling instead of using walkable area scaling.
  import attribute bool ManualScaling;
  /// Gets/sets the object's current scaling level.
  import attribute int  Scaling;
#endif
#ifdef SCRIPT_API_v399
  /// Gets/sets the blending mode for this object.
  import attribute BlendMode BlendMode;
  /// Gets/sets the object's sprite rotation in degrees.
  import attribute float GraphicRotation;
#endif
  readonly int reserved[2];  // $AUTOCOMPLETEIGNORE$
};

enum StopMovementStyle
{
  eKeepMoving = 0,
  eStopMoving = 1
};

builtin managed struct Character {
  /// Adds the specified item to the character's inventory.
  import function AddInventory(InventoryItem *item, int addAtIndex=SCR_NO_VALUE);
  /// Manually adds a waypoint to the character's movement path.
  import function AddWaypoint(int x, int y);
#ifdef SCRIPT_API_v3507
  /// Animates the character using its current locked view.
  import function Animate(int loop, int delay, RepeatStyle=eOnce, BlockingStyle=eBlock, Direction=eForwards, int frame=0);
#endif
#ifndef SCRIPT_API_v3507
  /// Animates the character using its current locked view.
  import function Animate(int loop, int delay, RepeatStyle=eOnce, BlockingStyle=eBlock, Direction=eForwards);
#endif
  /// Moves the character to another room. If this is the player character, the game will also switch to that room.
  import function ChangeRoom(int room, int x=SCR_NO_VALUE, int y=SCR_NO_VALUE, CharacterDirection direction=eDirectionNone);
  /// Moves the character to another room, using the old-style position variable
  import function ChangeRoomAutoPosition(int room, int position=0);
  /// Changes the character's normal walking view.
  import function ChangeView(int view);
  /// Turns this character to face the other character.
  import function FaceCharacter(Character* , BlockingStyle=eBlock);
  /// Turns this character to face the specified location in the room.
  import function FaceLocation(int x, int y, BlockingStyle=eBlock);
  /// Turns this character to face the specified object.
  import function FaceObject(Object* , BlockingStyle=eBlock);
  /// Starts this character following the other character.
  import function FollowCharacter(Character*, int dist=10, int eagerness=97);
  /// Returns the character that is at the specified position on the screen.
  import static Character* GetAtScreenXY(int x, int y);    // $AUTOCOMPLETESTATICONLY$
  /// Gets a numeric custom property for this character.
  import function GetProperty(const string property);
  /// Gets a text custom property for this character.
  import String   GetTextProperty(const string property);
  /// Checks whether the character currently has the specified inventory item.
  import bool     HasInventory(InventoryItem *item);
  /// Checks whether this character is in collision with the other character.
  import function IsCollidingWithChar(Character*);
  /// Checks whether this character is in collision with the object.
  import function IsCollidingWithObject(Object* );
  /// Locks the character to this view, ready for doing animations.
  import function LockView(int view, StopMovementStyle=eStopMoving);
#ifdef SCRIPT_API_v350
  /// Locks the character to this view, and aligns it against one side of the existing sprite.
  import function LockViewAligned(int view, int loop, HorizontalAlignment, StopMovementStyle=eStopMoving);
#endif
#ifndef SCRIPT_API_v350
  /// Locks the character to this view, and aligns it against one side of the existing sprite.
  import function LockViewAligned(int view, int loop, Alignment, StopMovementStyle=eStopMoving);
#endif
  /// Locks the character to the specified view frame
  import function LockViewFrame(int view, int loop, int frame, StopMovementStyle=eStopMoving);
  /// Locks the character to is view, with high-resolution position adjustment.
  import function LockViewOffset(int view, int xOffset, int yOffset, StopMovementStyle=eStopMoving);
  /// Removes the item from this character's inventory.
  import function LoseInventory(InventoryItem *item);
  /// Moves the character to the destination, without playing his walking animation.
  import function Move(int x, int y, BlockingStyle=eNoBlock, WalkWhere=eWalkableAreas);
  /// Moves the character to the nearest walkable area.
  import function PlaceOnWalkableArea();
  /// Removes an existing colour tint from the character.
  import void     RemoveTint();
  /// Runs one of the character's interaction events.
  import function RunInteraction(CursorMode);
  /// Says the specified text using the character's speech settings.
  import function Say(const string message, ...);
  /// Says the specified text at the specified position on the screen using the character's speech settings.
  import function SayAt(int x, int y, int width, const string message);
  /// Displays the text as lucasarts-style speech but does not block the game.
  import Overlay* SayBackground(const string message);
  /// Makes this character the player character.
  import function SetAsPlayer();
  /// Changes the character's idle view.
  import function SetIdleView(int view, int delay);
  /// Changes the character's movement speed.
  import function SetWalkSpeed(int x, int y);
  /// Stops the character from moving.
  import function StopMoving();
  /// The specified text is displayed in a thought-bubble GUI.
  import function Think(const string message, ...);
  /// Tints the character to the specified colour.
  import void     Tint(int red, int green, int blue, int saturation, int luminance);
  /// Unlocks the view after an animation has finished.
  import function UnlockView(StopMovementStyle=eStopMoving);
  /// Moves the character to the destination, automatically playing his walking animation.
  import function Walk(int x, int y, BlockingStyle=eNoBlock, WalkWhere=eWalkableAreas);
  /// Moves the character in a straight line as far as possible towards the co-ordinates. Useful for keyboard movement.
  import function WalkStraight(int x, int y, BlockingStyle=eNoBlock);
  /// Gets/sets the character's current inventory item. null if no item selected.
  import attribute InventoryItem* ActiveInventory;
  /// Gets whether the character is currently animating.
  readonly import attribute bool Animating;
  /// Gets/sets the character's animation speed.
  import attribute int  AnimationSpeed;
  /// Gets/sets a specific baseline for the character. 0 means character's Y-pos will be used.
  import attribute int  Baseline;
  /// Gets/sets the interval at which the character will blink while talking, in game loops.
  import attribute int  BlinkInterval;
  /// Gets/sets the view used for the character's blinking animation. -1 to disable.
  import attribute int  BlinkView;
  /// Gets/sets whether the character will blink while thinking as well as talking.
  import attribute bool BlinkWhileThinking;
  /// Allows you to manually specify the height of the blocking area at the character's feet.
  import attribute int  BlockingHeight;
  /// Allows you to manually specify the width of the blocking area at the character's feet.
  import attribute int  BlockingWidth;
  /// Gets/sets whether the mouse can be clicked on the character, or whether it passes straight through.
  import attribute bool Clickable;
  /// Gets/sets whether this character has an 8-loop walking view with diagonal loops.
  import attribute bool DiagonalLoops;
  /// Gets/sets the character's current frame number within its current view.
  import attribute int  Frame;
  /// Gets whether the character has an explicit tint set.
  readonly import attribute bool HasExplicitTint;
  /// Gets the character's ID number.
  readonly import attribute int ID;
  /// Gets the character's idle view (-1 if none).
  readonly import attribute int IdleView;
  /// Gets/sets whether the character ignores region tints and lighting.
  import attribute bool IgnoreLighting;
  import attribute bool IgnoreScaling;       // obsolete. $AUTOCOMPLETEIGNORE$
#ifdef SCRIPT_COMPAT_v340
  /// Gets/sets whether the character ignores walk-behind areas and is always placed on top.
  import attribute bool IgnoreWalkbehinds; 
#endif
  /// Accesses the number of each inventory item that the character currently has.
  import attribute int  InventoryQuantity[];
  /// Gets/sets the character's current loop number within its current view.
  import attribute int  Loop;
  /// Gets/sets whether the character uses manually specified scaling instead of using walkable area scaling.
  import attribute bool ManualScaling;
  /// Gets/sets whether the character only moves when their animation frame changes.
  import attribute bool MovementLinkedToAnimation;
  /// Gets whether the character is currently moving.
  readonly import attribute bool Moving;
  /// Gets/sets the character's name.
  import attribute String Name;
  /// Gets the character's normal walking view.
  readonly import attribute int NormalView;
  /// Gets the room number that the character was in before this one.
  readonly import attribute int PreviousRoom;
  /// Gets the room number that the character is currently in.
  readonly import attribute int Room;
  /// Gets/sets whether the character's movement speed is adjusted in line with its scaling level.
  import attribute bool ScaleMoveSpeed;
  /// Gets/sets whether the volume of frame-linked sounds for the character are adjusted in line with its scaling level.
  import attribute bool ScaleVolume;
  /// Gets/sets the character's current scaling level.
  import attribute int  Scaling;
  /// Gets/sets whether this character blocks other objects and characters from moving through it.
  import attribute bool Solid;
  /// Gets whether the character is currently in the middle of a Say command.
  readonly import attribute bool Speaking;
  /// Gets the current frame of the character's speaking animation (only valid when Speaking is true)
  readonly import attribute int SpeakingFrame;
  /// Gets/sets the character's speech animation delay (only if not using global setting).
  import attribute int  SpeechAnimationDelay;
  /// Gets/sets the character's speech text colour.
  import attribute int  SpeechColor;
  /// Gets/sets the character's speech view.
  import attribute int  SpeechView;
  /// Gets/sets the character's thinking view.
  import attribute int  ThinkView;
  /// Gets/sets the character's current transparency level.
  import attribute int  Transparency;
  /// Gets/sets whether the character turns on the spot to face the correct direction before walking.
  import attribute bool TurnBeforeWalking;
  /// Gets the character's current view number.
  readonly import attribute int View;
  /// Gets the character's X movement speed.
  readonly import attribute int WalkSpeedX;
  /// Gets the character's Y movement speed.
  readonly import attribute int WalkSpeedY;
  /// Gets whether the character is currently in the middle of a Think command.
  readonly import attribute bool Thinking;
  /// Gets the current frame of the character's thinking animation (only valid when Thinking is true)
  readonly import attribute int ThinkingFrame;
  /// Turns this character to face the specified direction.
  import function FaceDirection(CharacterDirection direction, BlockingStyle=eBlock);
  /// Sets an integer custom property for this character.
  import bool SetProperty(const string property, int value);
  /// Sets a text custom property for this character.
  import bool SetTextProperty(const string property, const string value);
  /// Checks whether an event handler has been registered for clicking on this character in the specified cursor mode.
  import bool     IsInteractionAvailable(CursorMode);
  /// Sets the individual light level for this character.
  import function SetLightLevel(int light_level);
  /// Gets the X position this character is currently moving towards.
  readonly import attribute int DestinationX;
  /// Gets the Y position this character is currently moving towards.
  readonly import attribute int DestinationY;
  /// Gets whether the character has an explicit light level set.
  readonly import attribute bool HasExplicitLight;
  /// Gets the individual light level for this character.
  readonly import attribute int  LightLevel;
  /// Gets the Blue component of this character's colour tint.
  readonly import attribute int  TintBlue;
  /// Gets the Green component of this character's colour tint.
  readonly import attribute int  TintGreen;
  /// Gets the Red component of this character's colour tint.
  readonly import attribute int  TintRed;
  /// Gets the Saturation of this character's colour tint.
  readonly import attribute int  TintSaturation;
  /// Gets the Luminance of this character's colour tint.
  readonly import attribute int  TintLuminance;
#ifdef SCRIPT_API_v3507
  /// Returns the character at the specified position within this room.
  import static Character* GetAtRoomXY(int x, int y);      // $AUTOCOMPLETESTATICONLY$
#endif
  /// The character's current X-position.
  import attribute int  x;
  /// The character's current Y-position.
  import attribute int  y;
  /// The character's current Z-position.
  import attribute int  z;
#ifdef SCRIPT_API_v399
  /// Gets/sets the character's current blend mode.
  import attribute BlendMode BlendMode;
  /// Gets/sets the character's sprite rotation in degrees.
  import attribute float GraphicRotation;
#endif
  readonly int reserved_a[28];   // $AUTOCOMPLETEIGNORE$
  readonly short reserved_f[MAX_INV];  // $AUTOCOMPLETEIGNORE$
  readonly int   reserved_e;   // $AUTOCOMPLETEIGNORE$
  readonly char  reserved_g[40];   // $AUTOCOMPLETEIGNORE$
  readonly char  reserved_h[20];   // $AUTOCOMPLETEIGNORE$
  // TODO: find out if Visible property may work as a proper analogue, otherwise add new one
  char  on;  // $AUTOCOMPLETEIGNORE$
  };

builtin struct Game {
  /// Changes the active translation.
  import static bool   ChangeTranslation(const string newTranslationFileName);
  /// Returns true the first time this command is called with this token.
  import static bool   DoOnceOnly(const string token);
  /// Gets the AGS Colour Number for the specified RGB colour.
  import static int    GetColorFromRGB(int red, int green, int blue);
  /// Gets the number of frames in the specified view loop.
  import static int    GetFrameCountForLoop(int view, int loop);
  /// Gets the name of whatever is on the screen at (x,y)
  import static String GetLocationName(int x, int y);
  /// Gets the number of loops in the specified view.
  import static int    GetLoopCountForView(int view);
  /// Returns the current pattern/track number if the current music is MOD or XM.
  import static int    GetMODPattern();
  /// Gets whether the "Run next loop after this" setting is checked for the specified loop.
  import static bool   GetRunNextSettingForLoop(int view, int loop);
  /// Gets the description of the specified save game slot.
  import static String GetSaveSlotDescription(int saveSlot);
  /// Gets the ViewFrame instance for the specified view frame.
  import static ViewFrame* GetViewFrame(int view, int loop, int frame);
  /// Prompts the user to type in a string, and returns the text that they type in.
  import static String InputBox(const string prompt);
  /// Gets whether any audio (of this type) is currently playing.
  import static bool   IsAudioPlaying(AudioType audioType=SCR_NO_VALUE);
  /// Changes the volume drop applied to this audio type when speech is played
  import static void   SetAudioTypeSpeechVolumeDrop(AudioType, int volumeDrop);
  /// Changes the default volume of audio clips of the specified type.
  import static void   SetAudioTypeVolume(AudioType, int volume, ChangeVolumeType);
  /// Sets the directory where AGS will save and load saved games.
  import static bool   SetSaveGameDirectory(const string directory);
  /// Stops all currently playing audio (optionally of the specified type).
  import static void   StopAudio(AudioType audioType=SCR_NO_VALUE);
#ifndef STRICT_AUDIO
  /// Stops all currently playing sound effects.
  import static void   StopSound(bool includeAmbientSounds=false);   // $AUTOCOMPLETEIGNORE$
#endif
  /// Gets the number of characters in the game.
  readonly import static attribute int CharacterCount;
  /// Gets the number of dialogs in the game.
  readonly import static attribute int DialogCount;
  /// Gets the name of the game EXE file.
  readonly import static attribute String FileName;
  /// Gets the number of fonts in the game.
  readonly import static attribute int FontCount;
  /// Accesses the legacy Global Messages, from AGS 2.x
  readonly import static attribute String GlobalMessages[];
  /// Accesses the global strings collection. This is obsolete.
  import static attribute String GlobalStrings[];
  /// Gets the number of GUIs in the game.
  readonly import static attribute int GUICount;
  /// Gets/sets the time for which user input is ignored after some text is automatically removed
  import static attribute int IgnoreUserInputAfterTextTimeoutMs;
  /// Checks whether the game is currently in the middle of a skippable cutscene.
  readonly import static attribute bool InSkippableCutscene;
  /// Gets the number of inventory items in the game.
  readonly import static attribute int InventoryItemCount;
  /// Gets/sets the minimum time that a piece of speech text stays on screen (in milliseconds)
  import static attribute int MinimumTextDisplayTimeMs;
  /// Gets the number of mouse cursors in the game.
  readonly import static attribute int MouseCursorCount;
  /// Gets/sets the game name.
  import static attribute String Name;
  /// Gets/sets the normal font used for displaying text.
  import static attribute FontType NormalFont;
  /// Checks whether the game is currently skipping over a cutscene.
  readonly import static attribute bool SkippingCutscene;
  /// Gets/sets the font used for displaying speech text.
  import static attribute FontType SpeechFont;
  /// Gets the height of the specified sprite.
  readonly import static attribute int SpriteHeight[];
  /// Gets the width of the specified sprite.
  readonly import static attribute int SpriteWidth[];
  /// Gets/sets how fast speech text is removed from the screen.
  import static attribute int TextReadingSpeed;
  /// Gets name of the currently active translation.
  readonly import static attribute String TranslationFilename;
  /// Gets whether the game is using native co-ordinates.
  readonly import static attribute bool UseNativeCoordinates;
  /// Gets the number of views in the game.
  readonly import static attribute int ViewCount;
#ifdef SCRIPT_API_v340
  /// Returns true if the given plugin is currently loaded.
  import static bool   IsPluginLoaded(const string name);
  /// Gets the number of audio clips in the game.
  readonly import static attribute int AudioClipCount;
  /// Accesses the audio clips collection.
  readonly import static attribute AudioClip *AudioClips[];
#endif
#ifdef SCRIPT_API_v350
  /// Play speech voice-over in non-blocking mode, optionally apply music and sound volume reduction
  import static AudioChannel* PlayVoiceClip(Character*, int cue, bool as_speech = true);
  /// Simulate a keypress on the keyboard.
  import static void   SimulateKeyPress(eKeyCode key);
#endif
#ifdef SCRIPT_API_v3507
  /// Gets the primary camera
  import static readonly attribute Camera *Camera;
  /// Gets the Camera by index.
  import static readonly attribute Camera *Cameras[];
  /// Gets the number of cameras.
  import static readonly attribute int CameraCount;
#endif
#ifdef SCRIPT_API_v399
  /// Gets the code which describes how was the last blocking state skipped by a user (or autotimer).
  import static readonly attribute int BlockingWaitSkipped;
#endif
};

builtin struct GameState {
  int  score;
  int  used_mode;
  int  disabled_user_interface;
  int  gscript_timer;
  int  debug_mode;
  int  globalvars[MAX_LEGACY_GLOBAL_VARS];
  int  messagetime;   // for auto-remove messages
  int  usedinv;
  int  reserved__[4];   // $AUTOCOMPLETEIGNORE$
  int  text_speed;
  int  sierra_inv_color;
  int  talkanim_speed;  // $AUTOCOMPLETEIGNORE$
  int  inv_item_wid;
  int  inv_item_hit;
  int  text_shadow_color;
  int  swap_portrait;
  int  speech_text_gui;
  int  following_room_timer;
  int  total_score;
  int  skip_display;
  int  no_multiloop_repeat;
  int  roomscript_finished;
  int  inv_activated;
  int  no_textbg_when_voice;
  int  max_dialogoption_width;
  int  no_hicolor_fadein;
  int  bgspeech_game_speed;
  int  bgspeech_stay_on_display;
  int  unfactor_speech_from_textlength;
  int  mp3_loop_before_end;
  int  speech_music_drop;
  readonly int  in_cutscene;
  readonly int  skipping_cutscene;
  readonly int  room_width;
  readonly int  room_height;
  int  game_speed_modifier;  // $AUTOCOMPLETEIGNORE$
  int  score_sound;
  int  previous_game_data;
#ifndef SCRIPT_COMPAT_v341
  readonly int unused__041; // $AUTOCOMPLETEIGNORE$
#endif
#ifdef SCRIPT_COMPAT_v341
  int  replay_hotkey;
#endif
  int  dialog_options_x;
  int  dialog_options_y;
  int  narrator_speech;
  int  ambient_sounds_persist;
  int  lipsync_speed;
  int  reserved__4;   // $AUTOCOMPLETEIGNORE$
  int  disable_antialiasing;
  int  text_speed_modifier;
  int  text_align;
  int  speech_bubble_width;
  int  min_dialogoption_width;
  int  disable_dialog_parser;
  int  anim_background_speed;
  int  top_bar_backcolor;
  int  top_bar_textcolor;
  int  top_bar_bordercolor;
  int  top_bar_borderwidth;
  int  top_bar_ypos;
  int  screenshot_width;
  int  screenshot_height;
  int  top_bar_font;
#ifndef SCRIPT_COMPAT_v321
  int  reserved__2;   // $AUTOCOMPLETEIGNORE$
#endif
  int  auto_use_walkto_points;
  int  inventory_greys_out;
#ifndef SCRIPT_COMPAT_v321
  int  reserved__3;   // $AUTOCOMPLETEIGNORE$
#endif
  int  abort_key;
  readonly int fade_color_red;
  readonly int fade_color_green;
  readonly int fade_color_blue;
  int  show_single_dialog_option;
  int  keep_screen_during_instant_transition;
  int  read_dialog_option_color;
  int  stop_dialog_at_end;   // $AUTOCOMPLETEIGNORE$
  int  reserved__5;   // $AUTOCOMPLETEIGNORE$
  int  reserved__6;   // $AUTOCOMPLETEIGNORE$
  int  reserved__7;   // $AUTOCOMPLETEIGNORE$
  int  reserved__8;   // $AUTOCOMPLETEIGNORE$
  int  dialog_options_highlight_color;
  };
  
enum SkipSpeechStyle {
  eSkipNone         = -1,
  eSkipKeyMouseTime = 0,
  eSkipKeyTime      = 1,
  eSkipTime         = 2,
  eSkipKeyMouse     = 3,
  eSkipMouseTime    = 4,
  eSkipKey          = 5,
  eSkipMouse        = 6
};
  
builtin struct Speech {
  /// Stop speech animation this number of game loops before speech ends (text mode only).
  import static attribute int             AnimationStopTimeMargin;
  /// Enables/disables the custom speech portrait placement.
  import static attribute bool            CustomPortraitPlacement;
  /// Gets/sets extra time the speech will always stay on screen after its common time runs out.
  import static attribute int             DisplayPostTimeMs;
  /// Gets/sets global speech animation delay (if using global setting).
  import static attribute int             GlobalSpeechAnimationDelay;
  /// Gets/sets speech portrait x offset relative to screen side.
  import static attribute int             PortraitXOffset;
  /// Gets/sets speech portrait y position.
  import static attribute int             PortraitY;
  /// Gets/sets specific key which can skip speech text.
  import static attribute eKeyCode        SkipKey;
  /// Gets/sets how the player can skip speech lines.
  import static attribute SkipSpeechStyle SkipStyle;
  /// Gets/sets the style in which speech is displayed.
  import static attribute eSpeechStyle    Style;
#ifdef SCRIPT_API_v350
  /// Gets/sets how text in message boxes and Sierra-style speech is aligned.
  import static attribute HorizontalAlignment TextAlignment;
#endif
#ifndef SCRIPT_API_v350
  /// Gets/sets how text in message boxes and Sierra-style speech is aligned.
  import static attribute Alignment       TextAlignment;
#endif
  /// Gets/sets whether speech animation delay should use global setting (or Character setting).
  import static attribute bool            UseGlobalSpeechAnimationDelay;
  /// Gets/sets whether voice and/or text are used in the game.
  import static attribute eVoiceMode      VoiceMode;
#ifdef SCRIPT_API_v399
  /// Gets the overlay representing displayed blocking text, or null if no such text none is displayed at the moment.
  import static readonly attribute Overlay* TextOverlay;
  /// Gets the overlay representing displayed portrait, or null if it is not displayed at the moment.
  import static readonly attribute Overlay* PortraitOverlay;
#endif
};

#ifdef SCRIPT_API_v3507
builtin managed struct Camera {
  /// Gets/sets the X position of this camera in the room.
  import attribute int X;
  /// Gets/sets the Y position of this camera in the room.
  import attribute int Y;
  /// Gets/sets the camera's capture width in room coordinates.
  import attribute int Width;
  /// Gets/sets the camera's capture height in room coordinates.
  import attribute int Height;
#ifdef SCRIPT_API_v399
  /// Gets/sets the camera rotation in degrees.
  import attribute float Rotation;
#endif

  /// Gets/sets whether this camera will follow the player character automatically.
  import attribute bool AutoTracking;

  /// Creates a new Camera.
  import static Camera *Create();
  /// Removes an existing camera; note that primary camera will never be removed
  import void Delete();
  /// Changes camera position in the room and disables automatic tracking of the player character.
  import void SetAt(int x, int y);
  /// Changes camera's capture dimensions in room coordinates.
  import void SetSize(int width, int height);
};

builtin managed struct Viewport {
  /// Gets/sets the X position on the screen where this viewport is located.
  import attribute int X;
  /// Gets/sets the Y position on the screen where this viewport is located.
  import attribute int Y;
  /// Gets/sets the viewport's width in screen coordinates.
  import attribute int Width;
  /// Gets/sets the viewport's height in screen coordinates.
  import attribute int Height;
  /// Gets/sets the room camera displayed in this viewport.
  import attribute Camera *Camera;
  /// Gets/sets whether the viewport is drawn on screen.
  import attribute bool Visible;
  /// Gets/sets the Viewport's z-order relative to other viewports.
  import attribute int ZOrder;

  /// Creates a new Viewport.
  import static Viewport *Create();
  /// Returns the viewport at the specified screen location.
  import static Viewport *GetAtScreenXY(int x, int y);
  /// Removes an existing viewport; note that primary viewport will never be removed
  import void Delete();
  /// Changes viewport's position on the screen
  import void SetPosition(int x, int y, int width, int height);
  /// Returns the point in room corresponding to the given screen coordinates if seen through this viewport.
  import Point *ScreenToRoomPoint(int scrx, int scry, bool clipViewport = true);
  /// Returns the point on screen corresponding to the given room coordinates if seen through this viewport.
  import Point *RoomToScreenPoint(int roomx, int roomy, bool clipViewport = true);
};

builtin struct Screen {
  /// Gets the width of the game resolution.
  import static readonly attribute int Width;
  /// Gets the height of the game resolution.
  import static readonly attribute int Height;
  /// Gets/sets whether the viewport should automatically adjust itself and camera to the new room's background size.
  import static attribute bool AutoSizeViewportOnRoomLoad;
  /// Gets the primary room viewport.
  import static readonly attribute Viewport *Viewport;
  /// Gets a Viewport by index.
  import static readonly attribute Viewport *Viewports[];
  /// Gets the number of viewports.
  import static readonly attribute int ViewportCount;

  /// Returns the point in room which is displayed at the given screen coordinates.
  import static Point *ScreenToRoomPoint(int sx, int sy);
  /// Returns the point on screen corresponding to the given room coordinates relative to the main viewport.
  import static Point *RoomToScreenPoint(int rx, int ry);
};
#endif



import readonly Character *player;
import Mouse mouse;
#ifdef SCRIPT_COMPAT_v350
import System system;
#endif
import GameState  game;
import Object object[MAX_ROOM_OBJECTS];
import ColorType palette[PALETTE_SIZE];
// [OBSOLETE]
import int   gs_globals[MAX_LEGACY_GLOBAL_VARS];
import short savegameindex[MAX_LISTBOX_SAVED_GAMES];

#undef CursorMode
#undef FontType
#undef AudioType
