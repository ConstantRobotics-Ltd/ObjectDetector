#include "ObjectDetector.h"
#include "ObjectDetectorVersion.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <new>
#include <string>
#include <vector>



namespace
{
// Encoded data stores int and float values as raw 4 bytes values.
static_assert(sizeof(int) == 4, "int must be 4 bytes");
static_assert(sizeof(float) == 4, "float must be 4 bytes");

/// Size of an int or float value in the encoded data, bytes.
constexpr int VALUE_SIZE{4};
/// Size of a bool value in the encoded data, bytes.
constexpr int FLAG_SIZE{1};
/// Data type of the encoded params (the first byte of the encoded params).
constexpr uint8_t PARAMS_DATA_TYPE{0x02};
/// Data type of an encoded action command (the first byte of the command).
constexpr uint8_t ACTION_COMMAND_DATA_TYPE{0x00};
/// Data type of an encoded set param command (the first byte of the command).
constexpr uint8_t SET_PARAM_COMMAND_DATA_TYPE{0x01};
/// Position of the fields mask in the encoded params.
constexpr int PARAMS_MASK_POS{3};
/// Size of the fields mask in the encoded params, bytes.
constexpr int PARAMS_MASK_SIZE{4};
/// Size of the encoded params header: data type, version and mask.
constexpr int PARAMS_HEADER_SIZE{PARAMS_MASK_POS + PARAMS_MASK_SIZE};
/// Number of bits in a byte of the mask.
constexpr int BITS_IN_BYTE{8};
/// The bit of the first field of a mask byte.
constexpr unsigned int FIRST_MASK_BIT{0x80U};
/// Number of the int, float and bool fields of the params in the encoded data.
constexpr int SCALAR_FIELDS_NUMBER{23};
/// Number of the values (int and float) of an encoded object.
constexpr int OBJECT_VALUES_NUMBER{10};
/// Size of an encoded object, bytes.
constexpr int OBJECT_SIZE{OBJECT_VALUES_NUMBER * VALUE_SIZE};
/// Index of the field "objects" in the order of the encoded data (and of the
/// bits of the mask). The init string and the class names follow it.
constexpr int OBJECTS_FIELD{SCALAR_FIELDS_NUMBER};
/// Index of the field "initString".
constexpr int INIT_STRING_FIELD{OBJECTS_FIELD + 1};
/// Index of the field "classNames".
constexpr int CLASS_NAMES_FIELD{INIT_STRING_FIELD + 1};
/// Number of the fields of the params which have a bit in the mask.
constexpr int FIELDS_NUMBER{CLASS_NAMES_FIELD + 1};
/// Position of the command ID in the encoded command.
constexpr int COMMAND_ID_POS{3};
/// Position of the param value in the encoded set param command.
constexpr int COMMAND_VALUE_POS{7};
/// Size of the encoded action command: data type, version and command ID.
constexpr int ACTION_COMMAND_SIZE{COMMAND_ID_POS + VALUE_SIZE};
/// Size of the encoded set param command: action command and param value.
constexpr int SET_PARAM_COMMAND_SIZE{ACTION_COMMAND_SIZE + VALUE_SIZE};
/// Value of all bytes of the mask if the mask is not given to encode(...).
constexpr uint8_t FULL_MASK_BYTE{0xFF};
static_assert(PARAMS_HEADER_SIZE == 7, "wrong size of the params header");
static_assert(SET_PARAM_COMMAND_SIZE == 11, "wrong size of the command");
static_assert(FIELDS_NUMBER <= PARAMS_MASK_SIZE * BITS_IN_BYTE,
              "mask is too small");

/// Flags of the params fields in the order of the encoded data.
using Selection = std::array<bool, FIELDS_NUMBER>;



/// Location of a scalar field of ObjectDetectorParams in the encoded data.
struct FieldRef
{
    /// Pointer to the field of an ObjectDetectorParams object.
    void* value{nullptr};
    /// Size of the field in the encoded data: VALUE_SIZE or FLAG_SIZE.
    int size{0};
};

/// Locations of all scalar fields of ObjectDetectorParams.
using FieldTable = std::array<FieldRef, SCALAR_FIELDS_NUMBER>;



/// Get the table of the scalar fields of ObjectDetectorParams in the order they
/// are stored in the encoded data (the same order as the bits of the mask).
/// Encode and decode use the same table: the formats cannot diverge.
FieldTable makeFieldTable(rapidpixel::ObjectDetectorParams& params)
{
    return {{
        {&params.logMode, VALUE_SIZE},
        {&params.frameBufferSize, VALUE_SIZE},
        {&params.minObjectWidth, VALUE_SIZE},
        {&params.maxObjectWidth, VALUE_SIZE},
        {&params.minObjectHeight, VALUE_SIZE},
        {&params.maxObjectHeight, VALUE_SIZE},
        {&params.minXSpeed, VALUE_SIZE},
        {&params.maxXSpeed, VALUE_SIZE},
        {&params.minYSpeed, VALUE_SIZE},
        {&params.maxYSpeed, VALUE_SIZE},
        {&params.xDetectionCriteria, VALUE_SIZE},
        {&params.yDetectionCriteria, VALUE_SIZE},
        {&params.resetCriteria, VALUE_SIZE},
        {&params.sensitivity, VALUE_SIZE},
        {&params.scaleFactor, VALUE_SIZE},
        {&params.numThreads, VALUE_SIZE},
        {&params.processingTimeMcs, VALUE_SIZE},
        {&params.type, VALUE_SIZE},
        {&params.enable, FLAG_SIZE},
        {&params.custom1, VALUE_SIZE},
        {&params.custom2, VALUE_SIZE},
        {&params.custom3, VALUE_SIZE},
        {&params.minDetectionProbability, VALUE_SIZE}
    }};
}



/// Get the values of an Object in the order they are stored in the encoded
/// data. All of them are int or float values of VALUE_SIZE bytes.
std::array<void*, OBJECT_VALUES_NUMBER> makeObjectTable(
        rapidpixel::Object& obj)
{
    return {{&obj.id, &obj.frameId, &obj.type, &obj.width, &obj.height,
             &obj.x, &obj.y, &obj.vX, &obj.vY, &obj.p}};
}



/// Get the flags of the params fields which are selected by the mask in the
/// order of the encoded data. All fields are selected if the mask is nullptr.
Selection makeSelection(const rapidpixel::ObjectDetectorParamsMask* mask)
{
    if (mask == nullptr)
    {
        Selection all{};
        all.fill(true);
        return all;
    }

    return {{
        mask->logMode,
        mask->frameBufferSize,
        mask->minObjectWidth,
        mask->maxObjectWidth,
        mask->minObjectHeight,
        mask->maxObjectHeight,
        mask->minXSpeed,
        mask->maxXSpeed,
        mask->minYSpeed,
        mask->maxYSpeed,
        mask->xDetectionCriteria,
        mask->yDetectionCriteria,
        mask->resetCriteria,
        mask->sensitivity,
        mask->scaleFactor,
        mask->numThreads,
        mask->processingTimeMcs,
        mask->type,
        mask->enable,
        mask->custom1,
        mask->custom2,
        mask->custom3,
        mask->minDetectionProbability,
        mask->objects,
        mask->initString,
        mask->classNames
    }};
}



/// Get the position of the mask byte which holds the bit of the field.
int maskBytePos(int field)
{
    return PARAMS_MASK_POS + field / BITS_IN_BYTE;
}



/// Get the bit of the field inside its mask byte: the first field of a byte
/// has the most significant bit.
uint8_t maskBit(int field)
{
    return static_cast<uint8_t>(FIRST_MASK_BIT >> (field % BITS_IN_BYTE));
}



/// Get the flags of the fields which are selected by the mask in the encoded
/// data. The data must have the complete header.
Selection readSelection(const uint8_t* data)
{
    Selection selected{};
    for (int field = 0; field < FIELDS_NUMBER; ++field)
    {
        selected[static_cast<size_t>(field)] =
                (data[maskBytePos(field)] & maskBit(field)) != 0;
    }

    return selected;
}



/// Write the mask of the selected fields. All mask bytes are set if the mask
/// is not given.
void writeMask(uint8_t* data, bool hasMask, const Selection& selected)
{
    std::memset(&data[PARAMS_MASK_POS], hasMask ? 0 : FULL_MASK_BYTE,
                PARAMS_MASK_SIZE);
    if (!hasMask)
    {
        return;
    }
    for (int field = 0; field < FIELDS_NUMBER; ++field)
    {
        if (selected[static_cast<size_t>(field)])
        {
            data[maskBytePos(field)] |= maskBit(field);
        }
    }
}



/// Check that the init string or a class name can be decoded: null character
/// terminates a string in the encoded data.
bool hasNullCharacter(const std::string& text)
{
    return text.find('\0') != std::string::npos;
}



/// Check that the list of class names can be decoded: the names have no null
/// characters and the empty name is only the last one (an empty name
/// terminates the list in the encoded data).
bool isEncodableClassNames(const std::vector<std::string>& names)
{
    for (size_t i = 0; i < names.size(); ++i)
    {
        if (hasNullCharacter(names[i]))
        {
            return false;
        }
        if (names[i].empty() && i + 1 != names.size())
        {
            return false;
        }
    }

    return true;
}



/// Check that the selected strings of the params can be decoded.
bool isEncodableStrings(const rapidpixel::ObjectDetectorParams& params,
                        const Selection& selected)
{
    if (selected[INIT_STRING_FIELD] && hasNullCharacter(params.initString))
    {
        return false;
    }

    return !selected[CLASS_NAMES_FIELD] ||
            isEncodableClassNames(params.classNames);
}



/// Calculate the size of the encoded selected scalar fields and of the
/// object counter, bytes. The header is included.
size_t fixedSizeOf(const FieldTable& fields, const Selection& selected)
{
    size_t size = PARAMS_HEADER_SIZE;
    for (size_t i = 0; i < fields.size(); ++i)
    {
        if (selected[i])
        {
            size += static_cast<size_t>(fields[i].size);
        }
    }
    if (selected[OBJECTS_FIELD])
    {
        size += VALUE_SIZE;
    }

    return size;
}



/// Calculate the size of the encoded selected strings with their terminators,
/// bytes. The size is calculated as size_t: the strings can be longer than the
/// buffer.
size_t stringsSizeOf(const rapidpixel::ObjectDetectorParams& params,
                     const Selection& selected)
{
    size_t size = 0;
    if (selected[INIT_STRING_FIELD])
    {
        size += params.initString.size() + 1;
    }
    if (selected[CLASS_NAMES_FIELD])
    {
        for (const std::string& name : params.classNames)
        {
            size += name.size() + 1;
        }
    }

    return size;
}



/// Write the selected scalar fields at the position pos of the data and return
/// the position after the last of them. The room must be checked before.
size_t writeScalars(uint8_t* data, size_t pos, const FieldTable& fields,
                    const Selection& selected)
{
    for (size_t i = 0; i < fields.size(); ++i)
    {
        if (!selected[i])
        {
            continue;
        }
        if (fields[i].size == FLAG_SIZE)
        {
            const bool flag = *static_cast<const bool*>(fields[i].value);
            data[pos] = flag ? 0x01 : 0x00;
        }
        else
        {
            std::memcpy(&data[pos], fields[i].value, VALUE_SIZE);
        }
        pos += static_cast<size_t>(fields[i].size);
    }

    return pos;
}



/// Write the counter and the first number objects at the position pos of the
/// data and return the position after the last of them. The room must be
/// checked before.
size_t writeObjects(uint8_t* data, size_t pos,
                    std::vector<rapidpixel::Object>& objects, size_t number)
{
    const int counter = static_cast<int>(number);
    std::memcpy(&data[pos], &counter, VALUE_SIZE);
    pos += VALUE_SIZE;
    for (size_t i = 0; i < number; ++i)
    {
        for (const void* value : makeObjectTable(objects[i]))
        {
            std::memcpy(&data[pos], value, VALUE_SIZE);
            pos += VALUE_SIZE;
        }
    }

    return pos;
}



/// Write the selected strings at the position pos of the data and return the
/// position after the last of them. The room must be checked before. The
/// terminators are encoded to recognize the end of a string.
size_t writeStrings(uint8_t* data, size_t pos,
                    const rapidpixel::ObjectDetectorParams& params,
                    const Selection& selected)
{
    if (selected[INIT_STRING_FIELD])
    {
        std::memcpy(&data[pos], params.initString.c_str(),
                    params.initString.size() + 1);
        pos += params.initString.size() + 1;
    }
    if (selected[CLASS_NAMES_FIELD])
    {
        for (const std::string& name : params.classNames)
        {
            std::memcpy(&data[pos], name.c_str(), name.size() + 1);
            pos += name.size() + 1;
        }
    }

    return pos;
}



/// Read the selected scalar fields at the position pos of the data. The fields
/// which are not selected are set to zero. Returns false if a field is not
/// inside the data.
bool readScalars(const uint8_t* data, int dataSize, int& pos,
                 const FieldTable& fields, const Selection& selected)
{
    for (size_t i = 0; i < fields.size(); ++i)
    {
        if (!selected[i])
        {
            std::memset(fields[i].value, 0, static_cast<size_t>(fields[i].size));
            continue;
        }

        // Check that the field is inside the data.
        if (dataSize - pos < fields[i].size)
        {
            return false;
        }
        if (fields[i].size == FLAG_SIZE)
        {
            *static_cast<bool*>(fields[i].value) = data[pos] != 0x00;
        }
        else
        {
            std::memcpy(fields[i].value, &data[pos], VALUE_SIZE);
        }
        pos += fields[i].size;
    }

    return true;
}



/// Read the counter and the objects at the position pos of the data. The
/// number of objects is checked against the rest of the data before the memory
/// is reserved, so the memory is limited by the size of the data.
bool readObjects(const uint8_t* data, int dataSize, int& pos,
                 std::vector<rapidpixel::Object>& objects)
{
    if (dataSize - pos < VALUE_SIZE)
    {
        return false;
    }
    int counter = 0;
    std::memcpy(&counter, &data[pos], VALUE_SIZE);
    pos += VALUE_SIZE;
    if (counter < 0 || counter > (dataSize - pos) / OBJECT_SIZE)
    {
        return false;
    }

    objects.reserve(static_cast<size_t>(counter));
    for (int i = 0; i < counter; ++i)
    {
        rapidpixel::Object obj;
        for (void* value : makeObjectTable(obj))
        {
            std::memcpy(value, &data[pos], VALUE_SIZE);
            pos += VALUE_SIZE;
        }
        objects.push_back(obj);
    }

    return true;
}



/// Find the null terminated string which starts at the position pos of the
/// data. Returns the length of the string (without the terminator) or -1 if
/// the terminator is not inside the data.
int findStringLength(const uint8_t* data, int dataSize, int pos)
{
    if (pos >= dataSize)
    {
        return -1;
    }
    const void* end = std::memchr(
                &data[pos], 0, static_cast<size_t>(dataSize - pos));
    if (end == nullptr)
    {
        return -1;
    }

    return static_cast<int>(static_cast<const uint8_t*>(end) - &data[pos]);
}



/// Read the selected strings at the position pos of the data. The init string
/// must be terminated inside the data. The class names are null terminated
/// strings up to the end of the data, the empty name terminates the list.
bool readStrings(const uint8_t* data, int dataSize, int& pos,
                 const Selection& selected,
                 rapidpixel::ObjectDetectorParams& params)
{
    if (selected[INIT_STRING_FIELD])
    {
        const int length = findStringLength(data, dataSize, pos);
        if (length < 0)
        {
            return false;
        }
        params.initString.assign(reinterpret_cast<const char*>(&data[pos]),
                                 static_cast<size_t>(length));
        pos += length + 1;
    }

    while (selected[CLASS_NAMES_FIELD] && pos < dataSize)
    {
        const int length = findStringLength(data, dataSize, pos);
        if (length < 0)
        {
            return false;
        }
        const char* name = reinterpret_cast<const char*>(&data[pos]);
        pos += length + 1;
        if (length == 0)
        {
            break;
        }
        params.classNames.emplace_back(name, static_cast<size_t>(length));
    }

    return true;
}
}



rapidpixel::ObjectDetector::~ObjectDetector() = default;



std::string rapidpixel::ObjectDetector::getVersion()
{
    return OBJECT_DETECTOR_VERSION;
}



bool rapidpixel::ObjectDetectorParams::encode(
        uint8_t* data, int bufferSize, int& size,
        ObjectDetectorParamsMask* mask)
{
    size = 0;
    if (data == nullptr || bufferSize < PARAMS_HEADER_SIZE)
    {
        return false;
    }

    // The strings are terminated by the null character in the encoded data:
    // the decoder cannot restore a string with such a character inside.
    const Selection selected = makeSelection(mask);
    if (!isEncodableStrings(*this, selected))
    {
        return false;
    }

    // All fields except the objects must fit into the buffer. The objects
    // are put into the rest of the buffer, the number of encoded objects is
    // reduced if there is no room for all of them.
    const FieldTable fields = makeFieldTable(*this);
    const size_t fixedSize = fixedSizeOf(fields, selected);
    const size_t stringsSize = stringsSizeOf(*this, selected);
    const size_t capacity = static_cast<size_t>(bufferSize);
    if (fixedSize > capacity || stringsSize > capacity - fixedSize)
    {
        return false;
    }
    size_t objectsNumber = 0;
    if (selected[OBJECTS_FIELD])
    {
        const size_t room = (capacity - fixedSize - stringsSize) / OBJECT_SIZE;
        objectsNumber = objects.size() < room ? objects.size() : room;
    }

    // Encode the data type, the version, the mask and the selected fields.
    data[0] = PARAMS_DATA_TYPE;
    data[1] = static_cast<uint8_t>(OBJECT_DETECTOR_MAJOR_VERSION);
    data[2] = static_cast<uint8_t>(OBJECT_DETECTOR_MINOR_VERSION);
    writeMask(data, mask != nullptr, selected);
    size_t pos = writeScalars(data, PARAMS_HEADER_SIZE, fields, selected);
    if (selected[OBJECTS_FIELD])
    {
        pos = writeObjects(data, pos, objects, objectsNumber);
    }
    pos = writeStrings(data, pos, *this, selected);
    size = static_cast<int>(pos);

    return true;
}



bool rapidpixel::ObjectDetectorParams::decode(uint8_t* data, int dataSize)
{
    // Check data size (the header must be complete), data type and version.
    if (data == nullptr || dataSize < PARAMS_HEADER_SIZE)
    {
        return false;
    }
    if (data[0] != PARAMS_DATA_TYPE)
    {
        return false;
    }
    if (data[1] != OBJECT_DETECTOR_MAJOR_VERSION ||
        data[2] != OBJECT_DETECTOR_MINOR_VERSION)
    {
        return false;
    }

    // Decode into a temporary object: this object is changed only if the
    // whole data is valid. Fields not selected by the mask are set to zero.
    // The memory of the temporary object is limited by the size of the data:
    // an object takes OBJECT_SIZE bytes of the data and a class name takes at
    // least two bytes.
    try
    {
        ObjectDetectorParams result;
        result.objects.clear();
        result.initString.clear();
        result.classNames.clear();
        const FieldTable fields = makeFieldTable(result);
        const Selection selected = readSelection(data);
        int pos = PARAMS_HEADER_SIZE;
        if (!readScalars(data, dataSize, pos, fields, selected))
        {
            return false;
        }
        if (selected[OBJECTS_FIELD] &&
            !readObjects(data, dataSize, pos, result.objects))
        {
            return false;
        }
        if (!readStrings(data, dataSize, pos, selected, result))
        {
            return false;
        }

        // The data must be used completely.
        if (pos != dataSize)
        {
            return false;
        }

        // Commit. Swap of the containers and copy of the scalar fields do
        // not throw: the object is not changed if the decoding failed.
        const FieldTable target = makeFieldTable(*this);
        for (size_t i = 0; i < target.size(); ++i)
        {
            std::memcpy(target[i].value, fields[i].value,
                        static_cast<size_t>(target[i].size));
        }
        initString.swap(result.initString);
        objects.swap(result.objects);
        classNames.swap(result.classNames);
    }
    catch (const std::bad_alloc&)
    {
        return false;
    }

    return true;
}



void rapidpixel::ObjectDetector::encodeSetParamCommand(
        uint8_t* data, int& size, ObjectDetectorParam id, float value)
{
    size = 0;
    if (data == nullptr)
    {
        return;
    }

    // Fill header.
    data[0] = SET_PARAM_COMMAND_DATA_TYPE;
    data[1] = static_cast<uint8_t>(OBJECT_DETECTOR_MAJOR_VERSION);
    data[2] = static_cast<uint8_t>(OBJECT_DETECTOR_MINOR_VERSION);

    // Fill data.
    const int paramId = static_cast<int>(id);
    std::memcpy(&data[COMMAND_ID_POS], &paramId, VALUE_SIZE);
    std::memcpy(&data[COMMAND_VALUE_POS], &value, VALUE_SIZE);
    size = SET_PARAM_COMMAND_SIZE;
}



void rapidpixel::ObjectDetector::encodeCommand(
        uint8_t* data, int& size, ObjectDetectorCommand id)
{
    size = 0;
    if (data == nullptr)
    {
        return;
    }

    // Fill header.
    data[0] = ACTION_COMMAND_DATA_TYPE;
    data[1] = static_cast<uint8_t>(OBJECT_DETECTOR_MAJOR_VERSION);
    data[2] = static_cast<uint8_t>(OBJECT_DETECTOR_MINOR_VERSION);

    // Fill data.
    const int commandId = static_cast<int>(id);
    std::memcpy(&data[COMMAND_ID_POS], &commandId, VALUE_SIZE);
    size = ACTION_COMMAND_SIZE;
}



int rapidpixel::ObjectDetector::decodeCommand(
        uint8_t* data, int size, ObjectDetectorParam& paramId,
        ObjectDetectorCommand& commandId, float& value)
{
    // Check pointer, minimum size and version.
    if (data == nullptr || size < ACTION_COMMAND_SIZE)
    {
        return -1;
    }
    if (data[1] != OBJECT_DETECTOR_MAJOR_VERSION ||
        data[2] != OBJECT_DETECTOR_MINOR_VERSION)
    {
        return -1;
    }

    // Extract the ID.
    int rawId = 0;
    std::memcpy(&rawId, &data[COMMAND_ID_POS], VALUE_SIZE);

    // Check command type, exact size of the command and range of the ID: an
    // unknown ID is not passed to the implementation.
    if (data[0] == ACTION_COMMAND_DATA_TYPE)
    {
        if (size != ACTION_COMMAND_SIZE ||
            rawId < static_cast<int>(ObjectDetectorCommand::RESET) ||
            rawId > static_cast<int>(ObjectDetectorCommand::OFF))
        {
            return -1;
        }
        commandId = static_cast<ObjectDetectorCommand>(rawId);
        value = 0.0f;
        return 0;
    }
    if (data[0] == SET_PARAM_COMMAND_DATA_TYPE)
    {
        if (size != SET_PARAM_COMMAND_SIZE ||
            rawId < static_cast<int>(ObjectDetectorParam::LOG_MODE) ||
            rawId > static_cast<int>(ObjectDetectorParam::CUSTOM_3))
        {
            return -1;
        }
        paramId = static_cast<ObjectDetectorParam>(rawId);
        std::memcpy(&value, &data[COMMAND_VALUE_POS], VALUE_SIZE);
        return 1;
    }

    return -1;
}
