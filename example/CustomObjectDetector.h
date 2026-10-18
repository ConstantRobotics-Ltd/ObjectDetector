#pragma once
#include <mutex>
#include <string>
#include <vector>
#include "ObjectDetector.h"



namespace cr
{
namespace detector
{
/**
 * @brief Custom object detector class. The class shows how to implement
 * the ObjectDetector interface: all methods are implemented, the values from
 * outside are validated and the methods are thread-safe.
 */
class CustomObjectDetector : public ObjectDetector
{
public:

    /**
     * @brief Class constructor.
     */
    CustomObjectDetector();

    /**
     * @brief Class destructor.
     */
    ~CustomObjectDetector() override;

    /**
     * @brief Copy constructor is deleted: the class owns a mutex.
     */
    CustomObjectDetector(const CustomObjectDetector&) = delete;

    /**
     * @brief Move constructor is deleted: the class owns a mutex.
     */
    CustomObjectDetector(CustomObjectDetector&&) = delete;

    /**
     * @brief Copy operator is deleted: the class owns a mutex.
     */
    CustomObjectDetector& operator= (const CustomObjectDetector&) = delete;

    /**
     * @brief Move operator is deleted: the class owns a mutex.
     */
    CustomObjectDetector& operator= (CustomObjectDetector&&) = delete;

    /**
     * @brief Get string of current class version.
     * @return String of current class version: "Major.Minor.Patch".
     */
    static std::string getVersion();

    /**
     * @brief Init object detector. The method validates the values of all
     * params (see the limits in CustomObjectDetector.cpp) and does not
     * change the object if a value is not valid.
     * @param params Parameters class.
     * @return TRUE if the object detector is init or FALSE if not.
     */
    bool initObjectDetector(ObjectDetectorParams& params) override;

    /**
     * @brief Set object detector param. The method is thread-safe.
     * @param id Param ID.
     * @param value Param value. NaN, infinity and values out of the range of
     * the param are rejected.
     * @return TRUE if param was set or FALSE if not (invalid value, read only
     * or unknown param).
     */
    bool setParam(ObjectDetectorParam id, float value) override;

    /**
     * @brief Get object detector param value. The method is thread-safe.
     * @param id Param ID.
     * @return Param value or -1 (if the param is not supported).
     */
    float getParam(ObjectDetectorParam id) override;

    /**
     * @brief Get object detector params structure. The method is thread-safe.
     * @param params Output params object.
     */
    void getParams(ObjectDetectorParams& params) override;

    /**
     * @brief Get list of detected objects. The method is thread-safe.
     * @return List of objects. The list is empty if the detector is disabled.
     */
    std::vector<Object> getObjects() override;

    /**
     * @brief Execute command. The method is thread-safe.
     * @param id Command ID.
     * @return TRUE if the command was executed or FALSE if not.
     */
    bool executeCommand(ObjectDetectorCommand id) override;

    /**
     * @brief Perform detection.
     * @param frame Source video frame.
     * @return TRUE if video frame was processed or FALSE if not.
     */
    bool detect(cr::video::Frame& frame) override;

    /**
     * @brief Set detection mask.
     * @param mask Detection mask.
     * @return TRUE if the mask was set or FALSE if not.
     */
    bool setMask(cr::video::Frame mask) override;

    /**
     * @brief Decode and execute command. The method is thread-safe.
     * @param data Pointer to command data.
     * @param size Size of data.
     * @return TRUE if command decoded and executed or FALSE if not.
     */
    bool decodeAndExecuteCommand(uint8_t* data, int size) override;

private:

    /// Object detector params. Protected by m_paramsMutex.
    ObjectDetectorParams m_params;
    /// Mutex to protect m_params.
    std::mutex m_paramsMutex;
};
}
}
