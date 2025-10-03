#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <functional>
#include <initializer_list>

#include "wchar_utils.hpp"
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

    // Byte GetIdentityByteByCommand(Command cmd)
    // {
    //     switch (cmd)
    //     {
    //         case Command::Null: return 0x00;
    //         case Command::StartOfHeading: return 0x01;
    //         case Command::StartOfText: return 0x02;
    //         case Command::EndOfText: return 0x03;
    //         case Command::EndOfTransmission: return 0x04;
    //         case Command::Enquiry: return 0x05;
    //         case Command::Acknowledge: return 0x06;
    //         case Command::Bell: return 0x07;
    //         case Command::Backspace: return 0x08;
    //         case Command::HorizontalTab: return 0x09;
    //         case Command::LineFeed: return 0x0A;
    //         case Command::VerticalTabulation: return 0x0B;
    //         case Command::FormFeed: return 0x0C;
    //         case Command::CarriageReturn: return 0x0D;
    //         case Command::ShiftOut: return 0x0E;
    //         case Command::ShiftIn: return 0x0F;
    //         case Command::DataLinkEscape: return 0x10;
    //         case Command::DeviceControlOne: return 0x11;
    //         case Command::DeviceControlTwo: return 0x12;
    //         case Command::DeviceControlThree: return 0x13;
    //         case Command::DeviceControlFour: return 0x14;
    //         case Command::NegativeAcknowledge: return 0x15;
    //         case Command::SynchronousIdle: return 0x16;
    //         case Command::EndOfTransmissionBlock: return 0x17;
    //         case Command::Cancel: return 0x18;
    //         case Command::EndOfMedium: return 0x19;
    //         case Command::Substitute: return 0x1A;
    //         case Command::Escape: return 0x1B;
    //         case Command::FileSeparator: return 0x1C;
    //         case Command::GroupSeparator: return 0x1D;
    //         case Command::RecordSeparator: return 0x1E;
    //         case Command::UnitSeparator: return 0x1F;
    //         case Command::Delete: return 0x7F;
    //         case Command::CursorUp: return 0x41;
    //         case Command::CursorDown: return 0x42;
    //         case Command::CursorForward: return 0x43;
    //         case Command::CursorBack: return 0x44;
    //         case Command::CursorNextLine: return 0x45;
    //         case Command::CursorPreviousLine: return 0x46;
    //         case Command::CursorHorizontalAbsolute: return 0x47;
    //         case Command::CursorPosition: return 0x48;
    //         case Command::EraseInDisplay: return 0x4A;
    //         case Command::EraseInLine: return 0x4B;
    //         case Command::ScrollUp: return 0x53;
    //         case Command::ScrollDown: return 0x54;
    //         case Command::HorizontalVerticalPosition: return 0x66;
    //         case Command::SelectGraphicRendition: return 0x6D;
    //         case Command::SingleShiftTwo: return 0x4E;
    //         case Command::SingleShiftThree: return 0x4F;
    //         case Command::DeviceControlString: return 0x50;
    //         case Command::StringTerminator: return 0x5C;
    //         case Command::StartOfString: return 0x58;
    //         case Command::PrivacyMessage: return 0x5E;
    //         case Command::ApplicationProgramCommand: return 0x5F;
    //     }
    // }

    Command GetControlCodeCommand() const
    {
        switch (GetByte(0))
        {
            case 0x00: return Command::Null;
            case 0x01: return Command::StartOfHeading;
            case 0x02: return Command::StartOfText;
            case 0x03: return Command::EndOfText;
            case 0x04: return Command::EndOfTransmission;
            case 0x05: return Command::Enquiry;
            case 0x06: return Command::Acknowledge;
            case 0x07: return Command::Bell;
            case 0x08: return Command::Backspace;
            case 0x09: return Command::HorizontalTab;
            case 0x0A: return Command::LineFeed;
            case 0x0B: return Command::VerticalTabulation;
            case 0x0C: return Command::FormFeed;
            case 0x0D: return Command::CarriageReturn;
            case 0x0E: return Command::ShiftOut;
            case 0x0F: return Command::ShiftIn;
            case 0x10: return Command::DataLinkEscape;
            case 0x11: return Command::DeviceControlOne;
            case 0x12: return Command::DeviceControlTwo;
            case 0x13: return Command::DeviceControlThree;
            case 0x14: return Command::DeviceControlFour;
            case 0x15: return Command::NegativeAcknowledge;
            case 0x16: return Command::SynchronousIdle;
            case 0x17: return Command::EndOfTransmissionBlock;
            case 0x18: return Command::Cancel;
            case 0x19: return Command::EndOfMedium;
            case 0x1A: return Command::Substitute;
            case 0x1B: return Command::Escape;
            case 0x1C: return Command::FileSeparator;
            case 0x1D: return Command::GroupSeparator;
            case 0x1E: return Command::RecordSeparator;
            case 0x1F: return Command::UnitSeparator;
            case 0x7F: return Command::Delete;
            default: return Command::None;
        }
    }

    Command GetControlSequenceIntroducerCommand() const
    {
        switch (GetByte(-1))
        {
            case 0x41: return Command::CursorUp;
            case 0x42: return Command::CursorDown;
            case 0x43: return Command::CursorForward;
            case 0x44: return Command::CursorBack;
            case 0x45: return Command::CursorNextLine;
            case 0x46: return Command::CursorPreviousLine;
            case 0x47: return Command::CursorHorizontalAbsolute;
            case 0x48: return Command::CursorPosition;
            case 0x4A: return Command::EraseInDisplay;
            case 0x4B: return Command::EraseInLine;
            case 0x53: return Command::ScrollUp;
            case 0x54: return Command::ScrollDown;
            case 0x66: return Command::HorizontalVerticalPosition;
            case 0x6D: return Command::SelectGraphicRendition;
            case 0x7E: return Command::DeleteAfter;
            default: return Command::None;
        }
    }

    Command GetOperatingSystemCommand() const
    {
        return Command::None;
    }

    Command GetFeSequenceCommand() const
    {
        switch (GetByte(-1))
        {
            case 0x4E: return Command::SingleShiftTwo;
            case 0x4F: return Command::SingleShiftThree;
            case 0x50: return Command::DeviceControlString;
            case 0x5C: return Command::StringTerminator;
            case 0x58: return Command::StartOfString;
            case 0x5E: return Command::PrivacyMessage;
            case 0x5F: return Command::ApplicationProgramCommand;
            default: return Command::None;
        }
    }

    Command GetFsSequenceCommand() const
    {
        return Command::None;
    }

    Command GetFpSequenceCommand() const
    {
        return Command::None;
    }

    Command GetnFSequenceCommand() const
    {
        return Command::None;
    }

    bool IsExtended() const
    {
        return bytes_length > baseSize;
    }

    enum class State
    {
        Start, End, Error,
        FSequence,
        CSI, CSIParameter, CSIInternal,
        nFSequence,
        Unicode2b1,
        Unicode3b1, Unicode3b2,
        Unicode4b1, Unicode4b2, Unicode4b3,
    };

    static void ReaderFSMStep(State& current_state, const Byte& byte)
    {
        switch (current_state)
        {
            case State::Start:
                if ((byte >= 0x80 and byte <= 0xBF) or // Continuation bytes
                    (byte >= 0xC0 and byte <= 0xC1) or // Unused codes
                    (byte >= 0xF5 and byte <= 0xFF))   // Unused codes
                    current_state = State::Error;
                else if ((byte >= 0x00 and byte <= 0x1A) or // Control codes
                            (byte >= 0x1C and byte <= 0x1F) or // Control codes
                            (byte == 0x7F) or                  // Control codes
                            (byte >= 0x20 and byte <= 0x7E))   // ASCIISymbol symbols
                    current_state = State::End;
                else if (byte == 0x1B) // Escaped sequence
                    current_state = State::FSequence;
                else if (byte >= 0xC2 and byte <= 0xDF) // Start of two-bytes unicode
                    current_state = State::Unicode2b1;
                else if (byte >= 0xE0 and byte <= 0xEF) // Start of three-bytes unicode
                    current_state = State::Unicode3b1;
                else if (byte >= 0xF0 and byte <= 0xF4) // Start of four-bytes unicode
                    current_state = State::Unicode4b1;
                break;

            case State::FSequence:
                if ((byte >= 0x00 and byte <= 0x1F) or // Incorrect for escape-sequence
                    (byte >= 0x7F and byte <= 0xFF) or // Incorrect for escape-sequence
                    (byte == 0x5D)) // FIXME: Operating system commands are not supported
                    current_state = State::Error;
                else if ((byte >= 0x30 and byte <= 0x5A) or
                            (byte == 0x5C) or
                            (byte >= 0x5E and byte <= 0x7E))
                    current_state = State::End;
                else if (byte == 0x5B)
                    current_state = State::CSI;
                else if (byte >= 0x20 and byte <= 0x2F)
                    current_state = State::nFSequence;
                break;
            
            case State::CSI:
            case State::CSIParameter:
                if ((byte >= 0x00 and byte <= 0x1F) or
                    (byte == 0x7F) or
                    (byte >= 0x80 and byte <= 0xFF))
                    current_state = State::Error;
                else if (byte >= 0x40 and byte <= 0x7E)
                    current_state = State::End;
                else if (byte >= 0x30 and byte <= 0x3F)
                    current_state = State::CSIParameter;
                else if (byte >= 0x20 and byte <= 0x2F)
                    current_state = State::CSIInternal;
                break;
            
            case State::CSIInternal:
                if ((byte >= 0x00 and byte <= 0x1F) or
                    (byte >= 0x30 and byte <= 0x3F) or
                    (byte == 0x7F) or
                    (byte >= 0x80 and byte <= 0xFF))
                    current_state = State::Error;
                else if (byte >= 0x40 and byte <= 0x7E)
                    current_state = State::End;
                else if (byte >= 0x20 and byte <= 0x2F)
                    current_state = State::CSIInternal;
                break;
            
            case State::nFSequence:
                if ((byte >= 0x00 and byte <= 0x1F) or
                    (byte == 0x7F) or
                    (byte >= 0x80 and byte <= 0xFF))
                    current_state = State::Error;
                else if (byte >= 0x20 and byte <= 0x2F)
                    current_state = State::nFSequence;
                else if (byte >= 0x30 and byte <= 0x7E)
                    current_state = State::End;                    
                break;
            
            case State::Unicode2b1:
                if ((byte >= 0x00 and byte <= 0x7F) or
                    (byte >= 0xC0 and byte <= 0xFF))
                    current_state = State::Error;
                else
                    current_state = State::End;
                break;
            
            case State::Unicode3b1:
                if ((byte >= 0x00 and byte <= 0x7F) or
                    (byte >= 0xC0 and byte <= 0xFF))
                    current_state = State::Error;
                else
                    current_state = State::Unicode3b2;
                break;
            
            case State::Unicode3b2:
                if ((byte >= 0x00 and byte <= 0x7F) or
                    (byte >= 0xC0 and byte <= 0xFF))
                    current_state = State::Error;
                else
                    current_state = State::End;
                break;
            
            case State::Unicode4b1:
                if ((byte >= 0x00 and byte <= 0x7F) or
                    (byte >= 0xC0 and byte <= 0xFF))
                    current_state = State::Error;
                else
                    current_state = State::Unicode4b2;
                break;
            
            case State::Unicode4b2:
                if ((byte >= 0x00 and byte <= 0x7F) or
                    (byte >= 0xC0 and byte <= 0xFF))
                    current_state = State::Error;
                else
                    current_state = State::Unicode4b3;
                break;
            
            case State::Unicode4b3:
                if ((byte >= 0x00 and byte <= 0x7F) or
                    (byte >= 0xC0 and byte <= 0xFF))
                    current_state = State::Error;
                else
                    current_state = State::End;
                break;
            
        }
    }

    void Initialize(const Byte* _bytes, const size_t _bytes_length)
    {
        if (IsExtended() and extendedBytes)
            delete[] extendedBytes;

        bytes_length = _bytes_length;

        bool extended = IsExtended();

        if (extended)
            extendedBytes = new Byte[_bytes_length];
        
        memcpy(extended ? extendedBytes : bytes, _bytes, bytes_length * sizeof(Byte));

        Byte first = bytes[0];

        if (first >= 0x20 and first <= 0x7E)
            type = Type::ASCIISymbol;
        else if ((first >= 0x00 and first <= 0x1A) or (first >= 0x1C and first <= 0x1F) or first == 0x7F)
            type = Type::ControlCode;
        else if (first == 0x1B)
            type = Type::EscapedSequence;
        else if (first >= 0xC2 and first <= 0xF4)
            type = Type::UnicodeSymbol;
        else
            type = Type::Invalid;

        switch (type)
        {
            case Type::ASCIISymbol:
                display_width = 1;
                break;
            case Type::UnicodeSymbol:
                display_width = WcharUtils::CalculateDisplayWidth(extended ? extendedBytes : bytes, bytes_length);
                break;
            default:
                display_width = 0;
                break;
        }
    }

public:
    UnicodeSymbol() { }
    UnicodeSymbol(Type _type) : type(_type) { }
    UnicodeSymbol(const Byte* _bytes, const size_t _bytes_length) { Initialize(_bytes, _bytes_length); }
    UnicodeSymbol(const Byte _byte) { Initialize(&_byte, 1); }

    ~UnicodeSymbol()
    {
        if (IsExtended())
        {
            delete[] extendedBytes;
            extendedBytes = nullptr;
        }
    }

    UnicodeSymbol(const UnicodeSymbol& other) noexcept
    {
        type = other.type;
        bytes_length = other.bytes_length;
        display_width = other.display_width;

        bool extended = IsExtended();

        if (extended)
            extendedBytes = new Byte[bytes_length];

        memcpy(extended ? extendedBytes : bytes, extended ? other.extendedBytes : other.bytes, baseSize);
    }

    UnicodeSymbol& operator=(const UnicodeSymbol& other) noexcept
    {
        if (this != &other)
        {
            type = other.type;
            bytes_length = other.bytes_length;
            display_width = other.display_width;

            bool extended = IsExtended();

            if (extended)
                extendedBytes = new Byte[bytes_length];

            memcpy(extended ? extendedBytes : bytes, extended ? other.extendedBytes : other.bytes, baseSize);
        }

        return *this;
    }

    UnicodeSymbol(UnicodeSymbol&& other) noexcept
    {
        bytes_length = other.bytes_length;
        type = other.type;
        extendedBytes = other.extendedBytes;
        display_width = other.display_width;

        other.bytes_length = 0;
        other.type = Type::Invalid;
        other.extendedBytes = nullptr;
    }

    UnicodeSymbol& operator=(UnicodeSymbol&& other) noexcept
    {
        if (this != &other)
        {
            if (IsExtended())
                delete[] extendedBytes;

            bytes_length = other.bytes_length;
            type = other.type;
            extendedBytes = other.extendedBytes;
            display_width = other.display_width;

            other.bytes_length = 0;
            other.type = Type::Invalid;
            other.extendedBytes = nullptr;
        }

        return *this;
    }

    Type GetType() const
    {
        return type;
    }

    bool OfType(std::initializer_list<Type> types) const
    {
        for (Type t : types)
            if (GetType() == t)
                return true;
        return false;
    }

    EscapeSequenceType GetSequenceType() const
    {
        if (type == Type::EscapedSequence and bytes_length >= 2)
        {
            Byte b = GetByte(1);

            if (b == 0x5B)
                return EscapeSequenceType::ControlSequenceIntroducer;
            else if (b == 0x5D)
                return EscapeSequenceType::OperatingSystemCommand;
            else if (b >= 0x40 and b <= 0x5F)
                return EscapeSequenceType::Fe;
            else if (b >= 0x60 and b <= 0x7E)
                return EscapeSequenceType::Fs;
            else if (b >= 0x30 and b <= 0x3F)
                return EscapeSequenceType::Fp;
            else if (b >= 0x20 and b <= 0x2F)
                return EscapeSequenceType::nF;
        }

        return EscapeSequenceType::None;
    }

    void DebugWriteTo(FILE* fileno = stdout) const
    {
        char buf[bytes_length + 1];
        memcpy(buf, bytes, bytes_length);
        buf[bytes_length] = 0;

        switch (type)
        {
            case Type::None:
            case Type::Invalid:
                fprintf(fileno, "<INVALID>");
                break;

            case Type::ASCIISymbol:
                fprintf(fileno, "<ASCIISymbol %c>", buf[0]);
                break;

            case Type::ControlCode:
                fprintf(fileno, "<Control code 0x%02x>", buf[0]);
                break;

            case Type::EscapedSequence:
                fprintf(fileno, "<Escaped sequence (%d bytes) \\e%s>\n", bytes_length, buf + sizeof(char));
                break;

            case Type::UnicodeSymbol:
                fprintf(fileno, "<Unicode symbol (%d bytes) ", bytes_length);
                for (size_t idx = 0; idx < bytes_length; idx++)
                    fprintf(fileno, "%02x ", bytes[idx]);
                fprintf(fileno, "%s>", buf);
                break;
        }
    }

    void WriteTo(FILE* fileno = stdout) const
    {
        if (type != Type::None and type != Type::Invalid)
            for (size_t idx = 0; idx < bytes_length; idx++)
                fputc(bytes[idx], fileno);
    }

    bool IsCorrect() const
    {
        return type != Type::None and type != Type::Invalid;
    }

    bool IsCommand() const
    {
        return type == Type::ControlCode or type == Type::EscapedSequence;
    }

    bool IsSymbol() const
    {
        return type == Type::ASCIISymbol or type == Type::UnicodeSymbol;
    }

    Command GetCommand() const
    {
        switch (type)
        {
            case Type::ControlCode: return GetControlCodeCommand();
            case Type::EscapedSequence:
                switch (GetSequenceType())
                {
                    case EscapeSequenceType::ControlSequenceIntroducer: return GetControlSequenceIntroducerCommand();
                    case EscapeSequenceType::OperatingSystemCommand: return GetOperatingSystemCommand();
                    case EscapeSequenceType::Fe: return GetFeSequenceCommand();
                    case EscapeSequenceType::Fs: return GetFsSequenceCommand();
                    case EscapeSequenceType::Fp: return GetFpSequenceCommand();
                    case EscapeSequenceType::nF: return GetnFSequenceCommand();
                    default: return Command::None;
                }
            default: return Command::None;
        }

        return Command::None;
    }

    Byte GetByte(int offset = 0) const
    {
        size_t idx = offset >= 0 ? offset : bytes_length + offset;

        if (idx < 0 or idx >= bytes_length)
            return 0;
            
        return IsExtended() ? extendedBytes[idx] : bytes[idx];
    }

    int GetCSIParameter(size_t position) const
    {
        int result = 0;
        size_t cur_pos = 0;

        for (size_t idx = 0; idx < bytes_length; idx++)
        {
            uint8_t byte = GetByte(idx);
            if (cur_pos == position and byte >= 0x30 and byte <= 0x39)
                result = 10 * result + (byte - 0x30);
            if (byte == 0x3B)
                cur_pos++;
        }

        return result;
    }

    uint8_t DisplayWidth() const
    {
        return display_width;
    }

    static UnicodeSymbol Create(std::function<Byte()> get_next_byte)
    {
        Byte buffer[100] = { 0 };
        size_t buffer_idx = 0;
        State current_state = State::Start;

        while (current_state != State::End and current_state != State::Error)
        {
            buffer[buffer_idx] = get_next_byte();
            ReaderFSMStep(current_state, buffer[buffer_idx++]);
        }

        return UnicodeSymbol(buffer, buffer_idx);
    }

    static void CreateInplace(UnicodeSymbol& destination, std::function<Byte()> get_next_byte)
    {
        Byte buffer[100] = { 0 };
        size_t buffer_idx = 0;
        State current_state = State::Start;

        while (current_state != State::End and current_state != State::Error)
        {
            buffer[buffer_idx] = get_next_byte();
            ReaderFSMStep(current_state, buffer[buffer_idx++]);
        }

        destination.Initialize(buffer, buffer_idx);
    }

    static UnicodeSymbol Create(const Byte*& string_ptr)
    {
        size_t length = 0;
        State current_state = State::Start;

        while (current_state != State::End and current_state != State::Error)
            ReaderFSMStep(current_state, string_ptr[length++]);

        UnicodeSymbol res = UnicodeSymbol(string_ptr, length);
        string_ptr += length;
        return res;
    }

    static void CreateInplace(UnicodeSymbol& destination, const Byte*& string_ptr)
    {
        size_t length = 0;
        State current_state = State::Start;

        while (current_state != State::End and current_state != State::Error)
            ReaderFSMStep(current_state, string_ptr[length++]);

        destination.Initialize(string_ptr, length);
        string_ptr += length;
    }

    const size_t BytesLength() const
    {
        return bytes_length;
    }

    const Byte* AsBytes() const
    {
        return IsExtended() ? (const Byte*) extendedBytes : (const Byte*) &bytes;
    }
};
