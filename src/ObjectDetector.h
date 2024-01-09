#pragma once
#include <vector>
#include "Frame.h"
#include "ConfigReader.h"



namespace cr::detector
{
/**
 * @brief Object data structure.
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
    bool initString{ true };
    bool logMode{ true };
    bool frameBufferSize{ true };
    bool minObjectWidth{ true };
    bool maxObjectWidth{ true };
    bool minObjectHeight{ true };
    bool maxObjectHeight{ true };
    bool minXSpeed{ true };
    bool maxXSpeed{ true };
    bool minYSpeed{ true };
    bool maxYSpeed{ true };
    bool minDetectionProbability{ true };
    bool xDetectionCriteria{ true };
    bool yDetectionCriteria{ true };
    bool resetCriteria{ true };
    bool sensitivity{ true };
    bool scaleFactor{ true };
    bool numThreads{ true };
    bool processingTimeMcs{ true };
    bool type{ true };
    bool enable{ true };
    bool custom1{ true };
    bool custom2{ true };
    bool custom3{ true };
    bool objects{ true };
    bool classNames{ true };
};



/**
 * @brief Object detector params class.
 */
class ObjectDetectorParams
{
public:
    /// Init string. Depends on implementation.
    std::string initString{ "" };
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
    int xDetectionCriteria{1};
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
    /// Mode. Default: false - Off, on - On.
    bool enable{ true };
    /// Custom parameter. Depends on implementation.
    float custom1{ 0.0f };
    /// Custom parameter. Depends on implementation.
    float custom2{ 0.0f };
    /// Custom parameter. Depends on implementation.
    float custom3{ 0.0f };
    /// List of detected objects.
    std::vector<Object> objects;
    // A list of object class names used in detectors that recognize different
    // object classes. Detected objects have an attribute called 'type.'
    // If a detector doesn't support object class recognition or can't determine
    // the object type, the 'type' field must be set to 0. Otherwise, the 'type'
    // should correspond to the ordinal number of the class name from the 
    // 'classNames' list (if the list was set in params), starting from 1
    // (where the first element in the list has 'type == 1').
    std::vector<std::string> classNames{ "" };

    JSON_READABLE(ObjectDetectorParams, initString, logMode, frameBufferSize,
                  minObjectWidth, maxObjectWidth, minObjectHeight,
                  maxObjectHeight, minXSpeed, maxXSpeed, minYSpeed, maxYSpeed,
                  minDetectionProbability, xDetectionCriteria,
                  yDetectionCriteria, resetCriteria, sensitivity, scaleFactor,
                  numThreads, type, enable, custom1, custom2, custom3,
                  classNames);

    /**
     * @brief Copy operator.
     * @param src Source object.
     * @return ObjectDetectorParams object.
     */
    ObjectDetectorParams& operator= (const ObjectDetectorParams& src);

    /**
     * @brief Encode (serialize) params.
     * @param data Pointer to data buffer.
     * @param bufferSize Size of data buffer.
     * @param size Size of data.
     * @param mask Pointer to parameters mask.
     * @return TRUE if params encoded or FALSE if not.
     */
    bool encode(uint8_t* data, int bufferSize, int& size,
                ObjectDetectorParamsMask* mask = nullptr);

    /**
     * @brief Decode (deserialize) params.
     * @param data Pointer to data (serialized params).
     * @param dataSize Size of data.
     * @return TRUE is params decoded or FALSE if not.
     */
    bool decode(uint8_t* data, int dataSize);
};



/**
 * @brief Enum object detector parameters.
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
     * @param params Parameters class.
     * @return TRUE if the object detector is init or FALSE if not.
     */
    virtual bool initObjectDetector(ObjectDetectorParams& params) = 0;

    /**
     * @brief Set object detector param.
     * @param id Param ID.
     * @param value Param value to set.
     * @return TRUE if param was set of FALSE if not (invalid value or
     * particular param not supported).
     */
    virtual bool setParam(ObjectDetectorParam id, float value) = 0;

    /**
     * @brief Get object detector param value.
     * @param id Param ID.
     * @return Param value or -1 (if particular param not supported).
     */
    virtual float getParam(ObjectDetectorParam id) = 0;

    /**
     * @brief Get object detector params structure.
     * @param params Object detector params object. If particular object
     * detector doesn't support some params this params must have default values.
     */
    virtual void getParams(ObjectDetectorParams& params) = 0;

    /**
     * @brief Get list of detected objects. If object detector disabled the
     * list of detected objects always must be empty.
     * @return List of objects. If no detected object the list will be empty.
     */
    virtual std::vector<Object> getObjects() = 0;

    /**
     * @brief Execute command.
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
    * @brief Set detection mask. Detector omits image segments, where detection
    * mask pixel values equal 0.
    * @param mask Detection binary mask. Frame object. The object detector must
    * support pixel formats for mask: GRAY, NV12, NV21, YV12 and YU12.
    * @return TRUE if video detection mask was set or FALSE if not.
    */
    virtual bool setMask(cr::video::Frame mask) = 0;

    /**
     * @brief Encode set param command.
     * @param data Pointer to data buffer. Must have size >= 11.
     * @param size Size of encoded data (size of command).
     * @param id Param id.
     * @param value Param value.
     */
    static void encodeSetParamCommand(
            uint8_t* data, int& size, ObjectDetectorParam id, float value);

    /**
     * @brief Encode action command.
     * @param data Pointer to data buffer. Must have size >= 11.
     * @param size Size of encoded data (size of command).
     * @param id Command ID.
     */
    static void encodeCommand(
            uint8_t* data, int& size, ObjectDetectorCommand id);

    /**
     * @brief Decode command. Method decodes commands which prepared with
     * encodeSetParamCommand(...) and encodeCommand(...) methods.
     * @param data Pointer to command data.
     * @param size Size of data.
     * @param paramId Output (decoded) parameter ID.
     * @param commandId Output (decoded) command ID.
     * @param value Param value if decoded.
     * @return Return code:
     * 0  - action command decoded, commandId will be initialized.
     * 1  - set param command decoded,
     * -1 - error (no commands decoded).
     */
    static int decodeCommand(uint8_t* data, int size,
                             ObjectDetectorParam& paramId,
                             ObjectDetectorCommand& commandId,
                             float& value);

    /**
     * @brief Decode and execute command. Method decodes commands which encoded
     * which prepared with encodeSetParamCommand(...) and encodeCommand(...)
     * methods and call setParam(...) or executeCommand(...) methods.
     * @param data Pointer to command data.
     * @param size Size of data.
     * @return TRUE if command decoded and executed or FALSE if not.
     */
    virtual bool decodeAndExecuteCommand(uint8_t* data, int size) = 0;
};
}