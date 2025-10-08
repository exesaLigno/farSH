#pragma once

#include <cstdint>
#include <cstdio>
#include <functional>
#include <initializer_list>

#include "types.hpp"

class UnicodeSymbol
{
public:
    enum class Type : uint8_t
    {
        None,
        ASCIISymbol, ControlCode,
        EscapedSequence,
        UnicodeSymbol,
        Invalid
    };

    enum class EscapeSequenceType : uint8_t
    {
        None,
        Fe, ControlSequenceIntroducer, OperatingSystemCommand, Fs, Fp, nF
    };

    enum class Command
    {
        None,

        // ASCII Controle Codes
        Null, NUL = Null,
        StartOfHeading, SOH = StartOfHeading,
        StartOfText, STX = StartOfText,
        EndOfText, ETX = EndOfText,
        EndOfTransmission, EOT = EndOfTransmission,
        Enquiry, ENQ = Enquiry,
        Acknowledge, ACK = Acknowledge,
        Bell, BEL = Bell,
        Backspace, BS = Backspace,
        HorizontalTab, HT = HorizontalTab,
        LineFeed, LF = LineFeed,
        VerticalTabulation, VT = VerticalTabulation,
        FormFeed, FF = FormFeed,
        CarriageReturn, CR = CarriageReturn,
        ShiftOut, SO = ShiftOut,
        ShiftIn, SI = ShiftIn,
        DataLinkEscape, DLE = DataLinkEscape,
        DeviceControlOne, DC1 = DeviceControlOne,
        DeviceControlTwo, DC2 = DeviceControlTwo,
        DeviceControlThree, DC3 = DeviceControlThree,
        DeviceControlFour, DC4 = DeviceControlFour,
        NegativeAcknowledge, NAK = NegativeAcknowledge,
        SynchronousIdle, SYN = SynchronousIdle,
        EndOfTransmissionBlock, ETB = EndOfTransmissionBlock,
        Cancel, CAN = Cancel,
        EndOfMedium, EM = EndOfMedium,
        Substitute, SUB = Substitute,
        Escape, ESC = Escape,
        FileSeparator, FS = FileSeparator,
        GroupSeparator, GS = GroupSeparator,
        RecordSeparator, RS = RecordSeparator,
        UnitSeparator, US = UnitSeparator,
        Delete, DEL = Delete,

        // Control Sequence Introducer
        CursorUp, CUU = CursorUp,
        CursorDown, CUD = CursorDown,
        CursorForward, CUF = CursorForward,
        CursorBack, CUB = CursorBack,
        CursorNextLine, CNL = CursorNextLine,
        CursorPreviousLine, CPL = CursorPreviousLine,
        CursorHorizontalAbsolute, CHA = CursorHorizontalAbsolute,
        CursorPosition, CUP = CursorPosition,
        EraseInDisplay, ED = EraseInDisplay,
        EraseInLine, EL = EraseInLine,
        ScrollUp, SU = ScrollUp,
        ScrollDown, SD = ScrollDown,
        HorizontalVerticalPosition, HVP = HorizontalVerticalPosition,
        SelectGraphicRendition, SGR = SelectGraphicRendition,
        DeleteAfter,

        // Fe Escape Sequence
        SingleShiftTwo, SS2 = SingleShiftTwo,
        SingleShiftThree, SS3 = SingleShiftThree,
        DeviceControlString, DCS = DeviceControlString,
        StringTerminator, ST = StringTerminator,
        StartOfString, SOS = StartOfString,
        PrivacyMessage, PM = PrivacyMessage,
        ApplicationProgramCommand, APC = ApplicationProgramCommand,
    };

private:
    constexpr static size_t baseSize = sizeof(Byte*) / sizeof(Byte);
    union
    {
        Byte bytes[baseSize] = { 0 };
        Byte* extendedBytes;
    };
    
    size_t bytes_length = 0;
    Type type = Type::None;
    uint8_t display_width = 0;

    Command GetControlCodeCommand() const;
    Command GetControlSequenceIntroducerCommand() const;
    Command GetOperatingSystemCommand() const;
    Command GetFeSequenceCommand() const;
    Command GetFsSequenceCommand() const;
    Command GetFpSequenceCommand() const;
    Command GetnFSequenceCommand() const;
    bool IsExtended() const;

    enum class State;

    static void ReaderFSMStep(State& current_state, const Byte& byte);

    void Initialize(const Byte* _bytes, const size_t _bytes_length);

public:
    UnicodeSymbol();
    explicit UnicodeSymbol(Type _type);
    explicit UnicodeSymbol(const Byte* _bytes, const size_t _bytes_length);
    explicit UnicodeSymbol(const Byte _byte);

    ~UnicodeSymbol();

    UnicodeSymbol(const UnicodeSymbol& other) noexcept;
    UnicodeSymbol(UnicodeSymbol&& other) noexcept;
    
    UnicodeSymbol& operator=(const UnicodeSymbol& other) noexcept;
    UnicodeSymbol& operator=(UnicodeSymbol&& other) noexcept;
        
    Type GetType() const;

    bool OfType(std::initializer_list<Type> types) const;

    EscapeSequenceType GetSequenceType() const;

    void DebugWriteTo(FILE* fileno = stdout) const;

    void WriteTo(FILE* fileno = stdout) const;

    bool IsCorrect() const;

    bool IsCommand() const;

    bool IsSymbol() const;

    Command GetCommand() const;

    Byte GetByte(int offset = 0) const;

    int GetCSIParameter(size_t position) const;

    uint8_t DisplayWidth() const;

    static size_t Create(UnicodeSymbol& destination, std::function<Byte()> get_next_byte);

    static size_t Create(UnicodeSymbol& destination, const Byte* string_ptr);

    const size_t BytesLength() const;

    const Byte* AsBytes() const;

    friend bool operator==(const UnicodeSymbol& first, const UnicodeSymbol& second);
    friend bool operator==(const UnicodeSymbol& first, const char second);
    friend bool operator==(const UnicodeSymbol& first, const char* second);
};
