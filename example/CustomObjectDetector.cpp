#include "CustomObjectDetector.h"
#include "CustomObjectDetectorVersion.h"



std::string getVersion()
{
    return CUSTOM_OBJECT_DETECTOR_VERSION;
}



bool cr::detector::CustomObjectDetector::initObjectDetector(cr::detector::ObjectDetectorParams& params)
{
    // Copy params.
    m_params = params;

    return true;
}



bool cr::detector::CustomObjectDetector::setParam(ObjectDetectorParam id, float value)
{
    // Check parameter ID.
    switch (id)
    {
    case cr::detector::ObjectDetectorParam::LOG_MODE:
    {
        m_params.logMode = (int)value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::FRAME_BUFFER_SIZE:
    {
        m_params.frameBufferSize = (int)value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::MIN_OBJECT_WIDTH:
    {
        m_params.minObjectWidth = (int)value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::MAX_OBJECT_WIDTH:
    {
        m_params.maxObjectWidth = (int)value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::MIN_OBJECT_HEIGHT:
    {
        m_params.minObjectHeight = (int)value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::MAX_OBJECT_HEIGHT:
    {
        m_params.maxObjectHeight = (int)value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::MIN_X_SPEED:
    {
        m_params.minXSpeed = value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::MAX_X_SPEED:
    {
        m_params.maxXSpeed = value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::MIN_Y_SPEED:
    {
        m_params.minYSpeed = value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::MAX_Y_SPEED:
    {
        m_params.maxYSpeed = value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::MIN_DETECTION_PROBABILITY:
    {
        m_params.minDetectionProbability = value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::X_DETECTION_CRITERIA:
    {
        m_params.xDetectionCriteria = (int)value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::Y_DETECTION_CRITERIA:
    {
        m_params.yDetectionCriteria = (int)value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::RESET_CRITERIA:
    {
        m_params.resetCriteria = (int)value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::SENSITIVITY:
    {
        m_params.sensitivity = value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::SCALE_FACTOR:
    {
        m_params.scaleFactor = (int)value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::NUM_THREADS:
    {
        m_params.numThreads = (int)value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::PROCESSING_TIME_MKS:
    {
        // Read only params.
        return true;
    }
    case cr::detector::ObjectDetectorParam::TYPE:
    {
        m_params.type = (int)value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::MODE:
    {
        m_params.enable = (int)value == 0 ? false : true;
        return true;
    }
    case cr::detector::ObjectDetectorParam::CUSTOM_1:
    {
        m_params.custom1 = value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::CUSTOM_2:
    {
        m_params.custom2 = value;
        return true;
    }
    case cr::detector::ObjectDetectorParam::CUSTOM_3:
    {
        m_params.custom2 = value;
        return true;
    }
    default:
    {
        return false;
    }
    }

    return false;
}



float cr::detector::CustomObjectDetector::getParam(ObjectDetectorParam id)
{
    // Check parameter ID.
    switch (id)
    {
    case cr::detector::ObjectDetectorParam::LOG_MODE:
    {
        return (float)m_params.logMode;
    }
    case cr::detector::ObjectDetectorParam::FRAME_BUFFER_SIZE:
    {
        return (float)m_params.frameBufferSize;
    }
    case cr::detector::ObjectDetectorParam::MIN_OBJECT_WIDTH:
    {
        return (float)m_params.minObjectWidth;
    }
    case cr::detector::ObjectDetectorParam::MAX_OBJECT_WIDTH:
    {
        return (float)m_params.maxObjectWidth;
    }
    case cr::detector::ObjectDetectorParam::MIN_OBJECT_HEIGHT:
    {
        return (float)m_params.minObjectHeight;
    }
    case cr::detector::ObjectDetectorParam::MAX_OBJECT_HEIGHT:
    {
        return (float)m_params.maxObjectHeight;
    }
    case cr::detector::ObjectDetectorParam::MIN_X_SPEED:
    {
        return m_params.minXSpeed;
    }
    case cr::detector::ObjectDetectorParam::MAX_X_SPEED:
    {
        return m_params.maxXSpeed;
    }
    case cr::detector::ObjectDetectorParam::MIN_Y_SPEED:
    {
        return m_params.minYSpeed;
    }
    case cr::detector::ObjectDetectorParam::MAX_Y_SPEED:
    {
        return m_params.maxYSpeed;
    }
    case cr::detector::ObjectDetectorParam::MIN_DETECTION_PROBABILITY:
    {
        return m_params.minDetectionProbability;
    }
    case cr::detector::ObjectDetectorParam::X_DETECTION_CRITERIA:
    {
        return (float)m_params.xDetectionCriteria;
    }
    case cr::detector::ObjectDetectorParam::Y_DETECTION_CRITERIA:
    {
        return (float)m_params.yDetectionCriteria;
    }
    case cr::detector::ObjectDetectorParam::RESET_CRITERIA:
    {
        return (float)m_params.resetCriteria;
    }
    case cr::detector::ObjectDetectorParam::SENSITIVITY:
    {
        return m_params.sensitivity;
    }
    case cr::detector::ObjectDetectorParam::SCALE_FACTOR:
    {
        return (float)m_params.scaleFactor;
    }
    case cr::detector::ObjectDetectorParam::NUM_THREADS:
    {
        return (float)m_params.numThreads;
    }
    case cr::detector::ObjectDetectorParam::PROCESSING_TIME_MKS:
    {
        return (float)m_params.processingTimeMks;
    }
    case cr::detector::ObjectDetectorParam::TYPE:
    {
        return (float)m_params.type;
    }
    case cr::detector::ObjectDetectorParam::MODE:
    {
        return m_params.enable ? 1.0f : 0.0f;
    }
    case cr::detector::ObjectDetectorParam::CUSTOM_1:
    {
        return m_params.custom1;
    }
    case cr::detector::ObjectDetectorParam::CUSTOM_2:
    {
        return m_params.custom2;
    }
    case cr::detector::ObjectDetectorParam::CUSTOM_3:
    {
        return m_params.custom2;
    }
    default:
    {
        return -1.0f;
    }
    }

    return -1.0f;
}



void cr::detector::CustomObjectDetector::getParams(cr::detector::ObjectDetectorParams& params)
{
    params = m_params;
}



std::vector<cr::detector::Object> cr::detector::CustomObjectDetector::getObjects()
{
    return m_params.objects;
}



bool cr::detector::CustomObjectDetector::executeCommand(ObjectDetectorCommand id)
{
    // Check command ID.
    switch (id)
    {
    case cr::detector::ObjectDetectorCommand::RESET:
    {
        return true;
    }
    case cr::detector::ObjectDetectorCommand::ON:
    {
        return true;
    }
    case cr::detector::ObjectDetectorCommand::OFF:
    {
        return true;
    }
    default:
    {
        return false;
    }
    }

    return false;
}



bool cr::detector::CustomObjectDetector::detect(cr::video::Frame& frame)
{
    return true;
}



bool cr::detector::CustomObjectDetector::setMask(cr::video::Frame mask)
{
    return true;
}



bool cr::detector::CustomObjectDetector::decodeAndExecuteCommand(uint8_t* data, int size)
{
    ObjectDetectorParam paramId;
    ObjectDetectorCommand commandId;
    float value = 0;

    switch (decodeCommand(data, size, paramId, commandId, value))
    {
    case 0:
    {
        return executeCommand(commandId);
    }
    case 1:
    {
        return setParam(paramId, value);
    }
    default:
    {
        return false;
    }
    }

    return false;
}
