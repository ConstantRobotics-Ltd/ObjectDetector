#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "Frame.h"
#include "ConfigReader.h"



namespace cr::detector
{
/**
 * @brief Object data structure.
 *
 * The structure only transports the values: they are not checked against any
 * range by the library.
 */
struct Object
{
    /// Object ID. Must be unique for particular object.
    int id{ 0 };
    /// Frame ID. Must be the same as frame ID of processed video frame.
    int frameId{ 0 };
    /// Object type. Depends on implementation.
    int type{ 0 };
    /// Object rectangle width, pixels.
    int width{ 0 };
    /// Object rectangle height, pixels.
    int height{ 0 };
    /// Object rectangle top-left horizontal coordinate, pixels.
    int x{ 0 };
    /// Object rectangle top-left vertical coordinate, pixels.
    int y{ 0 };
    /// Horizontal component of object velocity, +-pixels/frame.
    float vX{ 0.0f };
    /// Vertical component of object velocity, +-pixels/frame.
    float vY{ 0.0f };
    /// Detection probability from 0 (minimum) to 1 (maximum).
    float p{ 0.0f };
};



/**
 * @brief Struct representing the mask for object detector parameters.
 *
 * This struct defines a mask to control which parameters are enabled or
 * disabled for parameters serialization. Each field in the mask corresponds to
 * a specific parameter. When the flag is set to true, the parameter is
 * considered enabled for serialization. Otherwise, it is disabled.
 */
struct ObjectDetectorParamsMask
{
    /// Serialize initString field.
    bool initString{ true };
    /// Serialize logMode field.
    bool logMode{ true };
    /// Serialize frameBufferSize field.
    bool frameBufferSize{ true };
    /// Serialize minObjectWidth field.
    bool minObjectWidth{ true };
    /// Serialize maxObjectWidth field.
    bool maxObjectWidth{ true };
    /// Serialize minObjectHeight field.
    bool minObjectHeight{ true };
    /// Serialize maxObjectHeight field.
    bool maxObjectHeight{ true };
    /// Serialize minXSpeed field.
    bool minXSpeed{ true };
    /// Serialize maxXSpeed field.
    bool maxXSpeed{ true };
    /// Serialize minYSpeed field.
    bool minYSpeed{ true };
    /// Serialize maxYSpeed field.
    bool maxYSpeed{ true };
    /// Serialize minDetectionProbability field.
    bool minDetectionProbability{ true };
    /// Serialize xDetectionCriteria field.
    bool xDetectionCriteria{ true };
    /// Serialize yDetectionCriteria field.
    bool yDetectionCriteria{ true };
    /// Serialize resetCriteria field.
    bool resetCriteria{ true };
    /// Serialize sensitivity field.
    bool sensitivity{ true };
    /// Serialize scaleFactor field.
    bool scaleFactor{ true };
    /// Serialize numThreads field.
    bool numThreads{ true };
    /// Serialize processingTimeMcs field.
    bool processingTimeMcs{ true };
    /// Serialize type field.
    bool type{ true };
    /// Serialize enable field.
    bool enable{ true };
    /// Serialize custom1 field.
    bool custom1{ true };
    /// Serialize custom2 field.
    bool custom2{ true };
    /// Serialize custom3 field.
    bool custom3{ true };
    /// Serialize objects field.
    bool objects{ true };
    /// Serialize classNames field.
    bool classNames{ true };
};



/**
 * @brief Object detector params class.
 *
 * The values of the fields are transported by the class (JSON, serialization)
 * and are not checked against any range: the object detector implementation
 * must validate them before use.
 *
 * @note The class is not thread-safe: an object must not be accessed from
 * several threads at the same time without external synchronization.
 */
class ObjectDetectorParams
{
public:
    /// Init string. Depends on implementation. Must not contain the null
    /// character to be serialized by encode(...).
    std::string initString;
    /// Logging mode. Values:
    /// 0 - Disable,
    /// 1 - Only file,
    /// 2 - Only terminal (console),
    /// 3 - File and terminal (console).
    int logMode{ 0 };
    /// Frame buffer size. Depends on implementation.
    int frameBufferSize{ 1 };
    /// Minimum object width to be detected, pixels. To be detected object's
    /// width must be >= minObjectWidth.
    int minObjectWidth{ 4 };
    /// Maximum object width to be detected, pixels. To be detected object's
    /// width must be <= maxObjectWidth.
    int maxObjectWidth{ 128 };
    /// Minimum object height to be detected, pixels. To be detected object's
    /// height must be >= minObjectHeight.
    int minObjectHeight{ 4 };
    /// Maximum object height to be detected, pixels. To be detected object's
    /// height must be <= maxObjectHeight.
    int maxObjectHeight{ 128 };
    /// Minimum object's horizontal speed to be detected, pixels/frame. To be
    /// detected object's horizontal speed must be >= minXSpeed.
    float minXSpeed{ 0.0f };
    /// Maximum object's horizontal speed to be detected, pixels/frame. To be
    /// detected object's horizontal speed must be <= maxXSpeed.
    float maxXSpeed{ 30.0f };
    /// Minimum object's vertical speed to be detected, pixels/frame. To be
    /// detected object's vertical speed must be >= minYSpeed.
    float minYSpeed{ 0.0f };
    /// Maximum object's vertical speed to be detected, pixels/frame. To be
    /// detected object's vertical speed must be <= maxYSpeed.
    float maxYSpeed{ 30.0f };
    /// Probability threshold from 0 to 1. To be detected object detection
    /// probability must be >= minDetectionProbability.
    float minDetectionProbability{ 0.5f };
    /// Horizontal track detection criteria, frames. By default shows how many
    /// frames the objects must move in any(+/-) horizontal direction to be
    /// detected.
    int xDetectionCriteria{ 1 };
    /// Vertical track detection criteria, frames. By default shows how many
    /// frames the objects must move in any(+/-) vertical direction to be
    /// detected.
    int yDetectionCriteria{ 1 };
    /// Track reset criteria, frames. By default shows how many
    /// frames the objects should be not detected to be excluded from results.
    int resetCriteria{ 1 };
    /// Detection sensitivity. Depends on implementation. Default from 0 to 1.
    float sensitivity{ 0.04f };
    /// Frame scaling factor for processing purposes. Reduce the image size by
    /// scaleFactor times horizontally and vertically for faster processing.
    int scaleFactor{ 1 };
    /// Num threads. Number of threads for parallel computing.
    int numThreads{ 1 };
    /// Processing time of last frame in microseconds.
    int processingTimeMcs{ 0 };
    /// Algorithm / backend type. Depends on implementation.
    int type{ 0 };
    /// Mode. Default: false - Off, true - On.
    bool enable{ true };
    /// Custom parameter. Depends on implementation.
    float custom1{ 0.0f };
    /// Custom parameter. Depends on implementation.
    float custom2{ 0.0f };
    /// Custom parameter. Depends on implementation.
    float custom3{ 0.0f };
    /// List of detected objects.
    std::vector<Object> objects;
    /// A list of object class names used in detectors that recognize different
    /// object classes. Detected objects have an attribute called 'type.'
    /// If a detector doesn't support object class recognition or can't
    /// determine the object type, the 'type' field must be set to 0.
    /// Otherwise, the 'type' should correspond to the ordinal number of the
    /// class name from the 'classNames' list (if the list was set in params),
    /// starting from 1 (where the first element in the list has 'type == 1').
    /// The names are serialized by encode(...) as null terminated strings: a
    /// name must not contain the null character and an empty name is allowed
    /// only as the last element of the list. decode(...) returns the list
    /// without that empty name.
    std::vector<std::string> classNames{ "" };

    JSON_READABLE(ObjectDetectorParams, initString, logMode, frameBufferSize,
                  minObjectWidth, maxObjectWidth, minObjectHeight,
                  maxObjectHeight, minXSpeed, maxXSpeed, minYSpeed, maxYSpeed,
                  minDetectionProbability, xDetectionCriteria,
                  yDetectionCriteria, resetCriteria, sensitivity, scaleFactor,
                  numThreads, type, enable, custom1, custom2, custom3,
                  classNames)

    /**
     * @brief Default constructor.
     */
    ObjectDetectorParams() = default;

    /**
     * @brief Copy constructor.
     * @param src Source object.
     */
    ObjectDetectorParams(const ObjectDetectorParams& src) = default;

    /**
     * @brief Copy operator.
     * @param src Source object.
     * @return Reference to this object.
     */
    ObjectDetectorParams& operator= (const ObjectDetectorParams& src) = default;

    /**
     * @brief Encode (serialize) params.
     *
     * The encoded data contains the data type, the library version
     * (MAJOR.MINOR), the mask of the serialized fields and the values of the
     * selected fields. The size of the data with all fields selected, without
     * objects, with an empty init string and with the default list of class
     * names is 102 bytes. The method does not allocate memory and does not
     * write outside of the buffer.
     *
     * If the buffer is too small to hold all objects (40 bytes per object)
     * only the objects which fit are encoded. The fields of the object are
     * always encoded completely or not at all.
     *
     * @param data Pointer to data buffer. Must not be nullptr. The pointer
     * must be valid for bufferSize bytes (the library cannot check this).
     * @param bufferSize Size of the data buffer, bytes. The buffer must hold
     * all selected fields except objects, the object counter, the init string
     * and all class names with their null terminators.
     * @param size Size of the encoded data, bytes. Set to 0 if the method
     * returns FALSE.
     * @param mask Pointer to the parameters mask. If nullptr all fields are
     * encoded.
     * @return TRUE if params encoded or FALSE if not (data is nullptr, the
     * buffer is too small, the init string or a class name contains the null
     * character, an empty class name is not the last one). If FALSE is
     * returned the content of the buffer is not defined.
     */
    bool encode(uint8_t* data, int bufferSize, int& size,
                ObjectDetectorParamsMask* mask = nullptr);

    /**
     * @brief Decode (deserialize) params encoded by encode(...) method.
     *
     * The method checks the data type, the version (MAJOR.MINOR), the size of
     * the data before reading each field, the number of objects against the
     * size of the data and that init string and class names are null
     * terminated. The data must not have bytes after the last field. Fields
     * not selected in the encoded data are set to zero (empty for the string,
     * objects and class names). Values of the fields are not checked against
     * any range.
     *
     * @param data Pointer to encoded data. Must not be nullptr. The pointer
     * must be valid for dataSize bytes (the library cannot check this).
     * @param dataSize Size of the encoded data, bytes.
     * @return TRUE if params decoded or FALSE if not. If FALSE is returned
     * the object is not changed.
     */
    bool decode(uint8_t* data, int dataSize);
};



/**
 * @brief Enum object detector parameters.
 *
 * @note CUSTOM_3 must stay the last value: ObjectDetector::decodeCommand(...)
 * uses it to check the range of the decoded parameter ID.
 */
enum class ObjectDetectorParam
{
    /// Logging mode. Values:
    /// 0 - Disable,
    /// 1 - Only file,
    /// 2 - Only terminal (console),
    /// 3 - File and terminal (console).
    LOG_MODE = 1,
    /// Frame buffer size. Depends on implementation.
    FRAME_BUFFER_SIZE,
    /// Minimum object width to be detected, pixels. To be detected object's
    /// width must be >= MIN_OBJECT_WIDTH.
    MIN_OBJECT_WIDTH,
    /// Maximum object width to be detected, pixels. To be detected object's
    /// width must be <= MAX_OBJECT_WIDTH.
    MAX_OBJECT_WIDTH,
    /// Minimum object height to be detected, pixels. To be detected object's
    /// height must be >= MIN_OBJECT_HEIGHT.
    MIN_OBJECT_HEIGHT,
    /// Maximum object height to be detected, pixels. To be detected object's
    /// height must be <= MAX_OBJECT_HEIGHT.
    MAX_OBJECT_HEIGHT,
    /// Minimum object's horizontal speed to be detected, pixels/frame. To be
    /// detected object's horizontal speed must be >= MIN_X_SPEED.
    MIN_X_SPEED,
    /// Maximum object's horizontal speed to be detected, pixels/frame. To be
    /// detected object's horizontal speed must be <= MAX_X_SPEED.
    MAX_X_SPEED,
    /// Minimum object's vertical speed to be detected, pixels/frame. To be
    /// detected object's vertical speed must be >= MIN_Y_SPEED.
    MIN_Y_SPEED,
    /// Maximum object's vertical speed to be detected, pixels/frame. To be
    /// detected object's vertical speed must be <= MAX_Y_SPEED.
    MAX_Y_SPEED,
    /// Probability threshold from 0 to 1. To be detected object detection
    /// probability must be >= MIN_DETECTION_PROBABILITY.
    MIN_DETECTION_PROBABILITY,
    /// Horizontal track detection criteria, frames. By default shows how many
    /// frames the objects must move in any(+/-) horizontal direction to be
    /// detected.
    X_DETECTION_CRITERIA,
    /// Vertical track detection criteria, frames. By default shows how many
    /// frames the objects must move in any(+/-) vertical direction to be
    /// detected.
    Y_DETECTION_CRITERIA,
    /// Track reset criteria, frames. By default shows how many
    /// frames the objects should be not detected to be excluded from results.
    RESET_CRITERIA,
    /// Detection sensitivity. Depends on implementation. Default from 0 to 1.
    SENSITIVITY,
    /// Frame scaling factor for processing purposes. Reduce the image size by
    /// scaleFactor times horizontally and vertically for faster processing.
    SCALE_FACTOR,
    /// Num threads. Number of threads for parallel computing.
    NUM_THREADS,
    /// Processing time of last frame in microseconds.
    PROCESSING_TIME_MCS,
    /// Algorithm / backend type. Depends on implementation.
    TYPE,
    /// Mode. Default: 0 - Off, 1 - On.
    MODE,
    /// Custom parameter. Depends on implementation.
    CUSTOM_1,
    /// Custom parameter. Depends on implementation.
    CUSTOM_2,
    /// Custom parameter. Depends on implementation.
    CUSTOM_3
};



/**
 * @brief Object detector commands.
 *
 * @note OFF must stay the last value: ObjectDetector::decodeCommand(...) uses
 * it to check the range of the decoded command ID.
 */
enum class ObjectDetectorCommand
{
    /// Reset.
    RESET = 1,
    /// Enable.
    ON,
    /// Disable.
    OFF
};



/**
 * @brief Object detector interface class.
 */
class ObjectDetector
{
public:

    /**
     * @brief Class destructor.
     */
    virtual ~ObjectDetector();

    /**
     * @brief Get string of current library version.
     * @return String of current library version: "Major.Minor.Patch".
     */
    static std::string getVersion();

    /**
     * @brief Init object detector. The particular detector should initialize
     * only supported parameters from ObjectDetectorParams class.
     *
     * The values of the params are not checked by the interface: the
     * implementation must validate them.
     *
     * @param params Parameters class.
     * @return TRUE if the object detector is init or FALSE if not.
     */
    virtual bool initObjectDetector(ObjectDetectorParams& params) = 0;

    /**
     * @brief Set object detector param.
     * @note The implementation must be thread-safe.
     * @param id Param ID.
     * @param value Param value to set. The implementation must check that
     * the value is valid (the value can be NaN, infinity or out of the range
     * of the parameter) before it is converted or used.
     * @return TRUE if param was set or FALSE if not (invalid value or
     * particular param not supported).
     */
    virtual bool setParam(ObjectDetectorParam id, float value) = 0;

    /**
     * @brief Get object detector param value.
     * @note The implementation must be thread-safe.
     * @param id Param ID.
     * @return Param value or -1 (if particular param not supported).
     */
    virtual float getParam(ObjectDetectorParam id) = 0;

    /**
     * @brief Get object detector params structure.
     * @note The implementation must be thread-safe.
     * @param params Object detector params object. If particular object
     * detector doesn't support some params this params must have default
     * values.
     */
    virtual void getParams(ObjectDetectorParams& params) = 0;

    /**
     * @brief Get list of detected objects. If object detector disabled the
     * list of detected objects always must be empty.
     * @note The implementation must be thread-safe.
     * @return List of objects. If no detected object the list will be empty.
     */
    virtual std::vector<Object> getObjects() = 0;

    /**
     * @brief Execute command.
     * @note The implementation must be thread-safe.
     * @param id Command ID.
     * @return TRUE if the command accepted (executed) or FALSE if not.
     */
    virtual bool executeCommand(ObjectDetectorCommand id) = 0;

    /**
     * @brief Perform detection.
     * @param frame Source video frame. Required pixel format depends on
     * implementation. It is recommended to support all possible RAW pixel
     * formats supported by Frame object.
     * @return TRUE if video frame was processed or FALSE if not.
     */
    virtual bool detect(cr::video::Frame& frame) = 0;

    /**
     * @brief Set detection mask. Detector omits image segments, where
     * detection mask pixel values equal 0.
     * @param mask Detection binary mask. Frame object. The object detector
     * must support pixel formats for mask: GRAY, NV12, NV21, YV12 and YU12.
     * @return TRUE if video detection mask was set or FALSE if not.
     */
    virtual bool setMask(cr::video::Frame mask) = 0;

    /**
     * @brief Encode set param command.
     *
     * The param ID and the value are transported without any check:
     * decodeCommand(...) rejects an ID which is not a member of
     * ObjectDetectorParam enum.
     *
     * @param data Pointer to data buffer. Must have size >= 11. If nullptr
     * the size is set to 0 and nothing is written.
     * @param size Size of encoded data (size of command): 11 bytes.
     * @param id Param id.
     * @param value Param value.
     */
    static void encodeSetParamCommand(
            uint8_t* data, int& size, ObjectDetectorParam id, float value);

    /**
     * @brief Encode action command.
     *
     * The command ID is transported without any check: decodeCommand(...)
     * rejects an ID which is not a member of ObjectDetectorCommand enum.
     *
     * @param data Pointer to data buffer. Must have size >= 7. If nullptr
     * the size is set to 0 and nothing is written.
     * @param size Size of encoded data (size of command): 7 bytes.
     * @param id Command ID.
     */
    static void encodeCommand(
            uint8_t* data, int& size, ObjectDetectorCommand id);

    /**
     * @brief Decode command. Method decodes commands which prepared with
     * encodeSetParamCommand(...) and encodeCommand(...) methods.
     *
     * The method checks the pointer, the version (MAJOR.MINOR), the exact
     * size of the command (7 bytes for an action command, 11 bytes for a set
     * param command) and the range of the decoded ID. The output arguments
     * are not changed if the method returns -1.
     *
     * @param data Pointer to command data. Must not be nullptr. The pointer
     * must be valid for size bytes (the library cannot check this).
     * @param size Size of data.
     * @param paramId Output (decoded) parameter ID.
     * @param commandId Output (decoded) command ID.
     * @param value Param value if decoded, 0 for an action command.
     * @return Return code:
     * 0  - action command decoded, commandId will be initialized.
     * 1  - set param command decoded, paramId and value will be initialized.
     * -1 - error (no commands decoded).
     */
    static int decodeCommand(uint8_t* data, int size,
                             ObjectDetectorParam& paramId,
                             ObjectDetectorCommand& commandId,
                             float& value);

    /**
     * @brief Decode and execute command. Method decodes commands which
     * prepared with encodeSetParamCommand(...) and encodeCommand(...)
     * methods and call setParam(...) or executeCommand(...) methods.
     * @note The implementation must be thread-safe.
     * @param data Pointer to command data.
     * @param size Size of data.
     * @return TRUE if command decoded and executed or FALSE if not.
     */
    virtual bool decodeAndExecuteCommand(uint8_t* data, int size) = 0;

protected:

    /**
     * @brief Default constructor. The class can be used as a base only.
     */
    ObjectDetector() = default;

    /**
     * @brief Copy constructor.
     * @param src Source object.
     */
    ObjectDetector(const ObjectDetector& src) = default;

    /**
     * @brief Move constructor.
     * @param src Source object.
     */
    ObjectDetector(ObjectDetector&& src) = default;

    /**
     * @brief Copy operator.
     * @param src Source object.
     * @return Reference to this object.
     */
    ObjectDetector& operator= (const ObjectDetector& src) = default;

    /**
     * @brief Move operator.
     * @param src Source object.
     * @return Reference to this object.
     */
    ObjectDetector& operator= (ObjectDetector&& src) = default;
};
}
