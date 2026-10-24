#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>
#include "ConfigReader.h"
#include "ObjectDetector.h"
#include "ObjectDetectorVersion.h"



namespace
{
namespace det = rapidpixel;

using Params = det::ObjectDetectorParams;
using Mask = det::ObjectDetectorParamsMask;
using Command = det::ObjectDetectorCommand;
using Param = det::ObjectDetectorParam;

/// State of the test random generator. The tests use own deterministic
/// generator: results must be reproducible on all platforms.
uint32_t g_randomState{20260929U};



/// Get next random number.
uint32_t nextRandom()
{
    g_randomState = g_randomState * 1664525U + 1013904223U;
    return g_randomState >> 8;
}



/// Get next random number in [0, limit).
int randomInt(int limit)
{
    return static_cast<int>(nextRandom() % static_cast<uint32_t>(limit));
}



/// Get 32 random bits: any bit pattern, including NaN and infinity if the
/// bits are used as float.
uint32_t randomBits()
{
    return ((nextRandom() & 0xFFFFU) << 16) | (nextRandom() & 0xFFFFU);
}



/// Get the bits of an int or float value. Floats are compared by their bits.
template <class T>
uint32_t bitsOf(T value)
{
    static_assert(sizeof(T) == sizeof(uint32_t), "wrong size of the value");
    uint32_t bits = 0;
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}



/// Make a value of the type T from the bits.
template <class T>
T fromBits(uint32_t bits)
{
    static_assert(sizeof(T) == sizeof(uint32_t), "wrong size of the value");
    T value{};
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}



/// Print a failed check and return false.
bool fail(const char* message)
{
    std::cout << message << std::endl;
    return false;
}



/// Run one test and print the result.
bool runTest(const char* title, bool (*test)())
{
    std::cout << title << ":" << std::endl;
    const bool passed = test();
    std::cout << (passed ? "OK" : "ERROR") << std::endl << std::endl;
    return passed;
}



/// Removes a file. The result of the removal is checked and reported.
class FileRemover
{
public:
    explicit FileRemover(std::string name) : m_name(std::move(name)) {}
    FileRemover(const FileRemover&) = delete;
    FileRemover(FileRemover&&) = delete;
    FileRemover& operator= (const FileRemover&) = delete;
    FileRemover& operator= (FileRemover&&) = delete;

    ~FileRemover()
    {
        if (!m_removed)
        {
            remove();
        }
    }

    /// Remove the file. Returns false (and prints a message) if not removed.
    bool remove()
    {
        m_removed = true;
        if (std::remove(m_name.c_str()) != 0)
        {
            std::cout << "Can't remove file " << m_name << std::endl;
            return false;
        }
        return true;
    }

private:
    /// Name of the file.
    std::string m_name;
    /// The removal was already tried.
    bool m_removed{false};
};



/// Description of an int field of the params.
struct IntField
{
    /// Name of the field.
    const char* name;
    /// Pointer to the field.
    int Params::* value;
    /// Pointer to the flag of the field in the mask.
    bool Mask::* flag;
};



/// Description of a float field of the params.
struct FloatField
{
    /// Name of the field.
    const char* name;
    /// Pointer to the field.
    float Params::* value;
    /// Pointer to the flag of the field in the mask.
    bool Mask::* flag;
};



/// Int fields of the params.
const std::array<IntField, 13> INT_FIELDS{{
    {"logMode", &Params::logMode, &Mask::logMode},
    {"frameBufferSize", &Params::frameBufferSize, &Mask::frameBufferSize},
    {"minObjectWidth", &Params::minObjectWidth, &Mask::minObjectWidth},
    {"maxObjectWidth", &Params::maxObjectWidth, &Mask::maxObjectWidth},
    {"minObjectHeight", &Params::minObjectHeight, &Mask::minObjectHeight},
    {"maxObjectHeight", &Params::maxObjectHeight, &Mask::maxObjectHeight},
    {"xDetectionCriteria", &Params::xDetectionCriteria,
     &Mask::xDetectionCriteria},
    {"yDetectionCriteria", &Params::yDetectionCriteria,
     &Mask::yDetectionCriteria},
    {"resetCriteria", &Params::resetCriteria, &Mask::resetCriteria},
    {"scaleFactor", &Params::scaleFactor, &Mask::scaleFactor},
    {"numThreads", &Params::numThreads, &Mask::numThreads},
    {"processingTimeMcs", &Params::processingTimeMcs,
     &Mask::processingTimeMcs},
    {"type", &Params::type, &Mask::type}
}};



/// Float fields of the params.
const std::array<FloatField, 9> FLOAT_FIELDS{{
    {"minXSpeed", &Params::minXSpeed, &Mask::minXSpeed},
    {"maxXSpeed", &Params::maxXSpeed, &Mask::maxXSpeed},
    {"minYSpeed", &Params::minYSpeed, &Mask::minYSpeed},
    {"maxYSpeed", &Params::maxYSpeed, &Mask::maxYSpeed},
    {"minDetectionProbability", &Params::minDetectionProbability,
     &Mask::minDetectionProbability},
    {"sensitivity", &Params::sensitivity, &Mask::sensitivity},
    {"custom1", &Params::custom1, &Mask::custom1},
    {"custom2", &Params::custom2, &Mask::custom2},
    {"custom3", &Params::custom3, &Mask::custom3}
}};



/// Flags of the mask in the order of the fields in the encoded data (the same
/// order as the bits of the mask, the first field has the most significant bit
/// of the first mask byte). The format is part of the protocol, so the order is
/// written here independently of the implementation.
const std::array<bool Mask::*, 26> WIRE_FLAGS{{
    &Mask::logMode, &Mask::frameBufferSize, &Mask::minObjectWidth,
    &Mask::maxObjectWidth, &Mask::minObjectHeight, &Mask::maxObjectHeight,
    &Mask::minXSpeed, &Mask::maxXSpeed,
    &Mask::minYSpeed, &Mask::maxYSpeed, &Mask::xDetectionCriteria,
    &Mask::yDetectionCriteria, &Mask::resetCriteria, &Mask::sensitivity,
    &Mask::scaleFactor, &Mask::numThreads,
    &Mask::processingTimeMcs, &Mask::type, &Mask::enable, &Mask::custom1,
    &Mask::custom2, &Mask::custom3, &Mask::minDetectionProbability,
    &Mask::objects,
    &Mask::initString, &Mask::classNames
}};

/// Size of the encoded data header: data type, version and mask.
constexpr int HEADER_SIZE{7};
/// Position of the mask in the encoded data.
constexpr int MASK_POS{3};
/// Size of an encoded value (int or float).
constexpr int VALUE_SIZE{4};
/// Size of an encoded object.
constexpr int OBJECT_SIZE{40};
/// Position of the object counter in the encoded data with all fields: the
/// header, 22 int and float fields and the bool field of enable go first.
constexpr size_t OBJECTS_COUNTER_POS{HEADER_SIZE + 4 * 22 + 1};
/// Position of the byte of the field enable in the encoded data with all
/// fields: the header and 18 int and float fields go first.
constexpr size_t ENABLE_POS{HEADER_SIZE + 4 * 18};
/// Position of the command ID in the encoded command.
constexpr size_t COMMAND_ID_POS{3};
/// Size of the encoded set param command.
constexpr int SET_PARAM_COMMAND_SIZE{11};
/// Size of the encoded action command.
constexpr int ACTION_COMMAND_SIZE{7};
/// ID of the last param (see ObjectDetectorParam enum).
constexpr int LAST_PARAM_ID{23};
/// ID of the last command (see ObjectDetectorCommand enum).
constexpr int LAST_COMMAND_ID{3};
/// Size of the buffer to encode the params of the tests.
constexpr int BUFFER_SIZE{4096};
/// Number of the guard bytes after a buffer to detect writes behind it.
constexpr size_t GUARD_SIZE{16};
/// Value of the guard bytes.
constexpr uint8_t GUARD_VALUE{0xA5};



/// Get a random string without the null character (length is 0..maxLength).
std::string randomString(int maxLength, bool allowEmpty)
{
    int length = randomInt(maxLength + 1);
    if (length == 0 && !allowEmpty)
    {
        length = 1;
    }
    std::string text;
    for (int i = 0; i < length; ++i)
    {
        text.push_back(static_cast<char>(1 + randomInt(255)));
    }
    return text;
}



/// Fill the params with random values. The list of class names has only
/// non-empty names.
void randomizeParams(Params& p, int maxObjects)
{
    for (const IntField& field : INT_FIELDS)
    {
        p.*(field.value) = fromBits<int>(randomBits());
    }
    for (const FloatField& field : FLOAT_FIELDS)
    {
        p.*(field.value) = fromBits<float>(randomBits());
    }
    p.enable = randomInt(2) != 0;
    p.initString = randomString(40, true);
    p.objects.clear();
    const int objectsNumber = randomInt(maxObjects + 1);
    for (int i = 0; i < objectsNumber; ++i)
    {
        det::Object obj;
        obj.id = fromBits<int>(randomBits());
        obj.frameId = fromBits<int>(randomBits());
        obj.type = fromBits<int>(randomBits());
        obj.width = fromBits<int>(randomBits());
        obj.height = fromBits<int>(randomBits());
        obj.x = fromBits<int>(randomBits());
        obj.y = fromBits<int>(randomBits());
        obj.vX = fromBits<float>(randomBits());
        obj.vY = fromBits<float>(randomBits());
        obj.p = fromBits<float>(randomBits());
        p.objects.push_back(obj);
    }
    p.classNames.clear();
    const int namesNumber = randomInt(11);
    for (int i = 0; i < namesNumber; ++i)
    {
        p.classNames.push_back(randomString(16, false));
    }
}



/// Get a random mask.
Mask randomMask()
{
    Mask mask;
    for (bool Mask::* const flag : WIRE_FLAGS)
    {
        mask.*flag = randomInt(2) != 0;
    }
    return mask;
}



/// Get the mask without selected fields.
Mask emptyMask()
{
    Mask mask;
    for (bool Mask::* const flag : WIRE_FLAGS)
    {
        mask.*flag = false;
    }
    return mask;
}



/// Compare the params and print the names of the different fields. Returns
/// the number of different fields.
int compareParams(const Params& expected, const Params& actual)
{
    int errors = 0;
    for (const IntField& field : INT_FIELDS)
    {
        if (expected.*(field.value) != actual.*(field.value))
        {
            std::cout << "Mismatch in field " << field.name << std::endl;
            ++errors;
        }
    }
    for (const FloatField& field : FLOAT_FIELDS)
    {
        if (bitsOf(expected.*(field.value)) != bitsOf(actual.*(field.value)))
        {
            std::cout << "Mismatch in field " << field.name << std::endl;
            ++errors;
        }
    }
    if (expected.enable != actual.enable)
    {
        std::cout << "Mismatch in field enable" << std::endl;
        ++errors;
    }
    if (expected.initString != actual.initString)
    {
        std::cout << "Mismatch in field initString" << std::endl;
        ++errors;
    }
    if (expected.classNames != actual.classNames)
    {
        std::cout << "Mismatch in field classNames" << std::endl;
        ++errors;
    }
    if (expected.objects.size() != actual.objects.size())
    {
        std::cout << "Mismatch in field objects (size)" << std::endl;
        return errors + 1;
    }
    for (size_t i = 0; i < expected.objects.size(); ++i)
    {
        const det::Object& e = expected.objects[i];
        const det::Object& a = actual.objects[i];
        if (e.id != a.id || e.frameId != a.frameId || e.type != a.type ||
            e.width != a.width || e.height != a.height || e.x != a.x ||
            e.y != a.y || bitsOf(e.vX) != bitsOf(a.vX) ||
            bitsOf(e.vY) != bitsOf(a.vY) || bitsOf(e.p) != bitsOf(a.p))
        {
            std::cout << "Mismatch in object " << i << std::endl;
            ++errors;
        }
    }

    return errors;
}



/// Get the params which decode(...) must return for the params encoded with
/// the mask: fields which are not selected are zero (empty), the empty last
/// class name is not restored.
Params expectedAfterDecode(const Params& in, const Mask& mask)
{
    Params r;
    for (const IntField& field : INT_FIELDS)
    {
        r.*(field.value) = (mask.*(field.flag)) ? in.*(field.value) : 0;
    }
    for (const FloatField& field : FLOAT_FIELDS)
    {
        r.*(field.value) = (mask.*(field.flag)) ? in.*(field.value) : 0.0f;
    }
    r.enable = mask.enable && in.enable;
    r.objects.clear();
    if (mask.objects)
    {
        r.objects = in.objects;
    }
    r.initString.clear();
    if (mask.initString)
    {
        r.initString = in.initString;
    }
    r.classNames.clear();
    if (mask.classNames)
    {
        r.classNames = in.classNames;
        if (!r.classNames.empty() && r.classNames.back().empty())
        {
            r.classNames.pop_back();
        }
    }

    return r;
}



/// Calculate the size of the encoded data without the objects, the strings and
/// the object counter, bytes.
int fixedSize(const Mask& mask)
{
    int size = HEADER_SIZE;
    for (const IntField& field : INT_FIELDS)
    {
        size += (mask.*(field.flag)) ? VALUE_SIZE : 0;
    }
    for (const FloatField& field : FLOAT_FIELDS)
    {
        size += (mask.*(field.flag)) ? VALUE_SIZE : 0;
    }
    size += mask.enable ? 1 : 0;
    size += mask.objects ? VALUE_SIZE : 0;

    return size;
}



/// Calculate the size of the encoded data, bytes.
int expectedSize(const Params& p, const Mask& mask)
{
    int size = fixedSize(mask);
    if (mask.objects)
    {
        size += static_cast<int>(p.objects.size()) * OBJECT_SIZE;
    }
    if (mask.initString)
    {
        size += static_cast<int>(p.initString.size()) + 1;
    }
    if (mask.classNames)
    {
        for (const std::string& name : p.classNames)
        {
            size += static_cast<int>(name.size()) + 1;
        }
    }

    return size;
}



/// Encode the params into a new vector. Returns an empty vector if the
/// encoding failed.
std::vector<uint8_t> encodeToVector(Params& p, const Mask* mask)
{
    std::vector<uint8_t> buffer(BUFFER_SIZE);
    Mask copy;
    if (mask != nullptr)
    {
        copy = *mask;
    }
    int size = -1;
    const bool ok = p.encode(buffer.data(), BUFFER_SIZE, size,
                             mask != nullptr ? &copy : nullptr);
    if (!ok || size < HEADER_SIZE || size > BUFFER_SIZE)
    {
        return std::vector<uint8_t>();
    }
    buffer.resize(static_cast<size_t>(size));

    return buffer;
}



/// Decode data which is copied into a buffer of the exact size: the sanitizers
/// see every read behind the data.
bool decodeExact(Params& p, const std::vector<uint8_t>& data)
{
    std::vector<uint8_t> exact(data);
    uint8_t dummy = 0;
    return p.decode(exact.empty() ? &dummy : exact.data(),
                    static_cast<int>(exact.size()));
}



/// Get the params with the default values, a name and no objects: the
/// params of the tests which need a stable encoding.
Params makeFixedParams()
{
    Params p;
    p.initString = "init";
    p.classNames = {"a", "bb", "ccc"};
    p.logMode = 3;
    p.minXSpeed = 1.5f;
    for (int i = 0; i < 3; ++i)
    {
        det::Object obj;
        obj.id = i + 1;
        obj.frameId = 100 + i;
        obj.width = 10 * (i + 1);
        obj.vX = 0.5f * static_cast<float>(i);
        obj.p = 0.25f;
        p.objects.push_back(obj);
    }

    return p;
}



// Copy test.
bool copyTest()
{
    int errors = 0;
    for (int i = 0; i < 100; ++i)
    {
        Params in;
        randomizeParams(in, 5);
        in.processingTimeMcs = static_cast<int>(randomBits());

        // Copy constructor.
        const Params copy(in);
        errors += compareParams(in, copy);

        // Copy operator.
        Params assigned;
        assigned = in;
        errors += compareParams(in, assigned);

        // Self assignment.
        const Params& self = assigned;
        assigned = self;
        errors += compareParams(in, assigned);
    }

    return errors == 0;
}



// Encode/decode test.
bool encodeDecodeTest()
{
    int errors = 0;
    for (int i = 0; i < 3000; ++i)
    {
        Params in;
        randomizeParams(in, 6);
        const bool useMask = (i % 4) != 0;
        Mask mask = useMask ? randomMask() : Mask();

        // Encode data.
        uint8_t data[BUFFER_SIZE];
        int size = -1;
        if (!in.encode(data, BUFFER_SIZE, size, useMask ? &mask : nullptr))
        {
            return fail("Can't encode params");
        }
        if (size != expectedSize(in, mask))
        {
            std::cout << "Wrong size of encoded data: " << size << std::endl;
            return false;
        }

        // Header.
        if (data[0] != 0x02 || data[1] != OBJECT_DETECTOR_MAJOR_VERSION ||
            data[2] != OBJECT_DETECTOR_MINOR_VERSION)
        {
            return fail("Wrong header of encoded data");
        }

        // Decode data into an object filled with other values.
        Params out;
        randomizeParams(out, 3);
        const std::vector<uint8_t> encoded(data, data + size);
        if (!decodeExact(out, encoded))
        {
            return fail("Can't decode data");
        }
        errors += compareParams(expectedAfterDecode(in, mask), out);

        // The decoded params can be encoded and decoded again.
        Params again;
        const std::vector<uint8_t> second = encodeToVector(out, nullptr);
        if (second.empty() || !decodeExact(again, second))
        {
            return fail("Can't encode and decode the decoded params");
        }
        errors += compareParams(out, again);
        if (errors != 0)
        {
            return false;
        }
    }

    return true;
}



// Layout of the encoded data.
bool layoutTest()
{
    // Without mask all four mask bytes are 0xFF, all fields are encoded.
    Params p = makeFixedParams();
    std::vector<uint8_t> all = encodeToVector(p, nullptr);
    if (all.size() < static_cast<size_t>(HEADER_SIZE))
    {
        return fail("Can't encode params");
    }
    for (size_t i = 0; i < 4; ++i)
    {
        if (all[MASK_POS + i] != 0xFF)
        {
            return fail("Mask without mask pointer is not 0xFF");
        }
    }

    // The mask with all flags selects the same fields.
    const Mask full;
    std::vector<uint8_t> selected = encodeToVector(p, &full);
    if (selected.size() != all.size())
    {
        return fail("Different size for the mask with all flags");
    }
    if (std::memcmp(&selected[HEADER_SIZE], &all[HEADER_SIZE],
                    all.size() - HEADER_SIZE) != 0)
    {
        return fail("Different data for the mask with all flags");
    }

    // Every field has its own bit: the first field of a mask byte has the
    // most significant bit.
    for (size_t i = 0; i < WIRE_FLAGS.size(); ++i)
    {
        Mask one = emptyMask();
        one.*(WIRE_FLAGS[i]) = true;
        std::vector<uint8_t> data = encodeToVector(p, &one);
        if (data.size() < static_cast<size_t>(HEADER_SIZE))
        {
            return fail("Can't encode params with one field");
        }
        for (size_t b = 0; b < 4; ++b)
        {
            uint8_t expected = 0;
            if (b == i / 8)
            {
                expected = static_cast<uint8_t>(0x80U >> (i % 8));
            }
            if (data[static_cast<size_t>(MASK_POS) + b] != expected)
            {
                std::cout << "Wrong mask byte " << b << " for field " << i
                          << std::endl;
                return false;
            }
        }
    }

    // Known encoding: only logMode is selected. The values are copied with
    // memcpy, the byte order is the byte order of the host.
    Params known;
    known.logMode = 0x01020304;
    Mask logModeOnly = emptyMask();
    logModeOnly.logMode = true;
    const std::vector<uint8_t> data = encodeToVector(known, &logModeOnly);
    std::vector<uint8_t> expected = {
        0x02, static_cast<uint8_t>(OBJECT_DETECTOR_MAJOR_VERSION),
        static_cast<uint8_t>(OBJECT_DETECTOR_MINOR_VERSION),
        0x80, 0x00, 0x00, 0x00, 0, 0, 0, 0};
    std::memcpy(&expected[HEADER_SIZE], &known.logMode, sizeof(int));
    if (data != expected)
    {
        return fail("Wrong encoding of the known params");
    }

    return true;
}



// Check that the guard bytes behind the buffer are not changed.
bool isGuardIntact(const std::vector<uint8_t>& guarded, int bufferSize)
{
    for (size_t i = static_cast<size_t>(bufferSize); i < guarded.size(); ++i)
    {
        if (guarded[i] != GUARD_VALUE)
        {
            return false;
        }
    }

    return true;
}



// Encode into a buffer of the given size: the result must be TRUE only if the
// buffer holds all data, nothing must be written behind the buffer.
bool checkEncodeIntoBuffer(Params& p, int bufferSize, int need)
{
    std::vector<uint8_t> guarded(
                static_cast<size_t>(bufferSize) + GUARD_SIZE, GUARD_VALUE);
    int size = 123;
    const bool ok = p.encode(guarded.data(), bufferSize, size, nullptr);
    if (ok != (bufferSize >= need))
    {
        std::cout << "Wrong result for buffer size " << bufferSize
                  << std::endl;
        return false;
    }
    if (ok && size != need)
    {
        return fail("Wrong size of encoded data");
    }
    if (!ok && size != 0)
    {
        return fail("Size is not zero after a rejected call");
    }
    if (!isGuardIntact(guarded, bufferSize))
    {
        std::cout << "Write behind the buffer of " << bufferSize << " bytes"
                  << std::endl;
        return false;
    }

    return true;
}



// Buffer boundaries test.
bool bufferTest()
{
    Params p = makeFixedParams();
    p.objects.clear();
    const Mask mask;
    const int need = expectedSize(p, mask);

    // Null pointer and negative sizes.
    int size = 123;
    if (p.encode(nullptr, BUFFER_SIZE, size, nullptr) || size != 0)
    {
        return fail("nullptr data is not rejected");
    }
    std::vector<uint8_t> buffer(static_cast<size_t>(need) + GUARD_SIZE);
    for (const int bufferSize : {-1, 0, 1, HEADER_SIZE - 1,
                                 std::numeric_limits<int>::min()})
    {
        size = 123;
        if (p.encode(buffer.data(), bufferSize, size, nullptr) || size != 0)
        {
            return fail("Small buffer is not rejected");
        }
    }

    // Exact size, one byte less and a few bytes more.
    for (int bufferSize = 0; bufferSize <= need + 3; ++bufferSize)
    {
        if (!checkEncodeIntoBuffer(p, bufferSize, need))
        {
            return false;
        }
    }

    return true;
}



// Encode into a buffer which holds only some of the objects: the objects
// which do not fit are not encoded, the rest of the data is complete.
bool checkTruncatedObjects(Params& withObjects, int bufferSize,
                           int withoutObjects, size_t fit)
{
    const Mask mask;
    std::vector<uint8_t> guarded(
                static_cast<size_t>(bufferSize) + GUARD_SIZE, GUARD_VALUE);
    int size = 0;
    if (!withObjects.encode(guarded.data(), bufferSize, size, nullptr))
    {
        return fail("Can't encode params into the buffer");
    }
    const size_t objectsNumber = std::min(fit, withObjects.objects.size());
    if (size != withoutObjects + static_cast<int>(objectsNumber) * OBJECT_SIZE)
    {
        return fail("Wrong size of data with truncated objects");
    }
    if (!isGuardIntact(guarded, bufferSize))
    {
        return fail("Write behind the buffer");
    }
    Params out;
    const std::vector<uint8_t> exact(guarded.begin(), guarded.begin() + size);
    if (!decodeExact(out, exact))
    {
        return fail("Can't decode data with truncated objects");
    }
    Params expected = withObjects;
    expected.objects.resize(objectsNumber);

    return compareParams(expectedAfterDecode(expected, mask), out) == 0;
}



// Truncation of the objects by a small buffer test.
bool objectsTruncationTest()
{
    const Mask mask;
    Params withObjects = makeFixedParams();
    for (int i = 0; i < 7; ++i)
    {
        det::Object obj;
        obj.id = 1000 + i;
        withObjects.objects.push_back(obj);
    }
    const int withoutObjects = expectedSize(withObjects, mask) -
            static_cast<int>(withObjects.objects.size()) * OBJECT_SIZE;
    for (int fit = 0; fit <= 12; ++fit)
    {
        for (const int extra : {0, OBJECT_SIZE - 1})
        {
            const int bufferSize = withoutObjects + fit * OBJECT_SIZE + extra;
            if (!checkTruncatedObjects(withObjects, bufferSize, withoutObjects,
                                       static_cast<size_t>(fit)))
            {
                return false;
            }
        }
    }

    return true;
}



// Invalid class names and strings test.
bool encodeInvalidStringsTest()
{
    uint8_t data[BUFFER_SIZE];
    int size = 0;

    // Null character in the init string.
    Params p;
    p.initString = std::string("ab\0cd", 5);
    if (p.encode(data, BUFFER_SIZE, size))
    {
        return fail("Null character in the init string is not rejected");
    }
    Mask noInit;
    noInit.initString = false;
    if (!p.encode(data, BUFFER_SIZE, size, &noInit))
    {
        return fail("Not selected init string must not be checked");
    }

    // Null character in a class name, empty name which is not the last one.
    Params names;
    names.classNames = {"a", std::string("b\0c", 3)};
    if (names.encode(data, BUFFER_SIZE, size) || size != 0)
    {
        return fail("Null character in a class name is not rejected");
    }
    names.classNames = {"a", "", "b"};
    if (names.encode(data, BUFFER_SIZE, size) || size != 0)
    {
        return fail("Empty class name inside the list is not rejected");
    }

    // The last empty class name is allowed and is not restored: this is the
    // default list.
    for (const std::vector<std::string>& list :
         {std::vector<std::string>{""}, std::vector<std::string>{"a", ""}})
    {
        Params in;
        in.classNames = list;
        if (!in.encode(data, BUFFER_SIZE, size))
        {
            return fail("Can't encode the list with the last empty name");
        }
        Params out;
        if (!out.decode(data, size))
        {
            return fail("Can't decode the list with the last empty name");
        }
        if (out.classNames.size() + 1 != list.size())
        {
            return fail("Wrong class names after the last empty name");
        }
    }

    // Empty list.
    Params empty;
    empty.classNames.clear();
    if (!empty.encode(data, BUFFER_SIZE, size))
    {
        return fail("Can't encode the empty list");
    }
    Params out;
    if (!out.decode(data, size) || !out.classNames.empty())
    {
        return fail("Can't decode the empty list");
    }

    return true;
}



// Checks that invalid data is rejected and that the object is not changed by a
// rejected call.
class RejectChecker
{
public:
    RejectChecker()
    {
        randomizeParams(m_snapshot, 3);
    }

    /// Params which are used as the object for the decoding.
    const Params& snapshot() const
    {
        return m_snapshot;
    }

    /// Number of the failed checks.
    int errors() const
    {
        return m_errors;
    }

    /// The data must be rejected and the object must not be changed.
    void rejected(const std::vector<uint8_t>& data, const char* what)
    {
        Params out = m_snapshot;
        if (decodeExact(out, data))
        {
            std::cout << "Invalid data is accepted: " << what << std::endl;
            ++m_errors;
            return;
        }
        if (compareParams(m_snapshot, out) != 0)
        {
            std::cout << "Object is changed by rejected data: " << what
                      << std::endl;
            ++m_errors;
        }
    }

    /// The failed check.
    void failed()
    {
        ++m_errors;
    }

private:
    /// Params which are used as the object for the decoding.
    Params m_snapshot;
    /// Number of the failed checks.
    int m_errors{0};
};



// Null pointer and wrong sizes.
void invalidSizesTest(RejectChecker& checker)
{
    Params out = checker.snapshot();
    if (out.decode(nullptr, 100) || compareParams(checker.snapshot(), out) != 0)
    {
        std::cout << "nullptr data is not rejected" << std::endl;
        checker.failed();
    }
    uint8_t byte = 0;
    for (const int size : {-1, 0, 1, HEADER_SIZE - 1,
                           std::numeric_limits<int>::min()})
    {
        out = checker.snapshot();
        if (out.decode(&byte, size) || compareParams(checker.snapshot(), out) != 0)
        {
            std::cout << "Wrong size of data is not rejected" << std::endl;
            checker.failed();
        }
    }
}



// Every truncation length. The data is valid only at the end of a field which
// can be the last one: the end of all data, the end of the init string (no
// class names) and the end of every class name.
void truncationTest(RejectChecker& checker, const Params& p,
                    const std::vector<uint8_t>& valid)
{
    std::vector<size_t> validLengths{valid.size()};
    size_t end = valid.size();
    for (size_t i = 0; i < p.classNames.size(); ++i)
    {
        end -= p.classNames[p.classNames.size() - 1 - i].size() + 1;
        validLengths.push_back(end);
    }
    for (size_t length = 0; length < valid.size(); ++length)
    {
        const std::vector<uint8_t> cut(
                    valid.begin(),
                    valid.begin() + static_cast<std::ptrdiff_t>(length));
        const bool mustBeValid = std::find(validLengths.begin(),
                                           validLengths.end(), length) !=
                validLengths.end();
        Params result;
        if (!mustBeValid)
        {
            checker.rejected(cut, "truncated data");
        }
        else if (!decodeExact(result, cut))
        {
            std::cout << "Valid length is rejected: " << length << std::endl;
            checker.failed();
        }
    }
}



// Every byte of the header (data type and version) and bytes after the last
// field.
void headerAndTrailingTest(RejectChecker& checker, const Params& p,
                           const std::vector<uint8_t>& valid)
{
    for (size_t pos = 0; pos < 3; ++pos)
    {
        for (int value = 0; value < 256; ++value)
        {
            if (value == valid[pos])
            {
                continue;
            }
            std::vector<uint8_t> changed(valid);
            changed[pos] = static_cast<uint8_t>(value);
            checker.rejected(changed, "wrong header");
        }
    }

    // One zero byte is the empty class name which terminates the list: it is
    // valid data.
    for (const int value : {0, 1, 0x7F, 0xFF})
    {
        for (size_t extra = 1; extra <= 9; ++extra)
        {
            std::vector<uint8_t> longer(valid);
            longer.insert(longer.end(), extra, static_cast<uint8_t>(value));
            Params result;
            if (value != 0 || extra != 1)
            {
                checker.rejected(longer, "trailing bytes");
            }
            else if (!decodeExact(result, longer) ||
                     result.classNames != p.classNames)
            {
                std::cout << "The empty class name is not accepted"
                          << std::endl;
                checker.failed();
            }
        }
    }
}



// Number of objects: negative, larger than the data, one more than the data
// holds.
void objectsCounterTest(RejectChecker& checker,
                        const std::vector<uint8_t>& valid)
{
    const size_t countPos = OBJECTS_COUNTER_POS;
    const size_t rest = valid.size() - countPos - VALUE_SIZE;
    for (const int count : {-1, std::numeric_limits<int>::min(),
                            std::numeric_limits<int>::max(), 1 << 28,
                            static_cast<int>(rest / OBJECT_SIZE) + 1})
    {
        std::vector<uint8_t> changed(valid);
        std::memcpy(&changed[countPos], &count, sizeof(int));
        checker.rejected(changed, "wrong number of objects");
    }
}



// Non canonical bool: any non zero byte is TRUE.
bool boolFieldTest(const std::vector<uint8_t>& valid)
{
    for (const int value : {1, 2, 0x7F, 0x80, 0xFF})
    {
        std::vector<uint8_t> changed(valid);
        changed[ENABLE_POS] = static_cast<uint8_t>(value);
        Params result;
        if (!decodeExact(result, changed) || !result.enable)
        {
            return fail("Non zero byte is not TRUE");
        }
    }
    std::vector<uint8_t> falseData(valid);
    falseData[ENABLE_POS] = 0;
    Params falseResult;
    falseResult.enable = true;
    if (!decodeExact(falseResult, falseData) || falseResult.enable)
    {
        return fail("Zero byte is not FALSE");
    }

    return true;
}



// The init string without the terminator, the empty class name followed by
// data.
void stringsTest(RejectChecker& checker, const Params& p,
                 const std::vector<uint8_t>& valid)
{
    const size_t initPos = OBJECTS_COUNTER_POS + VALUE_SIZE +
            p.objects.size() * OBJECT_SIZE;
    std::vector<uint8_t> noTerminator(valid);
    noTerminator[initPos + p.initString.size()] = 'x';
    noTerminator.resize(initPos + p.initString.size() + 1);
    checker.rejected(noTerminator, "init string without terminator");
    std::vector<uint8_t> afterEmpty(
                valid.begin(), valid.begin() + static_cast<std::ptrdiff_t>(initPos));
    afterEmpty.insert(afterEmpty.end(), {'i', 0, 0, 'a', 0});
    checker.rejected(afterEmpty, "data after the empty class name");
}



// Invalid data test.
bool decodeInvalidDataTest()
{
    const Params p = makeFixedParams();
    Params source = p;
    const std::vector<uint8_t> valid = encodeToVector(source, nullptr);
    if (valid.size() != static_cast<size_t>(expectedSize(p, Mask())))
    {
        return fail("Unexpected layout of the test data");
    }
    int objectsInData = 0;
    std::memcpy(&objectsInData, &valid[OBJECTS_COUNTER_POS], sizeof(int));
    if (objectsInData != static_cast<int>(p.objects.size()))
    {
        return fail("Unexpected number of objects in the test data");
    }

    RejectChecker checker;
    invalidSizesTest(checker);
    truncationTest(checker, p, valid);
    headerAndTrailingTest(checker, p, valid);
    objectsCounterTest(checker, valid);
    stringsTest(checker, p, valid);

    return boolFieldTest(valid) && checker.errors() == 0;
}



// Damage the data: random replacement of 0.1 %, 0.5 % or 2 % of the bytes,
// truncation at 10 %, 50 % or 90 % or a duplicated piece.
void damage(std::vector<uint8_t>& data)
{
    const int kind = randomInt(5);
    if (kind < 3)
    {
        const int permille[3] = {1, 5, 20};
        const int limit = permille[randomInt(3)];
        for (uint8_t& byte : data)
        {
            if (randomInt(1000) < limit)
            {
                byte = static_cast<uint8_t>(randomInt(256));
            }
        }
        const size_t pos = static_cast<size_t>(
                    randomInt(static_cast<int>(data.size())));
        data[pos] = static_cast<uint8_t>(randomInt(256));
    }
    else if (kind == 3)
    {
        const size_t percents[3] = {10, 50, 90};
        data.resize(data.size() * percents[randomInt(3)] / 100);
    }
    else
    {
        const size_t from = static_cast<size_t>(
                    randomInt(static_cast<int>(data.size())));
        const size_t count = static_cast<size_t>(
                    randomInt(static_cast<int>(data.size() - from))) + 1;
        data.insert(data.end(),
                    data.begin() + static_cast<std::ptrdiff_t>(from),
                    data.begin() + static_cast<std::ptrdiff_t>(from + count));
    }
}



// Decode the damaged data: a rejected call must not change the object, an
// accepted data must give params which can be encoded and decoded again.
// Returns 0 if the data is rejected, 1 if the data is accepted and -1 if a
// check failed.
int checkDamaged(const std::vector<uint8_t>& data)
{
    Params snapshot;
    randomizeParams(snapshot, 2);
    Params out = snapshot;
    if (!decodeExact(out, data))
    {
        if (compareParams(snapshot, out) != 0)
        {
            std::cout << "Object is changed by rejected data" << std::endl;
            return -1;
        }
        return 0;
    }
    Params again;
    const std::vector<uint8_t> encoded = encodeToVector(out, nullptr);
    if (encoded.empty() || !decodeExact(again, encoded) ||
        compareParams(out, again) != 0)
    {
        std::cout << "Accepted data gives params which can not be encoded "
                     "and decoded again" << std::endl;
        return -1;
    }

    return 1;
}



// Damaged data campaign: the result must be deterministic.
bool damagedDataTest()
{
    // Samples of valid data.
    std::vector<std::vector<uint8_t>> samples;
    for (int i = 0; i < 12; ++i)
    {
        Params p;
        randomizeParams(p, 4);
        const Mask mask = (i == 0) ? Mask() : randomMask();
        const std::vector<uint8_t> data = encodeToVector(p, &mask);
        if (data.empty())
        {
            return fail("Can't encode params");
        }
        samples.push_back(data);
    }
    Params fixedParams = makeFixedParams();
    samples.push_back(encodeToVector(fixedParams, nullptr));

    // Two identical runs: the number of rejected and of accepted data.
    uint64_t counters[2][2] = {{0, 0}, {0, 0}};
    for (int run = 0; run < 2; ++run)
    {
        g_randomState = 12345U;
        for (size_t i = 0; i < 40000; ++i)
        {
            std::vector<uint8_t> data = samples[i % samples.size()];
            damage(data);
            const int result = checkDamaged(data);
            if (result < 0)
            {
                return false;
            }
            ++counters[run][result];
        }
    }
    std::cout << "Rejected: " << counters[0][0] << ", accepted: "
              << counters[0][1] << std::endl;

    return counters[0][0] == counters[1][0] && counters[0][1] == counters[1][1]
            && counters[0][0] > 0 && counters[0][1] > 0;
}



// Action commands: encode and decode of every command.
bool actionCommandsTest()
{
    for (int id = 1; id <= LAST_COMMAND_ID; ++id)
    {
        uint8_t data[ACTION_COMMAND_SIZE + GUARD_SIZE];
        std::memset(data, GUARD_VALUE, sizeof(data));
        int size = 0;
        det::ObjectDetector::encodeCommand(
                    data, size, static_cast<Command>(id));
        if (size != ACTION_COMMAND_SIZE)
        {
            return fail("Wrong size of action command");
        }
        for (size_t i = ACTION_COMMAND_SIZE; i < sizeof(data); ++i)
        {
            if (data[i] != GUARD_VALUE)
            {
                return fail("Write behind the action command");
            }
        }
        Command commandId = Command::RESET;
        Param paramId = Param::LOG_MODE;
        float value = 1.0f;
        if (det::ObjectDetector::decodeCommand(
                    data, size, paramId, commandId, value) != 0)
        {
            return fail("Action command is not decoded");
        }
        if (static_cast<int>(commandId) != id || bitsOf(value) != 0U ||
            paramId != Param::LOG_MODE)
        {
            return fail("Wrong decoded action command");
        }
    }

    return true;
}



// Set param commands with any value (NaN and infinity are transported).
bool setParamCommandsTest()
{
    for (int id = 1; id <= LAST_PARAM_ID; ++id)
    {
        for (int k = 0; k < 20; ++k)
        {
            const float outValue = fromBits<float>(randomBits());
            uint8_t data[SET_PARAM_COMMAND_SIZE + GUARD_SIZE];
            std::memset(data, GUARD_VALUE, sizeof(data));
            int size = 0;
            det::ObjectDetector::encodeSetParamCommand(
                        data, size, static_cast<Param>(id), outValue);
            if (size != SET_PARAM_COMMAND_SIZE)
            {
                return fail("Wrong size of set param command");
            }
            for (size_t i = SET_PARAM_COMMAND_SIZE; i < sizeof(data); ++i)
            {
                if (data[i] != GUARD_VALUE)
                {
                    return fail("Write behind the set param command");
                }
            }
            Command commandId = Command::RESET;
            Param paramId = Param::LOG_MODE;
            float value = 0.0f;
            if (det::ObjectDetector::decodeCommand(
                        data, size, paramId, commandId, value) != 1)
            {
                return fail("Set param command is not decoded");
            }
            if (static_cast<int>(paramId) != id ||
                bitsOf(value) != bitsOf(outValue) ||
                commandId != Command::RESET)
            {
                return fail("Wrong decoded set param command");
            }
        }
    }

    return true;
}



// Get invalid commands: every size except the exact size of the command, wrong
// version, wrong data type and unknown IDs.
std::vector<std::vector<uint8_t>> makeInvalidCommands()
{
    uint8_t action[ACTION_COMMAND_SIZE];
    uint8_t setParam[SET_PARAM_COMMAND_SIZE];
    int size = 0;
    det::ObjectDetector::encodeCommand(action, size, Command::OFF);
    det::ObjectDetector::encodeSetParamCommand(
                setParam, size, Param::SENSITIVITY, 0.5f);
    const std::vector<uint8_t> validAction(action, action + sizeof(action));
    const std::vector<uint8_t> validSetParam(setParam,
                                             setParam + sizeof(setParam));
    std::vector<std::vector<uint8_t>> invalid;

    for (size_t length = 0; length <= 20; ++length)
    {
        for (const std::vector<uint8_t>* valid : {&validAction, &validSetParam})
        {
            if (length != valid->size())
            {
                std::vector<uint8_t> resized(*valid);
                resized.resize(length, GUARD_VALUE);
                invalid.push_back(resized);
            }
        }
    }
    for (const std::vector<uint8_t>* valid : {&validAction, &validSetParam})
    {
        for (size_t pos = 0; pos < 3; ++pos)
        {
            for (int value = 0; value < 256; ++value)
            {
                // The data types 0 and 1 are valid commands.
                const bool validValue = pos == 0 ? value <= 1 :
                                                   value == (*valid)[pos];
                if (!validValue)
                {
                    std::vector<uint8_t> changed(*valid);
                    changed[pos] = static_cast<uint8_t>(value);
                    invalid.push_back(changed);
                }
            }
        }
    }
    for (const int id : {0, LAST_COMMAND_ID + 1, -1,
                         std::numeric_limits<int>::min(),
                         std::numeric_limits<int>::max()})
    {
        std::vector<uint8_t> changed(validAction);
        std::memcpy(&changed[COMMAND_ID_POS], &id, sizeof(int));
        invalid.push_back(changed);
    }
    for (const int id : {0, LAST_PARAM_ID + 1, -1,
                         std::numeric_limits<int>::min(),
                         std::numeric_limits<int>::max()})
    {
        std::vector<uint8_t> changed(validSetParam);
        std::memcpy(&changed[COMMAND_ID_POS], &id, sizeof(int));
        invalid.push_back(changed);
    }

    return invalid;
}



// Invalid commands: the result is -1 and the output arguments are not changed.
bool invalidCommandsTest()
{
    for (const std::vector<uint8_t>& data : makeInvalidCommands())
    {
        std::vector<uint8_t> exact(data);
        uint8_t dummy = 0;
        Command commandId = Command::ON;
        Param paramId = Param::TYPE;
        float value = 7.0f;
        const int result = det::ObjectDetector::decodeCommand(
                    exact.empty() ? &dummy : exact.data(),
                    static_cast<int>(exact.size()), paramId, commandId, value);
        if (result != -1 || commandId != Command::ON ||
            paramId != Param::TYPE || bitsOf(value) != bitsOf(7.0f))
        {
            return fail("Invalid command is not rejected");
        }
    }
    Command commandId = Command::ON;
    Param paramId = Param::TYPE;
    float value = 7.0f;
    if (det::ObjectDetector::decodeCommand(
                nullptr, SET_PARAM_COMMAND_SIZE, paramId, commandId, value) != -1)
    {
        return fail("nullptr command is not rejected");
    }

    return true;
}



// nullptr is not written: the size is set to zero.
bool nullCommandsTest()
{
    int size = 123;
    det::ObjectDetector::encodeCommand(nullptr, size, Command::ON);
    if (size != 0)
    {
        return fail("Size is not zero for nullptr (action command)");
    }
    size = 123;
    det::ObjectDetector::encodeSetParamCommand(
                nullptr, size, Param::TYPE, 1.0f);
    if (size != 0)
    {
        return fail("Size is not zero for nullptr (set param command)");
    }

    return true;
}



// Encode/decode commands test.
bool encodeDecodeCommandsTest()
{
    return actionCommandsTest() && setParamCommandsTest() &&
            nullCommandsTest() && invalidCommandsTest();
}



// Version test.
bool versionTest()
{
    const std::string expected = std::to_string(OBJECT_DETECTOR_MAJOR_VERSION) +
            "." + std::to_string(OBJECT_DETECTOR_MINOR_VERSION) + "." +
            std::to_string(OBJECT_DETECTOR_PATCH_VERSION);
    std::cout << "Version: " << det::ObjectDetector::getVersion() << std::endl;

    return det::ObjectDetector::getVersion() == expected &&
            expected == OBJECT_DETECTOR_VERSION;
}



// JSON read/write test.
bool jsonReadWriteTest()
{
    // Prepare random params. The fields which are not in JSON (processing
    // time and objects) are not compared.
    Params in;
    randomizeParams(in, 5);
    in.classNames = {"apple", "banana", "orange", "pineapple", "strawberry",
                     "watermelon", "lemon", "peach", "pear", "plum"};
    in.initString = "sfsfsfsfsf";
    in.processingTimeMcs = 0;
    in.objects.clear();
    for (const FloatField& field : FLOAT_FIELDS)
    {
        in.*(field.value) = static_cast<float>(randomInt(255));
    }
    in.enable = false;

    // Write params to string and read them back.
    rapidpixel::ConfigReader inConfig;
    if (!inConfig.set(in, "ObjectDetectorParams"))
    {
        return fail("Can't set params");
    }
    std::string text;
    if (!inConfig.writeToString(text))
    {
        return fail("Can't write params to string");
    }
    rapidpixel::ConfigReader textConfig;
    Params fromText;
    if (!textConfig.readFromString(text) ||
        !textConfig.get(fromText, "ObjectDetectorParams"))
    {
        return fail("Can't read params from string");
    }
    if (compareParams(in, fromText) != 0)
    {
        return false;
    }

    // Write params to file and read them back. The file is removed.
    const std::string fileName = "ObjectDetectorParamsTest.json";
    FileRemover remover(fileName);
    if (!inConfig.writeToFile(fileName))
    {
        return fail("Can't write params to file");
    }
    rapidpixel::ConfigReader outConfig;
    if (!outConfig.readFromFile(fileName))
    {
        return fail("Can't open config file");
    }
    Params out;
    if (!outConfig.get(out, "ObjectDetectorParams"))
    {
        return fail("Can't read params from file");
    }
    if (compareParams(in, out) != 0)
    {
        return false;
    }
    if (!remover.remove())
    {
        return false;
    }

    // Invalid documents are rejected and do not change the object: wrong
    // syntax, no params, missing fields, wrong types of the fields.
    Params snapshot;
    randomizeParams(snapshot, 2);
    const std::vector<std::string> documents = {
        "", "{", "[]", "null", "{\"ObjectDetectorParams\": 5}",
        "{\"ObjectDetectorParams\": {}}",
        "{\"ObjectDetectorParams\": {\"logMode\": 1}}"
    };
    for (const std::string& document : documents)
    {
        rapidpixel::ConfigReader config;
        Params result = snapshot;
        if (config.readFromString(document) &&
            config.get(result, "ObjectDetectorParams"))
        {
            return fail("Invalid JSON is accepted");
        }
        if (compareParams(snapshot, result) != 0)
        {
            return fail("Object is changed by invalid JSON");
        }
    }
    const std::vector<std::pair<std::string, std::string>> replacements = {
        {"\"logMode\":", "\"logMode\":\"text\",\"x\":"},
        {"\"classNames\":[", "\"classNames\":[1,"},
        {"\"enable\":false", "\"enable\":\"no\""},
        {"\"initString\":\"sfsfsfsfsf\"", "\"initString\":5"}
    };
    for (const auto& replacement : replacements)
    {
        std::string changed = text;
        const size_t pos = changed.find(replacement.first);
        if (pos == std::string::npos)
        {
            return fail("Unexpected JSON text");
        }
        changed.replace(pos, replacement.first.size(), replacement.second);
        rapidpixel::ConfigReader config;
        Params result = snapshot;
        if (config.readFromString(changed) &&
            config.get(result, "ObjectDetectorParams"))
        {
            return fail("JSON with a wrong type is accepted");
        }
        if (compareParams(snapshot, result) != 0)
        {
            return fail("Object is changed by JSON with a wrong type");
        }
    }

    return true;
}



}



// Entry point.
int main()
{
    std::cout << "#####################################" << std::endl;
    std::cout << "#                                   #" << std::endl;
    std::cout << "# ObjectDetector test               #" << std::endl;
    std::cout << "#                                   #" << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << std::endl;

    bool allPassed = true;
    allPassed &= runTest("Copy test", copyTest);
    allPassed &= runTest("Encode/Decode test", encodeDecodeTest);
    allPassed &= runTest("Layout of encoded data test", layoutTest);
    allPassed &= runTest("Buffer boundaries test", bufferTest);
    allPassed &= runTest("Truncation of objects test", objectsTruncationTest);
    allPassed &= runTest("Invalid strings test", encodeInvalidStringsTest);
    allPassed &= runTest("Invalid data test", decodeInvalidDataTest);
    allPassed &= runTest("Damaged data test", damagedDataTest);
    allPassed &= runTest("Encode/Decode commands test",
                         encodeDecodeCommandsTest);
    allPassed &= runTest("Version test", versionTest);
    allPassed &= runTest("JSON read/write test", jsonReadWriteTest);

    return allPassed ? 0 : 1;
}
