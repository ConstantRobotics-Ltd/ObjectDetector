![object_detector_web_logo](./static/object_detector_web_logo.png)



# **ObjectDetector interface C++ library**

**v1.8.1**



# Table of contents

- [Overview](#overview)
- [Versions](#versions)
- [Library files](#library-files)
- [ObjectDetector interface class description](#objectdetector-interface-class-description)
  - [ObjectDetector class declaration](#objectdetector-class-declaration)
  - [getVersion method](#getversion-method)
  - [initObjectDetector method](#initobjectdetector-method)
  - [setParam method](#setparam-method)
  - [getParam method](#getparam-method)
  - [getParams method](#getparams-method)
  - [getObjects method](#getobjects-method)
  - [executeCommand method](#executecommand-method)
  - [detect method](#detect-method)
  - [setMask method](#setmask-method)
  - [encodeSetParamCommand method](#encodesetparamcommand-method)
  - [encodeCommand method](#encodecommand-method)
  - [decodeCommand method](#decodecommand-method)
  - [decodeAndExecuteCommand method](#decodeandexecutecommand-method)
- [Data structures](#data-structures)
  - [ObjectDetectorCommand enum](#objectdetectorcommand-enum)
  - [ObjectDetectorParam enum](#objectdetectorparam-enum)
  - [Object structure](#object-structure)
- [ObjectDetectorParams class description](#objectdetectorparams-class-description)
  - [ObjectDetectorParams class declaration](#objectdetectorparams-class-declaration)
  - [Serialize object detector params](#serialize-object-detector-params)
  - [Deserialize object detector params](#deserialize-object-detector-params)
  - [Read params from JSON file and write to JSON file](#read-params-from-json-file-and-write-to-json-file)
- [Build and connect to your project](#build-and-connect-to-your-project)
  - [Build stand alone](#build-stand-alone)
  - [CMake options](#cmake-options)
  - [Connect to your project](#connect-to-your-project)
- [Portability](#portability)
- [How to create a custom implementation](#how-to-create-a-custom-implementation)



# Overview

**ObjectDetector** C++ library provides standard interface as well as defines data structures and rules for different object detectors (motion detectors, events detectors, neural networks etc.). **ObjectDetector** interface class does not do anything, just provides interface, defines data structures and provides methods to encode/decode commands and encode/decode (serialize/deserialize) parameters. Different object detector classes inherit interface form **ObjectDetector** C++ class. **ObjectDetector.h** file contains [ObjectDetectorCommand](#objectdetectorcommand-enum) enum (provides enum of action commands), [ObjectDetectorParam](#objectdetectorparam-enum) enum (provides enum of params), [ObjectDetectorParams](#objectdetectorparams-class-description) class (contains list of params) and includes [ObjectDetector](#objectdetector-class-declaration) class declaration. All object detectors should include params and commands listed in **ObjectDetector.h** file. **ObjectDetector** class depends on two external libraries: [Frame](https://rapidpixel.constantrobotics.com/docs/Service/Frame.html) (video frame data structure) and [ConfigReader](https://rapidpixel.constantrobotics.com/docs/Service/ConfigReader.html) (provides methods to read/write JSON config files). The repositories of these libraries must be placed next to the repository of **ObjectDetector** (see [Build and connect to your project](#build-and-connect-to-your-project) section). It uses C++17 standard.



# Versions

**Table 1** - Library versions.

| Version | Release date | What's new                                                   |
| ------- | ------------ | ------------------------------------------------------------ |
| 1.0.0   | 17.07.2023   | First version.                                               |
| 1.0.1   | 17.07.2023   | - 3rdparty variable name mistake fixed.                      |
| 1.1.0   | 18.07.2023   | - Added frame ID field for detection results.                |
| 1.2.0   | 22.08.2023   | - Added new setMask method.                                  |
| 1.3.0   | 12.09.2023   | - Changed params serialization method.                       |
| 1.4.0   | 24.09.2023   | - Updated encode(...) and decode(...) methods of ObjectDetectorParams.<br />- Added decodeAndExecuteCommand(...) method.<br />- Added example of object detector implementation. |
| 1.5.0   | 26.09.2023   | - Signature of getParams(...) method changed.                |
| 1.5.1   | 13.11.2023   | - Frame class updated.                                       |
| 1.6.1   | 14.12.2023   | - Virtual destructor added.<br />- Frame class updated.      |
| 1.7.0   | 08.01.2024   | - List of objects class names added to ObjectDetectorParams.<br />- Data structures updated.<br />- Added automatic linux build check based on GitHub Actions. |
| 1.7.1   | 25.03.2024   | - Frame class updated.<br />- ConfigReader class updated.<br />- Documentation updated. |
| 1.7.2   | 20.05.2024   | - Frame class updated.<br />- ConfigReader class updated.<br />- Documentation updated. |
| 1.7.3   | 23.07.2024   | - CMake structure updated.                                   |
| 1.8.0   | 02.10.2026   | - Submodules updated.<br />- Compiler warnings fixed.        |
| 1.8.1   | 17.10.2026   | - Git submodules removed: the repositories of Frame and ConfigReader must be placed next to the repository of the library.<br />- ObjectDetectorParams::decode(...) checks the size of data, the number of objects and the strings, rejects data with bytes after the last field and does not change the object if the data is rejected.<br />- ObjectDetectorParams::encode(...) checks the buffer size for all fields including the strings, encodes the objects which fit into the buffer completely, rejects null characters in the strings and the empty class names inside the list.<br />- ObjectDetector::decodeCommand(...) checks the exact size of the command and the range of the ID and returns -1 for all errors.<br />- Null pointer arguments of encode and decode methods are rejected.<br />- Example of object detector implementation completed: all methods are implemented, values are validated, methods are thread-safe.<br />- Compiler warnings fixed, strict warnings and hardening options added to CMake files, test application extended.<br />- Documentation updated. |



# Library files

The library is supplied as source code only. The user is provided with a set of files in the form of a CMake project (repository). The repository structure is shown below:

```xml
CMakeLists.txt ------------------------ Main CMake file of the library.
example ------------------------------- Folder with example object detector class.
    CMakeLists.txt -------------------- CMake file for custom object detector class.
    CustomObjectDetector.cpp ---------- C++ implementation file.
    CustomObjectDetector.h ------------ Header with class declaration.
    CustomObjectDetectorVersion.h ----- Header file which includes class version.
    CustomObjectDetectorVersion.h.in -- CMake service file to generate version file.
test ---------------------------------- Folder with test of data structures.
    CMakeLists.txt -------------------- CMake file for test application.
    main.cpp -------------------------- Source code file of test application.
src ----------------------------------- Folder with source code of the library.
    CMakeLists.txt -------------------- CMake file of the library.
    ObjectDetector.cpp ---------------- C++ implementation file.
    ObjectDetector.h ------------------ Header file with class declaration.
    ObjectDetectorVersion.h ----------- Header file with class version.
    ObjectDetectorVersion.h.in -------- CMake service file to generate version file.
```



# ObjectDetector interface class description



## ObjectDetector class declaration

**ObjectDetector** interface class declared in **ObjectDetector.h** file. Class declaration:

```cpp
class ObjectDetector
{
public:

    /// Class destructor.
    virtual ~ObjectDetector();

    /// Get string of current library version.
    static std::string getVersion();

    /// Init object detector.
    virtual bool initObjectDetector(ObjectDetectorParams& params) = 0;

    /// Set object detector param.
    virtual bool setParam(ObjectDetectorParam id, float value) = 0;

    /// Get object detector param value.
    virtual float getParam(ObjectDetectorParam id) = 0;

    /// Get object detector params structure.
    virtual void getParams(ObjectDetectorParams& params) = 0;

    /// Get list of detected objects.
    virtual std::vector<Object> getObjects() = 0;

    /// Execute command.
    virtual bool executeCommand(ObjectDetectorCommand id) = 0;

    /// Perform detection.
    virtual bool detect(rapidpixel::Frame& frame) = 0;

    /// Set detection mask.
    virtual bool setMask(rapidpixel::Frame mask) = 0;

    /// Encode set param command.
    static void encodeSetParamCommand(
            uint8_t* data, int& size, ObjectDetectorParam id, float value);

    /// Encode action command.
    static void encodeCommand(
            uint8_t* data, int& size, ObjectDetectorCommand id);

    /// Decode command.
    static int decodeCommand(uint8_t* data, int size,
                             ObjectDetectorParam& paramId,
                             ObjectDetectorCommand& commandId,
                             float& value);

    /// Decode and execute command.
    virtual bool decodeAndExecuteCommand(uint8_t* data, int size) = 0;

protected:

    /// Default constructor. The class can be used as a base only.
    ObjectDetector() = default;

    /// Copy constructor.
    ObjectDetector(const ObjectDetector& src) = default;

    /// Move constructor.
    ObjectDetector(ObjectDetector&& src) = default;

    /// Copy operator.
    ObjectDetector& operator= (const ObjectDetector& src) = default;

    /// Move operator.
    ObjectDetector& operator= (ObjectDetector&& src) = default;
};
```



## getVersion method

The **getVersion()** method returns string of current version of **ObjectDetector** class. Particular object detector class can have it's own **getVersion()** method. Method declaration:

```cpp
static std::string getVersion();
```

Method can be used without **ObjectDetector** class instance:

```cpp
std::cout << "ObjectDetector class version: " <<
             rapidpixel::ObjectDetector::getVersion() << std::endl;
```

Console output:

```bash
ObjectDetector class version: 1.8.1
```



## initObjectDetector method

The **initObjectDetector(...)** method initializes object detector. Particular object detector class should initialize only supported parameters from [ObjectDetectorParams](#objectdetectorparams-class-description) class. Method declaration:

```cpp
virtual bool initObjectDetector(ObjectDetectorParams& params) = 0;
```

| Parameter | Value                                                        |
| --------- | ------------------------------------------------------------ |
| params    | Object detector parameters class. Particular object detector class may not support all params listed in [ObjectDetectorParams](#objectdetectorparams-class-description) class. If object detector doesn't support particular params this params should have default values. The values of the params are not checked by the interface: the implementation must validate them. |

**Returns:** TRUE if the object detector initialized or FALSE if not.



## setParam method

The **setParam(...)** method sets the object detector param value. Particular object detector class must provide thread-safe **setParam(...)** method call. This means that the **setParam(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual bool setParam(ObjectDetectorParam id, float value) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| id        | Parameter ID according to [ObjectDetectorParam](#objectdetectorparam-enum) enum. |
| value     | Parameter value. Value depends on parameter ID. The implementation must check that the value is valid (it can be NaN, infinity or out of the range of the parameter) before it is converted or used. |

**Returns:** TRUE if the parameter was set or FALSE if not (invalid value or the parameter is not supported).



## getParam method

The **getParam(...)** method returns object detector parameter value. Particular object detector class must provide thread-safe **getParam(...)** method call. This means that the **getParam(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual float getParam(ObjectDetectorParam id) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| id        | Parameter ID according to [ObjectDetectorParam](#objectdetectorparam-enum) enum. |

**Returns:** parameter value or -1 if the parameter is not supported by particular object detector class.



## getParams method

The **getParams(...)** method returns object detector params class object as well as the list of detected objects. Particular object detector class must provide thread-safe **getParams(...)** method call. This means that the **getParams(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual void getParams(ObjectDetectorParams& params) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| params    | Output [ObjectDetectorParams](#objectdetectorparams-class-description) class object. If particular object detector doesn't support some params this params must have default values. |



## getObjects method

The **getObjects()** method returns list of detected objects. User can obtain list of detected objects via **getParams(...)** method as well. Particular object detector class must provide thread-safe **getObjects(...)** method call. This means that the **getObjects(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual std::vector<Object> getObjects() = 0;
```

**Returns:** list of detected objects (see [Object](#object-structure) class description). If no detected object the list will be empty. If object detector is disabled the list is always empty.



## executeCommand method

The **executeCommand(...)** method executes object detector command. Particular object detector class must provide thread-safe **executeCommand(...)** method call. This means that the **executeCommand(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual bool executeCommand(ObjectDetectorCommand id) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| id        | Command ID according to [ObjectDetectorCommand](#objectdetectorcommand-enum) enum. |

**Returns:** TRUE if the command was executed or FALSE if not.



## detect method

The **detect(...)** method performs detection algorithm. Method declaration:

```cpp
virtual bool detect(rapidpixel::Frame& frame) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| frame     | Video frame for processing. Object detector processes only RAW pixel formats (BGR24, RGB24, GRAY, YUYV24, YUYV, UYVY, NV12, NV21, YV12, YU12, see [Frame](https://rapidpixel.constantrobotics.com/docs/Service/Frame.html) class description). Required pixel format depends on implementation. It is recommended to support all possible RAW pixel formats supported by [Frame](https://rapidpixel.constantrobotics.com/docs/Service/Frame.html) object. |

**Returns:** TRUE if the video frame was processed or FALSE if not. If object detector disabled (see [ObjectDetectorParam](#objectdetectorparam-enum) enum description) the method should return TRUE.



## setMask method

The **setMask(...)** method designed to set detection mask. Method declaration:

```cpp
virtual bool setMask(rapidpixel::Frame mask) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| mask      | Detection binary mask: [Frame](https://rapidpixel.constantrobotics.com/docs/Service/Frame.html) object. The object detector must support pixel formats for mask: GRAY, NV12, NV21, YV12 and YU12. Detector omits image segments, where detection mask pixel values equal 0. |

**Returns:** TRUE if the detection mask was set or FALSE if not.



## encodeSetParamCommand method

The **encodeSetParamCommand(...)** static method designed to encode command to change any parameter for remote object detector. To control object detector remotely, the developer has to design his own protocol and according to it encode the command and deliver it over the communication channel. To simplify this, the **ObjectDetector** class contains static methods for encoding the control command. The **ObjectDetector** class provides set param commands and an action command. **encodeSetParamCommand(...)** encodes set param commands. Method declaration:

```cpp
static void encodeSetParamCommand(uint8_t* data, int& size, ObjectDetectorParam id, float value);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to data buffer for encoded command. Must have size >= 11. If the pointer is nullptr nothing is written and the size is set to 0. |
| size      | Size of encoded data. Will be 11 bytes.                      |
| id        | Parameter ID according to [ObjectDetectorParam](#objectdetectorparam-enum) enum. The ID is not checked by the method: **decodeCommand(...)** rejects an ID which is not a member of the enum. |
| value     | Parameter value. Value depends on parameter ID. The value is not checked by the method. |

**encodeSetParamCommand(...)** is a static method and can be used without **ObjectDetector** class instance. This method can be used on client side (control system). Command encoding example:

```cpp
// Buffer for encoded data.
uint8_t data[11];
// Size of encoded data.
int size = 0;
// Parameter value.
float value = 10.0f;
// Encode command.
rapidpixel::ObjectDetector::encodeSetParamCommand(
    data, size, rapidpixel::ObjectDetectorParam::MIN_OBJECT_WIDTH, value);
```



## encodeCommand method

The **encodeCommand(...)** static method designed to encode command for remote object detector. To control object detector remotely, the developer has to design his own protocol and according to it encode the command and deliver it over the communication channel. To simplify this, the **ObjectDetector** class contains static methods for encoding the control command. The **ObjectDetector** class provides set param commands and an action command. **encodeCommand(...)** encodes action commands. Method declaration:

```cpp
static void encodeCommand(uint8_t* data, int& size, ObjectDetectorCommand id);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to data buffer for encoded command. Must have size >= 7. If the pointer is nullptr nothing is written and the size is set to 0. |
| size      | Size of encoded data. Will be 7 bytes.                       |
| id        | Command ID according to [ObjectDetectorCommand](#objectdetectorcommand-enum) enum. The ID is not checked by the method: **decodeCommand(...)** rejects an ID which is not a member of the enum. |

**encodeCommand(...)** is static and used without **ObjectDetector** class instance. This method used on client side (control system). Command encoding example:

```cpp
// Buffer for encoded data.
uint8_t data[7];
// Size of encoded data.
int size = 0;
// Encode command.
rapidpixel::ObjectDetector::encodeCommand(
    data, size, rapidpixel::ObjectDetectorCommand::RESET);
```



## decodeCommand method

The **decodeCommand(...)** static method decodes command on object detector side (edge device). Method declaration:

```cpp
static int decodeCommand(uint8_t* data, int size, ObjectDetectorParam& paramId, ObjectDetectorCommand& commandId, float& value);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to input command. Must not be nullptr. The pointer must be valid for **size** bytes. |
| size      | Size of command. Must be exactly 11 bytes for set param command and exactly 7 bytes for action command. |
| paramId   | Output (decoded) parameter ID according to [ObjectDetectorParam](#objectdetectorparam-enum) enum. After decoding set param command the method will return parameter ID. |
| commandId | Output (decoded) command ID according to [ObjectDetectorCommand](#objectdetectorcommand-enum) enum. After decoding COMMAND the method will return command ID. |
| value     | Parameter value (after decoding set param command). Set to 0 after decoding action command. |

**Returns:** **0** - in case decoding action command, **1** - in case decoding set param command or **-1** in case error (data is nullptr, wrong size, wrong version, wrong command type or ID which is not a member of the enum). The method checks the version (MAJOR.MINOR) of the library which encoded the command. The output arguments are not changed if the method returns -1.



## decodeAndExecuteCommand method

The **decodeAndExecuteCommand(...)** method decodes and executes command on object detector side (edge device). Method decodes commands which prepared with encodeSetParamCommand(...) and encodeCommand(...) methods and call setParam(...) or executeCommand(...) methods. The particular implementation of the object detector must provide thread-safe **decodeAndExecuteCommand(...)** method call. This means that the **decodeAndExecuteCommand(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual bool decodeAndExecuteCommand(uint8_t* data, int size) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to input command.                                    |
| size      | Size of command. Must be 11 bytes for set param command or 7 bytes for action command. |

**Returns:** TRUE if command decoded and executed (action command or set param command) or FALSE if not (the command is not valid, see [decodeCommand method](#decodecommand-method), or the command was rejected by the implementation).



# Data structures



## ObjectDetectorCommand enum

ObjectDetectorCommand enum represents action commands. Enum declaration:

```cpp
enum class ObjectDetectorCommand
{
    /// Reset.
    RESET = 1,
    /// Enable.
    ON,
    /// Disable.
    OFF
};
```

**Table 2** - Object detector action commands description. Some commands maybe unsupported by particular object detector class.

| Command | Description                                                  |
| ------- | ------------------------------------------------------------ |
| RESET   | Reset object detector. After reset the list of detected object should be empty until new objects will be detected. |
| ON      | Enable object detector.                                      |
| OFF     | Disable object detector. If object detector disabled the list of detected objects must be empty. |



## ObjectDetectorParam enum

ObjectDetectorParam enum represents detector parameters. Enum declaration:

```cpp
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
```

**Table 3** - Object detector params description. Some params maybe unsupported by particular object detector class.

| Parameter                 | Access       | Description                                                  |
| ------------------------- | ------------ | ------------------------------------------------------------ |
| LOG_MODE                  | read / write | Logging mode. Defines where log information will be printed in. Values:<br />0 - Disable,<br />1 - Only file,<br />2 - Only terminal (console),<br />3 - File and terminal. |
| FRAME_BUFFER_SIZE         | read / write | Frame buffer size. Depends on implementation. It can be buffer size for image filtering or can be buffer size to collect frames for processing. |
| MIN_OBJECT_WIDTH          | read / write | Minimum object width to be detected, pixels. To be detected object's width must be >= MIN_OBJECT_WIDTH. |
| MAX_OBJECT_WIDTH          | read / write | Maximum object width to be detected, pixels. To be detected object's width must be <= MAX_OBJECT_WIDTH. |
| MIN_OBJECT_HEIGHT         | read / write | Minimum object height to be detected, pixels. To be detected object's height must be >= MIN_OBJECT_HEIGHT. |
| MAX_OBJECT_HEIGHT         | read / write | Maximum object height to be detected, pixels. To be detected object's height must be <= MAX_OBJECT_HEIGHT. |
| MIN_X_SPEED               | read / write | Minimum object's horizontal speed to be detected, pixels/frame. To be detected object's horizontal speed must be >= MIN_X_SPEED. |
| MAX_X_SPEED               | read / write | Maximum object's horizontal speed to be detected, pixels/frame. To be detected object's horizontal speed must be <= MAX_X_SPEED. |
| MIN_Y_SPEED               | read / write | Minimum object's vertical speed to be detected, pixels/frame. To be detected object's vertical speed must be >= MIN_Y_SPEED. |
| MAX_Y_SPEED               | read / write | Maximum object's vertical speed to be detected, pixels/frame. To be detected object's vertical speed must be <= MAX_Y_SPEED. |
| MIN_DETECTION_PROBABILITY | read / write | Probability threshold from 0 to 1. To be detected object detection probability must be >= MIN_DETECTION_PROBABILITY. For example: neural networks for each detection result calculates detection probability from 0 to 1. MIN_DETECTION_PROBABILITY parameters used to filter detection results. |
| X_DETECTION_CRITERIA      | read / write | Horizontal track detection criteria, frames. By default shows how many frames the objects must move in any(+/-) horizontal direction to be detected. |
| Y_DETECTION_CRITERIA      | read / write | Vertical track detection criteria, frames. By default shows how many frames the objects must move in any(+/-) vertical direction to be detected. |
| RESET_CRITERIA            | read / write | Track reset criteria, frames. By default shows how many frames the objects should be not detected to be excluded from results. |
| SENSITIVITY               | read / write | Detection sensitivity. Depends on implementation. Default from 0 to 1. |
| SCALE_FACTOR              | read / write | Frame scaling factor for processing purposes. Reduce the image size by scaleFactor times horizontally and vertically for faster processing. |
| NUM_THREADS               | read / write | Num threads. Number of threads for parallel computing.       |
| PROCESSING_TIME_MCS       | read only    | Processing time of last frame in microseconds.               |
| TYPE                      | read / write | Algorithm / backend type. Depends on implementation.         |
| MODE                      | read / write | Mode. Default: 0 - Off, 1 - On.                              |
| CUSTOM_1                  | read / write | Custom parameter. Depends on implementation.                 |
| CUSTOM_2                  | read / write | Custom parameter. Depends on implementation.                 |
| CUSTOM_3                  | read / write | Custom parameter. Depends on implementation.                 |



## Object structure

**Object** structure used to describe detected object. Object structure declared in **ObjectDetector.h** file. Structure declaration:

```cpp
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
```

**Table 4** - Object structure fields description.

| Field   | Type  | Description                                                  |
| ------- | ----- | ------------------------------------------------------------ |
| id      | int   | Object ID. Must be unique for particular object. Object detector must assign unique ID for each detected object. This is necessary for control algorithms to distinguish different objects from frame to frame. |
| frameId | int   | Frame ID. Must be the same as frame ID of processed video frame. |
| type    | int   | Object type. Depends on implementation. For example detection neural networks can detect multiple type of objects. |
| width   | int   | Object rectangle width, pixels. Must be MIN_OBJECT_WIDTH <= width <= MAX_OBJECT_WIDTH (see [ObjectDetectorParam](#objectdetectorparam-enum) enum description). |
| height  | int   | Object rectangle height, pixels. Must be MIN_OBJECT_HEIGHT <= height <= MAX_OBJECT_HEIGHT (see [ObjectDetectorParam](#objectdetectorparam-enum) enum description). |
| x       | int   | Object rectangle top-left horizontal coordinate, pixels.     |
| y       | int   | Object rectangle top-left vertical coordinate, pixels.       |
| vX      | float | Horizontal component of object velocity, +-pixels/frame. if it possible object detector should estimate object velocity on video frames. |
| vY      | float | Vertical component of object velocity, +-pixels/frame. if it possible object detector should estimate object velocity on video frames. |
| p       | float | Detection probability from 0 (minimum) to 1 (maximum). Must be >= MIN_DETECTION_PROBABILITY (see [ObjectDetectorParam](#objectdetectorparam-enum) enum description). |



# ObjectDetectorParams class description



## ObjectDetectorParams class declaration

**ObjectDetectorParams** class used for object detector initialization (**initObjectDetector(...)** method) or to get all actual params (**getParams()** method). Also **ObjectDetectorParams** provides structure to write/read params from JSON files (**JSON_READABLE** macro, see [ConfigReader](https://rapidpixel.constantrobotics.com/docs/Service/ConfigReader.html) class description) and provide methods to encode (serialize) and decode (deserialize) params. Class declaration:

```cpp
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

    /// Default constructor.
    ObjectDetectorParams() = default;

    /// Copy constructor.
    ObjectDetectorParams(const ObjectDetectorParams& src) = default;

    /// Copy operator.
    ObjectDetectorParams& operator= (const ObjectDetectorParams& src) = default;

    /// Encode (serialize) params.
    bool encode(uint8_t* data, int bufferSize, int& size,
                ObjectDetectorParamsMask* mask = nullptr);

    /// Decode (deserialize) params.
    bool decode(uint8_t* data, int dataSize);
};
```

**Table 5** - ObjectDetectorParams class fields description.

| Field                   | Type        | Description                                                  |
| ----------------------- | ----------- | ------------------------------------------------------------ |
| initString              | string      | Init string. Depends on implementation.                      |
| logMode                 | int         | Logging mode. Values: 0 - Disable, 1 - Only file, 2 - Only terminal, 3 - File and terminal. |
| frameBufferSize         | int         | Frame buffer size. Depends on implementation. It can be buffer size for image filtering or can be buffer size to collect frames for processing. |
| minObjectWidth          | int         | Minimum object width to be detected, pixels. To be detected object's width must be >= minObjectWidth. |
| maxObjectWidth          | int         | Maximum object width to be detected, pixels. To be detected object's width must be <= maxObjectWidth. |
| minObjectHeight         | int         | Minimum object height to be detected, pixels. To be detected object's height must be >= minObjectHeight. |
| maxObjectHeight         | int         | Maximum object height to be detected, pixels. To be detected object's height must be <= maxObjectHeight. |
| minXSpeed               | float       | Minimum object's horizontal speed to be detected, pixels/frame. To be detected object's horizontal speed must be >= minXSpeed. |
| maxXSpeed               | float       | Maximum object's horizontal speed to be detected, pixels/frame. To be detected object's horizontal speed must be <= maxXSpeed. |
| minYSpeed               | float       | Minimum object's vertical speed to be detected, pixels/frame. To be detected object's vertical speed must be >= minYSpeed. |
| maxYSpeed               | float       | Maximum object's vertical speed to be detected, pixels/frame. To be detected object's vertical speed must be <= maxYSpeed. |
| minDetectionProbability | float       | Probability threshold from 0 to 1. To be detected object detection probability must be >= minDetectionProbability. For example: neural networks for each detection result calculates detection probability from 0 to 1. minDetectionProbability parameters used to filter detection results. |
| xDetectionCriteria      | int         | Horizontal track detection criteria, frames. By default shows how many frames the objects must move in any(+/-) horizontal direction to be detected. |
| yDetectionCriteria      | int         | Vertical track detection criteria, frames. By default shows how many frames the objects must move in any(+/-) vertical direction to be detected. |
| resetCriteria           | int         | Track reset criteria, frames. By default shows how many frames the objects should be not detected to be excluded from results. |
| sensitivity             | float       | Detection sensitivity. Depends on implementation. Default from 0 to 1. |
| scaleFactor             | int         | Frame scaling factor for processing purposes. Reduce the image size by scaleFactor times horizontally and vertically for faster processing. |
| numThreads              | int         | Num threads. Number of threads for parallel computing.       |
| processingTimeMcs       | int         | Processing time of last frame in microseconds.               |
| type                    | int         | Algorithm / backend type. Depends on implementation.         |
| enable                  | bool        | Mode: false - Off, true - On.                                |
| custom1                 | float       | Custom parameter. Depends on implementation.                 |
| custom2                 | float       | Custom parameter. Depends on implementation.                 |
| custom3                 | float       | Custom parameter. Depends on implementation.                 |
| objects                 | std::vector | List of detected objects.                                    |
| classNames              | std::vector | A list of object class names used in detectors that recognize different object classes. Detected objects have an attribute called 'type.' If a detector doesn't support object class recognition or can't determine the object type, the 'type' field must be set to 0. Otherwise, the 'type'  should correspond to the ordinal number of the class name from the 'classNames' list (if the list was set in params), starting from 1 (where the first element in the list has 'type == 1'). The names are serialized by **encode(...)** as null terminated strings: a name must not contain the null character and an empty name is allowed only as the last element of the list. **decode(...)** returns the list without that empty name. |

The values of the fields are transported by the class (JSON, serialization) and are not checked against any range: the object detector implementation must validate them before use.

**Note:** *ObjectDetectorParams class fields listed in Table 5 **must** reflect params set/get by methods setParam(...) and getParam(...).*



## Serialize object detector params

[ObjectDetectorParams](#objectdetectorparams-class-description) class provides method **encode(...)** to serialize object detector params (fields of [ObjectDetectorParams](#objectdetectorparams-class-description) class, see Table 5). Serialization of object detector params necessary in case when you need to send params via communication channels. Method provides options to exclude particular parameters from serialization. To do this method inserts binary mask (4 bytes) where each bit represents particular parameter and **decode(...)** method recognizes it. Method declaration:

```cpp
bool encode(uint8_t* data, int bufferSize, int& size, ObjectDetectorParamsMask* mask = nullptr);
```

| Parameter      | Value                                                        |
| -------------- | ------------------------------------------------------------ |
| data           | Pointer to data buffer. Must not be nullptr. The pointer must be valid for **bufferSize** bytes. |
| bufferSize     | Size of data buffer, bytes. The buffer must hold all selected fields except the objects, the object counter, the init string and all class names with their null terminators. The size of the data with all fields selected, without objects, with an empty init string and with the default list of class names is 102 bytes. If the data buffer size is not large enough to serialize all detected objects (40 bytes per object), only the objects which fit into the buffer are included in the data. |
| size           | Size of encoded data. Set to 0 if the method returns FALSE.  |
| mask           | Parameters mask - pointer to **ObjectDetectorParamsMask** structure. **ObjectDetectorParamsMask** (declared in ObjectDetector.h file) determines flags for each field (parameter) declared in [ObjectDetectorParams](#objectdetectorparams-class-description) class. If the user wants to exclude any parameters from serialization, he can put a pointer to the mask. If the user wants to exclude a particular parameter from serialization, he should set the corresponding flag in the ObjectDetectorParamsMask structure. If the pointer is nullptr all fields are serialized. |

**Returns:** TRUE if params serialized or FALSE if not (data is nullptr, the buffer is too small to hold the selected fields, the init string or a class name contains the null character, an empty class name is not the last one in the list). The method does not write outside of the buffer. If FALSE is returned the content of the buffer is not defined.

**ObjectDetectorParamsMask** structure declaration:

```cpp
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
```

Example without parameters mask:

```cpp
// Prepare params.
rapidpixel::ObjectDetectorParams in;
in.logMode = 2;
in.classNames = {"apple", "banana", "orange", "pineapple", "strawberry",
                 "watermelon", "lemon", "peach", "pear", "plum"};
for (int i = 0; i < 5; ++i)
{
    rapidpixel::Object obj;
    obj.id = i + 1;
    obj.type = i + 1;
    obj.width = 20 + i;
    obj.height = 30 + i;
    obj.x = 100 * i;
    obj.y = 50 * i;
    obj.vX = 1.5f * static_cast<float>(i);
    obj.vY = 0.5f * static_cast<float>(i);
    obj.p = 0.9f;
    in.objects.push_back(obj);
}

// Encode data.
uint8_t data[2048];
int size = 0;
if (!in.encode(data, 2048, size))
{
    std::cout << "Can't encode params" << std::endl;
    return false;
}
std::cout << "Encoded data size: " << size << " bytes" << std::endl;
```

Example with parameters mask:

```cpp
// Prepare params.
rapidpixel::ObjectDetectorParams in;
in.logMode = 2;
in.classNames = {"apple", "banana", "orange", "pineapple", "strawberry",
                 "watermelon", "lemon", "peach", "pear", "plum"};
for (int i = 0; i < 5; ++i)
{
    rapidpixel::Object obj;
    obj.id = i + 1;
    obj.type = i + 1;
    obj.width = 20 + i;
    obj.height = 30 + i;
    obj.x = 100 * i;
    obj.y = 50 * i;
    obj.vX = 1.5f * static_cast<float>(i);
    obj.vY = 0.5f * static_cast<float>(i);
    obj.p = 0.9f;
    in.objects.push_back(obj);
}

// Prepare mask.
rapidpixel::ObjectDetectorParamsMask mask;
mask.logMode = false; // Exclude parameter from serialization.

// Encode data.
uint8_t data[2048];
int size = 0;
if (!in.encode(data, 2048, size, &mask))
{
    std::cout << "Can't encode params" << std::endl;
    return false;
}
std::cout << "Encoded data size: " << size << " bytes" << std::endl;
```



## Deserialize object detector params

[ObjectDetectorParams](#objectdetectorparams-class-description) class provides method **decode(...)** to deserialize params (fields of [ObjectDetectorParams](#objectdetectorparams-class-description) class, see Table 5). Deserialization of params necessary in case when you need to receive params via communication channels. Method automatically recognizes which parameters were serialized by **encode(...)** method. The method checks the data type, the version (MAJOR.MINOR), the size of the data before reading each field, the number of objects against the size of the data and that the init string and the class names are null terminated. The data must not have bytes after the last field. Fields which are not serialized (excluded by the mask) are set to zero (empty for the init string, the objects and the class names). Values of the fields are not checked against any range. Method declaration:

```cpp
bool decode(uint8_t* data, int dataSize);
```

| Parameter | Value                                                        |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to encode data buffer. Must not be nullptr. The pointer must be valid for **dataSize** bytes. |
| dataSize  | Data size, bytes.                                            |

**Returns:** TRUE if data decoded (deserialized) or FALSE if not (data is nullptr, the data is too short, wrong data type or version, wrong number of objects, the string is not terminated, bytes after the last field). If FALSE is returned the object is not changed.

Example:

```cpp
// Prepare params.
rapidpixel::ObjectDetectorParams in;
in.logMode = 2;
in.classNames = {"apple", "banana", "orange", "pineapple", "strawberry",
                 "watermelon", "lemon", "peach", "pear", "plum"};
for (int i = 0; i < 5; ++i)
{
    rapidpixel::Object obj;
    obj.id = i + 1;
    obj.type = i + 1;
    obj.width = 20 + i;
    obj.height = 30 + i;
    obj.x = 100 * i;
    obj.y = 50 * i;
    obj.vX = 1.5f * static_cast<float>(i);
    obj.vY = 0.5f * static_cast<float>(i);
    obj.p = 0.9f;
    in.objects.push_back(obj);
}

// Encode data.
uint8_t data[2048];
int size = 0;
if (!in.encode(data, 2048, size))
{
    std::cout << "Can't encode params" << std::endl;
    return false;
}
std::cout << "Encoded data size: " << size << " bytes" << std::endl;

// Decode data.
rapidpixel::ObjectDetectorParams out;
if (!out.decode(data, size))
{
    std::cout << "Can't decode data" << std::endl;
    return false;
}
```



## Read params from JSON file and write to JSON file

**ObjectDetector** library depends on **ConfigReader** library which provides method to read params from JSON file and to write params to JSON file. The list of objects (**objects**) and the processing time (**processingTimeMcs**) are not stored in JSON. All other fields of [ObjectDetectorParams](#objectdetectorparams-class-description) class are required in the JSON file: the **get(...)** method of **ConfigReader** returns false and does not change the object if a field is missing or has a wrong type. Example of writing and reading params to JSON file:

```cpp
// Prepare params.
rapidpixel::ObjectDetectorParams in;
in.initString = "init";
in.logMode = 2;
in.minObjectWidth = 8;
in.classNames = {"apple", "banana", "orange", "pineapple", "strawberry",
                 "watermelon", "lemon", "peach", "pear", "plum"};

// Write params to file.
rapidpixel::ConfigReader inConfig;
if (!inConfig.set(in, "ObjectDetectorParams") ||
    !inConfig.writeToFile("ObjectDetectorParams.json"))
{
    std::cout << "Can't write params to file" << std::endl;
    return false;
}

// Read params from file.
rapidpixel::ConfigReader outConfig;
if (!outConfig.readFromFile("ObjectDetectorParams.json"))
{
    std::cout << "Can't open config file" << std::endl;
    return false;
}

rapidpixel::ObjectDetectorParams out;
if (!outConfig.get(out, "ObjectDetectorParams"))
{
    std::cout << "Can't read params from file" << std::endl;
    return false;
}
```

**ObjectDetectorParams.json** will look like:

```json
{
    "ObjectDetectorParams": {
        "initString": "init",
        "logMode": 2,
        "frameBufferSize": 1,
        "minObjectWidth": 8,
        "maxObjectWidth": 128,
        "minObjectHeight": 4,
        "maxObjectHeight": 128,
        "minXSpeed": 0.0,
        "maxXSpeed": 30.0,
        "minYSpeed": 0.0,
        "maxYSpeed": 30.0,
        "minDetectionProbability": 0.5,
        "xDetectionCriteria": 1,
        "yDetectionCriteria": 1,
        "resetCriteria": 1,
        "sensitivity": 0.039999999105930328,
        "scaleFactor": 1,
        "numThreads": 1,
        "type": 0,
        "enable": true,
        "custom1": 0.0,
        "custom2": 0.0,
        "custom3": 0.0,
        "classNames": [
            "apple",
            "banana",
            "orange",
            "pineapple",
            "strawberry",
            "watermelon",
            "lemon",
            "peach",
            "pear",
            "plum"
        ]
    }
}
```



# Build and connect to your project



## Build stand alone

The repository does not contain sub-repositories. The libraries **ObjectDetector** depends on (**Frame** and **ConfigReader**) must be cloned into the same folder as **ObjectDetector**. The folder structure must be:

```bash
<work folder>
    ObjectDetector
    Frame
    ConfigReader
```

Typical commands to build the library, the test application and the example (use the URLs of the repositories provided to you):

```bash
git clone <ObjectDetector repository URL>
git clone <Frame repository URL>
git clone <ConfigReader repository URL>
cd ObjectDetector
mkdir build
cd build
cmake ..
make
```

On Windows use `cmake --build . --config Release` instead of `make`. If **ObjectDetector** is the top-level CMake project, the main CMakeLists.txt file adds the folders ../Frame and ../ConfigReader (in this order) before the library itself. If a folder does not exist CMake stops with an error.



## CMake options

**Table 6** - CMake options of the repository. In the options below ${PARENT} is the prefix defined by the top-level project (empty if **ObjectDetector** is the top-level project).

| Option                              | Default                                           | Description                                                  |
| ----------------------------------- | ------------------------------------------------- | ------------------------------------------------------------ |
| ${PARENT}_OBJECT_DETECTOR           | ON                                                | Build the library.                                           |
| ${PARENT}_OBJECT_DETECTOR_TEST      | ON (stand alone), OFF (included in other project) | Build the test application.                                  |
| ${PARENT}_OBJECT_DETECTOR_EXAMPLE   | ON (stand alone), OFF (included in other project) | Build the example.                                           |
| ${PARENT}_SUBMODULE_CACHE_OVERWRITE | ON                                                | Forced rewriting of the options above in the CMake cache. The top-level project sets it to OFF when it includes **ObjectDetector** as a sub-repository. |
| CMAKE_BUILD_TYPE                    | Release (if not specified)                        | Build type.                                                  |



## Connect to your project

If you want connect **ObjectDetector** library to your CMake project as source code you can make follow. For example, if your repository has structure:

```bash
CMakeLists.txt
src
    CMakeList.txt
    yourLib.h
    yourLib.cpp
```

You can add the repository **ObjectDetector** and the repositories of the libraries it depends on as sub-repositories by commands:

```bash
cd <your repository folder>
git submodule add <Frame repository URL> 3rdparty/Frame
git submodule add <ConfigReader repository URL> 3rdparty/ConfigReader
git submodule add <ObjectDetector repository URL> 3rdparty/ObjectDetector
git submodule update --init --recursive
```

In your repository folder the folder **3rdparty** will be created which contains the files of **Frame**, **ConfigReader** and **ObjectDetector** repositories. The structure of your repository:

```bash
CMakeLists.txt
src
    CMakeList.txt
    yourLib.h
    yourLib.cpp
3rdparty
    Frame
    ConfigReader
    ObjectDetector
```

**ObjectDetector** does not add **Frame** and **ConfigReader** when it is included in another project: the project must add them. Create CMakeLists.txt file in **3rdparty** folder. CMakeLists.txt adds the dependencies in dependency order and then **ObjectDetector**. CMakeLists.txt should contain:

```cmake
cmake_minimum_required(VERSION 3.13)

################################################################################
## 3RD-PARTY
## dependencies for the project
################################################################################
project(3rdparty LANGUAGES CXX)

################################################################################
## SETTINGS
## basic 3rd-party settings before use
################################################################################
# To inherit the top-level architecture when the project is used as a submodule.
SET(PARENT ${PARENT}_YOUR_PROJECT_3RDPARTY)
# Disable self-overwriting of parameters inside included subdirectories.
SET(${PARENT}_SUBMODULE_CACHE_OVERWRITE OFF CACHE BOOL "" FORCE)

################################################################################
## CONFIGURATION
## 3rd-party submodules configuration
################################################################################
SET(${PARENT}_SUBMODULE_FRAME                           ON  CACHE BOOL "" FORCE)
if (${PARENT}_SUBMODULE_FRAME)
    SET(${PARENT}_FRAME                                 ON  CACHE BOOL "" FORCE)
    SET(${PARENT}_FRAME_TEST                            OFF CACHE BOOL "" FORCE)
endif()

SET(${PARENT}_SUBMODULE_CONFIG_READER                   ON  CACHE BOOL "" FORCE)
if (${PARENT}_SUBMODULE_CONFIG_READER)
    SET(${PARENT}_CONFIG_READER                         ON  CACHE BOOL "" FORCE)
    SET(${PARENT}_CONFIG_READER_EXAMPLES                OFF CACHE BOOL "" FORCE)
    SET(${PARENT}_CONFIG_READER_TESTS                   OFF CACHE BOOL "" FORCE)
endif()

SET(${PARENT}_SUBMODULE_OBJECT_DETECTOR                 ON  CACHE BOOL "" FORCE)
if (${PARENT}_SUBMODULE_OBJECT_DETECTOR)
    SET(${PARENT}_OBJECT_DETECTOR                       ON  CACHE BOOL "" FORCE)
    SET(${PARENT}_OBJECT_DETECTOR_TEST                  OFF CACHE BOOL "" FORCE)
    SET(${PARENT}_OBJECT_DETECTOR_EXAMPLE               OFF CACHE BOOL "" FORCE)
endif()

################################################################################
## INCLUDING SUBDIRECTORIES
## Adding subdirectories according to the 3rd-party configuration
################################################################################
if (${PARENT}_SUBMODULE_FRAME)
    add_subdirectory(Frame)
endif()

if (${PARENT}_SUBMODULE_CONFIG_READER)
    add_subdirectory(ConfigReader)
endif()

if (${PARENT}_SUBMODULE_OBJECT_DETECTOR)
    add_subdirectory(ObjectDetector)
endif()
```

File **3rdparty/CMakeLists.txt** adds the folders **Frame**, **ConfigReader** and **ObjectDetector** to your project and excludes test application and example (**ObjectDetector** class test application and example) from compiling (by default test applications and example are excluded from compiling if **ObjectDetector** is included as sub-repository). The structure of your repository:

```bash
CMakeLists.txt
src
    CMakeList.txt
    yourLib.h
    yourLib.cpp
3rdparty
    CMakeLists.txt
    Frame
    ConfigReader
    ObjectDetector
```

Next you need include folder 3rdparty in main **CMakeLists.txt** file of your repository. Add string at the end of your main **CMakeLists.txt**:

```cmake
add_subdirectory(3rdparty)
```

Next you have to include ObjectDetector library in your **src/CMakeLists.txt** file:

```cmake
target_link_libraries(${PROJECT_NAME} ObjectDetector)
```

If **Frame** or **ConfigReader** is not added by your project, the build stops with the compile error "Frame.h: No such file or directory" ("ConfigReader.h: No such file or directory").

Done!



# Portability

The encoded data of [ObjectDetectorParams](#objectdetectorparams-class-description) class and of the commands stores int and float values as raw 4 bytes values in the byte order of the host. The sizes of int and float (4 bytes) are checked at compile time. The library does not use SIMD instructions, threads, operating system interfaces or environment variables. The library was built and its test application was run with MSVC 2022 (x64, Windows), GCC 12 and 13 and Clang 15 and 18 (x86-64, Linux) and with GCC 13 for aarch64 (Linux, the test application was run under QEMU).



# How to create a custom implementation

The **ObjectDetector** class provides only an interface, data structures, and methods for encoding and decoding commands and params. To create your own implementation of the object detector, you must include the ObjectDetector repository in your project (see [Build and connect to your project](#build-and-connect-to-your-project) section). The catalogue **example** (see [Library files](#library-files) section) includes an example of the design of the custom object detector. You must implement all the methods of the ObjectDetector interface class. The example validates the values which come from outside (params, commands) and provides thread-safe methods: the params are protected by a mutex, so the class does not have copy and move operations. Custom object detector class declaration:

```cpp
class CustomObjectDetector : public ObjectDetector
{
public:

    /// Class constructor.
    CustomObjectDetector();

    /// Class destructor.
    ~CustomObjectDetector() override;

    /// Copy and move operations are deleted: the class owns a mutex.
    CustomObjectDetector(const CustomObjectDetector&) = delete;
    CustomObjectDetector(CustomObjectDetector&&) = delete;
    CustomObjectDetector& operator= (const CustomObjectDetector&) = delete;
    CustomObjectDetector& operator= (CustomObjectDetector&&) = delete;

    /// Get string of current class version.
    static std::string getVersion();

    /// Init object detector.
    bool initObjectDetector(ObjectDetectorParams& params) override;

    /// Set object detector param.
    bool setParam(ObjectDetectorParam id, float value) override;

    /// Get object detector param value.
    float getParam(ObjectDetectorParam id) override;

    /// Get object detector params structure.
    void getParams(ObjectDetectorParams& params) override;

    /// Get list of objects.
    std::vector<Object> getObjects() override;

    /// Execute command.
    bool executeCommand(ObjectDetectorCommand id) override;

    /// Perform detection.
    bool detect(rapidpixel::Frame& frame) override;

    /// Set detection mask.
    bool setMask(rapidpixel::Frame mask) override;

    /// Decode and execute command.
    bool decodeAndExecuteCommand(uint8_t* data, int size) override;

private:

    /// Object detector params. Protected by m_paramsMutex.
    ObjectDetectorParams m_params;
    /// Mutex to protect m_params.
    std::mutex m_paramsMutex;
};
```
