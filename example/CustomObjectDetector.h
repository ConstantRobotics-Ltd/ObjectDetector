#pragma once
#include "ObjectDetector.h"



namespace cr
{
namespace detector
{
/// Custom object detector class.
class CustomObjectDetector: public ObjectDetector
{
public:

    /// Class constructor.
    CustomObjectDetector();

    /// Class destructor.
    ~CustomObjectDetector();

    /// Get string of current library version.
    std::string getVersion();

    /// Init object detector.
    bool initObjectDetector(ObjectDetectorParams& params);

    /// Set object detector param.
    bool setParam(ObjectDetectorParam id, float value);

    /// Get object detector param value.
    float getParam(ObjectDetectorParam id);

    /// Get object detector params structure.
    void getParams(ObjectDetectorParams& params);

    /// Get list of objects.
    std::vector<Object> getObjects();

    /// Execute command.
    bool executeCommand(ObjectDetectorCommand id);

    /// Perform detection.
    bool detect(cr::video::Frame& frame);

    /// Set detection mask.
    bool setMask(cr::video::Frame mask);

    /// Decode command.
    bool decodeAndExecuteCommand(uint8_t* data, int size);

private:

    /// Object detector params (default params).
    ObjectDetectorParams m_params;
};
}
}




