#include "CustomObjectDetector.h"
#include "CustomObjectDetectorVersion.h"
#include <cmath>
#include <cstdint>
#include <limits>
#include <mutex>
#include <string>
#include <vector>
#include "Frame.h"
#include "ObjectDetector.h"



namespace
{
/// Limit of float values which can be converted to int. 2^31 is exactly
/// representable as float, so (-LIMIT <= value < LIMIT) is the exact range of
/// int.
constexpr float INT_RANGE_LIMIT{2147483648.0f};
/// Maximum value of the logging mode: file and terminal.
constexpr int MAX_LOG_MODE{3};
/// Maximum width or height of an object, pixels. Limit of this example.
constexpr int MAX_OBJECT_SIZE{65536};
/// Maximum absolute value of an object speed, pixels/frame. Limit of this
/// example.
constexpr float MAX_SPEED{65536.0f};
/// Maximum value of a counter (frames, buffer size). Limit of this example.
constexpr int MAX_COUNT{1000000};
/// Maximum frame scaling factor. Limit of this example.
constexpr int MAX_SCALE_FACTOR{64};
/// Maximum number of threads. Limit of this example.
constexpr int MAX_THREADS{256};



/// Convert float param value to int by truncation. A conversion of NaN or of a
/// value outside of the int range is undefined behavior, so such values are
/// rejected.
bool toInt(float value, int& result)
{
    // The negation is intentional: the comparisons are false for NaN.
    if (!(value >= -INT_RANGE_LIMIT && value < INT_RANGE_LIMIT))
    {
        return false;
    }
    result = static_cast<int>(value);
    return true;
}



/// Check that the value is inside [minValue, maxValue].
bool isInRange(int value, int minValue, int maxValue)
{
    return value >= minValue && value <= maxValue;
}



/// Check that the value is inside [minValue, maxValue]. The result is FALSE
/// for NaN because every comparison with NaN is false.
bool isInRange(float value, float minValue, float maxValue)
{
    return value >= minValue && value <= maxValue;
}



/// Convert the float value to int and assign it to the target if the result
/// is inside [minValue, maxValue].
bool assignInt(int& target, float value, int minValue, int maxValue)
{
    int intValue = 0;
    if (!toInt(value, intValue) || !isInRange(intValue, minValue, maxValue))
    {
        return false;
    }
    target = intValue;
    return true;
}



/// Assign the value to the target if the value is inside [minValue, maxValue].
bool assignFloat(float& target, float value, float minValue, float maxValue)
{
    if (!isInRange(value, minValue, maxValue))
    {
        return false;
    }
    target = value;
    return true;
}



/// Assign the value to the target if the value is finite (not NaN or
/// infinity).
bool assignFinite(float& target, float value)
{
    if (!std::isfinite(value))
    {
        return false;
    }
    target = value;
    return true;
}



/// Check that all params are inside the limits of this example.
bool isValidParams(const cr::detector::ObjectDetectorParams& params)
{
    if (!isInRange(params.logMode, 0, MAX_LOG_MODE) ||
        !isInRange(params.frameBufferSize, 1, MAX_COUNT) ||
        !isInRange(params.scaleFactor, 1, MAX_SCALE_FACTOR) ||
        !isInRange(params.numThreads, 1, MAX_THREADS) ||
        !isInRange(params.xDetectionCriteria, 0, MAX_COUNT) ||
        !isInRange(params.yDetectionCriteria, 0, MAX_COUNT) ||
        !isInRange(params.resetCriteria, 0, MAX_COUNT) ||
        !isInRange(params.processingTimeMcs, 0, std::numeric_limits<int>::max()) ||
        params.type < 0)
    {
        return false;
    }
    if (!isInRange(params.minObjectWidth, 0, MAX_OBJECT_SIZE) ||
        !isInRange(params.maxObjectWidth, 0, MAX_OBJECT_SIZE) ||
        !isInRange(params.minObjectHeight, 0, MAX_OBJECT_SIZE) ||
        !isInRange(params.maxObjectHeight, 0, MAX_OBJECT_SIZE))
    {
        return false;
    }
    if (!isInRange(params.minXSpeed, -MAX_SPEED, MAX_SPEED) ||
        !isInRange(params.maxXSpeed, -MAX_SPEED, MAX_SPEED) ||
        !isInRange(params.minYSpeed, -MAX_SPEED, MAX_SPEED) ||
        !isInRange(params.maxYSpeed, -MAX_SPEED, MAX_SPEED))
    {
        return false;
    }
    if (!isInRange(params.minDetectionProbability, 0.0f, 1.0f) ||
        !isInRange(params.sensitivity, 0.0f, 1.0f) ||
        !std::isfinite(params.custom1) || !std::isfinite(params.custom2) ||
        !std::isfinite(params.custom3))
    {
        return false;
    }

    return true;
}
}



cr::detector::CustomObjectDetector::CustomObjectDetector() = default;



cr::detector::CustomObjectDetector::~CustomObjectDetector() = default;



std::string cr::detector::CustomObjectDetector::getVersion()
{
    return CUSTOM_OBJECT_DETECTOR_VERSION;
}



bool cr::detector::CustomObjectDetector::initObjectDetector(
        ObjectDetectorParams& params)
{
    // Check params before they are accepted.
    if (!isValidParams(params))
    {
        return false;
    }

    // Copy params.
    const std::lock_guard<std::mutex> lock(m_paramsMutex);
    m_params = params;

    return true;
}



bool cr::detector::CustomObjectDetector::setParam(
        ObjectDetectorParam id, float value)
{
    const std::lock_guard<std::mutex> lock(m_paramsMutex);

    // Check parameter ID.
    switch (id)
    {
    case ObjectDetectorParam::LOG_MODE:
    {
        return assignInt(m_params.logMode, value, 0, MAX_LOG_MODE);
    }
    case ObjectDetectorParam::FRAME_BUFFER_SIZE:
    {
        return assignInt(m_params.frameBufferSize, value, 1, MAX_COUNT);
    }
    case ObjectDetectorParam::MIN_OBJECT_WIDTH:
    {
        return assignInt(m_params.minObjectWidth, value, 0, MAX_OBJECT_SIZE);
    }
    case ObjectDetectorParam::MAX_OBJECT_WIDTH:
    {
        return assignInt(m_params.maxObjectWidth, value, 0, MAX_OBJECT_SIZE);
    }
    case ObjectDetectorParam::MIN_OBJECT_HEIGHT:
    {
        return assignInt(m_params.minObjectHeight, value, 0, MAX_OBJECT_SIZE);
    }
    case ObjectDetectorParam::MAX_OBJECT_HEIGHT:
    {
        return assignInt(m_params.maxObjectHeight, value, 0, MAX_OBJECT_SIZE);
    }
    case ObjectDetectorParam::MIN_X_SPEED:
    {
        return assignFloat(m_params.minXSpeed, value, -MAX_SPEED, MAX_SPEED);
    }
    case ObjectDetectorParam::MAX_X_SPEED:
    {
        return assignFloat(m_params.maxXSpeed, value, -MAX_SPEED, MAX_SPEED);
    }
    case ObjectDetectorParam::MIN_Y_SPEED:
    {
        return assignFloat(m_params.minYSpeed, value, -MAX_SPEED, MAX_SPEED);
    }
    case ObjectDetectorParam::MAX_Y_SPEED:
    {
        return assignFloat(m_params.maxYSpeed, value, -MAX_SPEED, MAX_SPEED);
    }
    case ObjectDetectorParam::MIN_DETECTION_PROBABILITY:
    {
        return assignFloat(m_params.minDetectionProbability, value, 0.0f, 1.0f);
    }
    case ObjectDetectorParam::X_DETECTION_CRITERIA:
    {
        return assignInt(m_params.xDetectionCriteria, value, 0, MAX_COUNT);
    }
    case ObjectDetectorParam::Y_DETECTION_CRITERIA:
    {
        return assignInt(m_params.yDetectionCriteria, value, 0, MAX_COUNT);
    }
    case ObjectDetectorParam::RESET_CRITERIA:
    {
        return assignInt(m_params.resetCriteria, value, 0, MAX_COUNT);
    }
    case ObjectDetectorParam::SENSITIVITY:
    {
        return assignFloat(m_params.sensitivity, value, 0.0f, 1.0f);
    }
    case ObjectDetectorParam::SCALE_FACTOR:
    {
        return assignInt(m_params.scaleFactor, value, 1, MAX_SCALE_FACTOR);
    }
    case ObjectDetectorParam::NUM_THREADS:
    {
        return assignInt(m_params.numThreads, value, 1, MAX_THREADS);
    }
    case ObjectDetectorParam::PROCESSING_TIME_MCS:
    {
        // Read only param.
        return false;
    }
    case ObjectDetectorParam::TYPE:
    {
        return assignInt(m_params.type, value, 0,
                         std::numeric_limits<int>::max());
    }
    case ObjectDetectorParam::MODE:
    {
        int mode = 0;
        if (!assignInt(mode, value, 0, 1))
        {
            return false;
        }
        m_params.enable = mode != 0;
        if (!m_params.enable)
        {
            m_params.objects.clear();
        }
        return true;
    }
    case ObjectDetectorParam::CUSTOM_1:
    {
        return assignFinite(m_params.custom1, value);
    }
    case ObjectDetectorParam::CUSTOM_2:
    {
        return assignFinite(m_params.custom2, value);
    }
    case ObjectDetectorParam::CUSTOM_3:
    {
        return assignFinite(m_params.custom3, value);
    }
    default:
    {
        return false;
    }
    }
}



float cr::detector::CustomObjectDetector::getParam(ObjectDetectorParam id)
{
    const std::lock_guard<std::mutex> lock(m_paramsMutex);

    // Check parameter ID.
    switch (id)
    {
    case ObjectDetectorParam::LOG_MODE:
    {
        return static_cast<float>(m_params.logMode);
    }
    case ObjectDetectorParam::FRAME_BUFFER_SIZE:
    {
        return static_cast<float>(m_params.frameBufferSize);
    }
    case ObjectDetectorParam::MIN_OBJECT_WIDTH:
    {
        return static_cast<float>(m_params.minObjectWidth);
    }
    case ObjectDetectorParam::MAX_OBJECT_WIDTH:
    {
        return static_cast<float>(m_params.maxObjectWidth);
    }
    case ObjectDetectorParam::MIN_OBJECT_HEIGHT:
    {
        return static_cast<float>(m_params.minObjectHeight);
    }
    case ObjectDetectorParam::MAX_OBJECT_HEIGHT:
    {
        return static_cast<float>(m_params.maxObjectHeight);
    }
    case ObjectDetectorParam::MIN_X_SPEED:
    {
        return m_params.minXSpeed;
    }
    case ObjectDetectorParam::MAX_X_SPEED:
    {
        return m_params.maxXSpeed;
    }
    case ObjectDetectorParam::MIN_Y_SPEED:
    {
        return m_params.minYSpeed;
    }
    case ObjectDetectorParam::MAX_Y_SPEED:
    {
        return m_params.maxYSpeed;
    }
    case ObjectDetectorParam::MIN_DETECTION_PROBABILITY:
    {
        return m_params.minDetectionProbability;
    }
    case ObjectDetectorParam::X_DETECTION_CRITERIA:
    {
        return static_cast<float>(m_params.xDetectionCriteria);
    }
    case ObjectDetectorParam::Y_DETECTION_CRITERIA:
    {
        return static_cast<float>(m_params.yDetectionCriteria);
    }
    case ObjectDetectorParam::RESET_CRITERIA:
    {
        return static_cast<float>(m_params.resetCriteria);
    }
    case ObjectDetectorParam::SENSITIVITY:
    {
        return m_params.sensitivity;
    }
    case ObjectDetectorParam::SCALE_FACTOR:
    {
        return static_cast<float>(m_params.scaleFactor);
    }
    case ObjectDetectorParam::NUM_THREADS:
    {
        return static_cast<float>(m_params.numThreads);
    }
    case ObjectDetectorParam::PROCESSING_TIME_MCS:
    {
        return static_cast<float>(m_params.processingTimeMcs);
    }
    case ObjectDetectorParam::TYPE:
    {
        return static_cast<float>(m_params.type);
    }
    case ObjectDetectorParam::MODE:
    {
        return m_params.enable ? 1.0f : 0.0f;
    }
    case ObjectDetectorParam::CUSTOM_1:
    {
        return m_params.custom1;
    }
    case ObjectDetectorParam::CUSTOM_2:
    {
        return m_params.custom2;
    }
    case ObjectDetectorParam::CUSTOM_3:
    {
        return m_params.custom3;
    }
    default:
    {
        return -1.0f;
    }
    }
}



void cr::detector::CustomObjectDetector::getParams(
        ObjectDetectorParams& params)
{
    const std::lock_guard<std::mutex> lock(m_paramsMutex);
    params = m_params;

    // The list of objects is always empty if the detector is disabled.
    if (!params.enable)
    {
        params.objects.clear();
    }
}



std::vector<cr::detector::Object>
cr::detector::CustomObjectDetector::getObjects()
{
    const std::lock_guard<std::mutex> lock(m_paramsMutex);
    if (!m_params.enable)
    {
        return std::vector<Object>();
    }

    return m_params.objects;
}



bool cr::detector::CustomObjectDetector::executeCommand(
        ObjectDetectorCommand id)
{
    const std::lock_guard<std::mutex> lock(m_paramsMutex);

    // Check command ID.
    switch (id)
    {
    case ObjectDetectorCommand::RESET:
    {
        m_params.objects.clear();
        return true;
    }
    case ObjectDetectorCommand::ON:
    {
        m_params.enable = true;
        return true;
    }
    case ObjectDetectorCommand::OFF:
    {
        m_params.enable = false;
        m_params.objects.clear();
        return true;
    }
    default:
    {
        return false;
    }
    }
}



bool cr::detector::CustomObjectDetector::detect(cr::video::Frame&)
{
    return true;
}



bool cr::detector::CustomObjectDetector::setMask(cr::video::Frame)
{
    return true;
}



bool cr::detector::CustomObjectDetector::decodeAndExecuteCommand(
        uint8_t* data, int size)
{
    // Decode command. The decoder checks the size, the version and the IDs.
    ObjectDetectorParam paramId{ObjectDetectorParam::LOG_MODE};
    ObjectDetectorCommand commandId{ObjectDetectorCommand::RESET};
    float value{0.0f};
    const int result = decodeCommand(data, size, paramId, commandId, value);

    // Execute the decoded command.
    if (result == 0)
    {
        return executeCommand(commandId);
    }
    if (result == 1)
    {
        return setParam(paramId, value);
    }

    return false;
}
