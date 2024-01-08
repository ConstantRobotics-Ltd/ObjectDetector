#include "ObjectDetector.h"
#include "ObjectDetectorVersion.h"
#include <string>



cr::detector::ObjectDetector::~ObjectDetector()
{
    
}



std::string cr::detector::ObjectDetector::getVersion()
{
    return OBJECT_DETECTOR_VERSION;
}



cr::detector::ObjectDetectorParams &cr::detector::ObjectDetectorParams::operator= (const cr::detector::ObjectDetectorParams &src)
{
    // Check yourself.
    if (this == &src)
    {
        return *this;
    }  

    // Copy params.
    logMode = src.logMode;
    frameBufferSize = src.frameBufferSize;
    minObjectWidth = src.minObjectWidth;
    maxObjectWidth = src.maxObjectWidth;
    minObjectHeight = src.minObjectHeight;
    maxObjectHeight = src.maxObjectHeight;
    minXSpeed = src.minXSpeed;
    maxXSpeed = src.maxXSpeed;
    minYSpeed = src.minYSpeed;
    maxYSpeed = src.maxYSpeed;
    xDetectionCriteria = src.xDetectionCriteria;
    yDetectionCriteria = src.yDetectionCriteria;
    resetCriteria = src.resetCriteria;
    sensitivity = src.sensitivity;
    scaleFactor = src.scaleFactor;
    numThreads = src.numThreads;
    processingTimeMks = src.processingTimeMks;
    type = src.type;
    enable = src.enable;
    custom1 = src.custom1;
    custom2 = src.custom2;
    custom3 = src.custom3;
    objects = src.objects;
    minDetectionProbability = src.minDetectionProbability;
    initString = src.initString;
    classNames = src.classNames;

    return *this;
}



bool cr::detector::ObjectDetectorParams::encode(
    uint8_t* data, int bufferSize, int& size,
    ObjectDetectorParamsMask* mask)
{
    // Check data buffer size.
    if (bufferSize < 99)
    {
        size = 0;
        return false;
    }

    // Encode version.
    int pos = 0;
    data[pos] = 0x02; pos += 1;
    data[pos] = OBJECT_DETECTOR_MAJOR_VERSION; pos += 1;
    data[pos] = OBJECT_DETECTOR_MINOR_VERSION; pos += 1;

    if (mask == nullptr)
    {
        // Prepare mask.
        data[pos] = 0xFF; pos += 1;
        data[pos] = 0xFF; pos += 1;
        data[pos] = 0xFF; pos += 1;
        data[pos] = 0xFF; pos += 1;

        // Encode data.
        memcpy(&data[pos], &logMode, 4); pos += 4;
        memcpy(&data[pos], &frameBufferSize, 4); pos += 4;
        memcpy(&data[pos], &minObjectWidth, 4); pos += 4;
        memcpy(&data[pos], &maxObjectWidth, 4); pos += 4;
        memcpy(&data[pos], &minObjectHeight, 4); pos += 4;
        memcpy(&data[pos], &maxObjectHeight, 4); pos += 4;
        memcpy(&data[pos], &minXSpeed, 4); pos += 4;
        memcpy(&data[pos], &maxXSpeed, 4); pos += 4;

        memcpy(&data[pos], &minYSpeed, 4); pos += 4;
        memcpy(&data[pos], &maxYSpeed, 4); pos += 4;
        memcpy(&data[pos], &xDetectionCriteria, 4); pos += 4;
        memcpy(&data[pos], &yDetectionCriteria, 4); pos += 4;
        memcpy(&data[pos], &resetCriteria, 4); pos += 4;
        memcpy(&data[pos], &sensitivity, 4); pos += 4;
        memcpy(&data[pos], &scaleFactor, 4); pos += 4;
        memcpy(&data[pos], &numThreads, 4); pos += 4;

        memcpy(&data[pos], &processingTimeMks, 4); pos += 4;
        memcpy(&data[pos], &type, 4); pos += 4;
        data[pos] = enable ? 0x01 : 0x00; pos += 1;
        memcpy(&data[pos], &custom1, 4); pos += 4;
        memcpy(&data[pos], &custom2, 4); pos += 4;
        memcpy(&data[pos], &custom3, 4); pos += 4;
        memcpy(&data[pos], &minDetectionProbability, 4); pos += 4;
        int numObjects = (int)objects.size();
        memcpy(&data[pos], &numObjects, 4); pos += 4;

        // Check how much object we can put into buffer.
        int restObjects = (bufferSize - pos) / 40;
        if (restObjects < 0)
            restObjects = 0;
        if (restObjects < numObjects)
        {
            numObjects = restObjects;
            memcpy(&data[pos - 4], &numObjects, 4); pos += 4;
        }

        for (int i = 0; i < numObjects; ++i)
        {
            memcpy(&data[pos], &objects[i].id, 4); pos += 4;
            memcpy(&data[pos], &objects[i].frameId, 4); pos += 4;
            memcpy(&data[pos], &objects[i].type, 4); pos += 4;
            memcpy(&data[pos], &objects[i].width, 4); pos += 4;
            memcpy(&data[pos], &objects[i].height, 4); pos += 4;
            memcpy(&data[pos], &objects[i].x, 4); pos += 4;
            memcpy(&data[pos], &objects[i].y, 4); pos += 4;
            memcpy(&data[pos], &objects[i].vX, 4); pos += 4;
            memcpy(&data[pos], &objects[i].vY, 4); pos += 4;
            memcpy(&data[pos], &objects[i].p, 4); pos += 4;
        }

        // Add size + 1 of init string to data to recognize it on decode with null terminator.
        memcpy(&data[pos], &initString[0], initString.size() + 1);
        pos += static_cast<int>(initString.size()) + 1;

        // Iterate through all of the class names and add them to the data.
        for (int i = 0; i < classNames.size(); ++i)
        {
            // Add size + 1 of init string to data to recognize it on decode with null terminator.
            memcpy(&data[pos], &classNames[i][0], classNames[i].size() + 1);
            pos += static_cast<int>(classNames[i].size()) + 1;
        }
        size = pos;

        return true;
    }

    // Prepare mask.
    data[pos] = 0;
    data[pos] = data[pos] | (mask->logMode ? static_cast<uint8_t>(128) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->frameBufferSize ? static_cast<uint8_t>(64) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->minObjectWidth ? static_cast<uint8_t>(32) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->maxObjectWidth ? static_cast<uint8_t>(16) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->minObjectHeight ? static_cast<uint8_t>(8) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->maxObjectHeight ? static_cast<uint8_t>(4) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->minXSpeed ? static_cast<uint8_t>(2) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->maxXSpeed ? static_cast<uint8_t>(1) : static_cast<uint8_t>(0));
    pos += 1;
    data[pos] = 0;
    data[pos] = data[pos] | (mask->minYSpeed ? static_cast<uint8_t>(128) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->maxYSpeed ? static_cast<uint8_t>(64) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->xDetectionCriteria ? static_cast<uint8_t>(32) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->yDetectionCriteria ? static_cast<uint8_t>(16) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->resetCriteria ? static_cast<uint8_t>(8) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->sensitivity ? static_cast<uint8_t>(4) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->scaleFactor ? static_cast<uint8_t>(2) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->numThreads ? static_cast<uint8_t>(1) : static_cast<uint8_t>(0));
    pos += 1;
    data[pos] = 0;
    data[pos] = data[pos] | (mask->processingTimeMks ? static_cast<uint8_t>(128) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->type ? static_cast<uint8_t>(64) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->enable ? static_cast<uint8_t>(32) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->custom1 ? static_cast<uint8_t>(16) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->custom2 ? static_cast<uint8_t>(8) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->custom3 ? static_cast<uint8_t>(4) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->minDetectionProbability ? static_cast<uint8_t>(2) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->objects ? static_cast<uint8_t>(1) : static_cast<uint8_t>(0));
    pos += 1;
    data[pos] = 0;
    data[pos] = data[pos] | (mask->initString ? static_cast<uint8_t>(128) : static_cast<uint8_t>(0));
    data[pos] = data[pos] | (mask->classNames ? static_cast<uint8_t>(64) : static_cast<uint8_t>(0));
    pos += 1;

    if (mask->logMode)
    {
        memcpy(&data[pos], &logMode, 4); pos += 4;
    }
    if (mask->frameBufferSize)
    {
        memcpy(&data[pos], &frameBufferSize, 4); pos += 4;
    }
    if (mask->minObjectWidth)
    {
        memcpy(&data[pos], &minObjectWidth, 4); pos += 4;
    }
    if (mask->maxObjectWidth)
    {
        memcpy(&data[pos], &maxObjectWidth, 4); pos += 4;
    }
    if (mask->minObjectHeight)
    {
        memcpy(&data[pos], &minObjectHeight, 4); pos += 4;
    }
    if (mask->maxObjectHeight)
    {
        memcpy(&data[pos], &maxObjectHeight, 4); pos += 4;
    }
    if (mask->minXSpeed)
    {
        memcpy(&data[pos], &minXSpeed, 4); pos += 4;
    }
    if (mask->maxXSpeed)
    {
        memcpy(&data[pos], &maxXSpeed, 4); pos += 4;
    }


    if (mask->minYSpeed)
    {
        memcpy(&data[pos], &minYSpeed, 4); pos += 4;
    }
    if (mask->maxYSpeed)
    {
        memcpy(&data[pos], &maxYSpeed, 4); pos += 4;
    }
    if (mask->xDetectionCriteria)
    {
        memcpy(&data[pos], &xDetectionCriteria, 4); pos += 4;
    }
    if (mask->yDetectionCriteria)
    {
        memcpy(&data[pos], &yDetectionCriteria, 4); pos += 4;
    }
    if (mask->resetCriteria)
    {
        memcpy(&data[pos], &resetCriteria, 4); pos += 4;
    }
    if (mask->sensitivity)
    {
        memcpy(&data[pos], &sensitivity, 4); pos += 4;
    }
    if (mask->scaleFactor)
    {
        memcpy(&data[pos], &scaleFactor, 4); pos += 4;
    }
    if (mask->numThreads)
    {
        memcpy(&data[pos], &numThreads, 4); pos += 4;
    }


    if (mask->processingTimeMks)
    {
        memcpy(&data[pos], &processingTimeMks, 4); pos += 4;
    }
    if (mask->type)
    {
        memcpy(&data[pos], &type, 4); pos += 4;
    }
    if (mask->enable)
    {
        data[pos] = enable == true ? 0x01 : 0x00; pos += 1;
    }
    if (mask->custom1)
    {
        memcpy(&data[pos], &custom1, 4); pos += 4;
    }
    if (mask->custom2)
    {
        memcpy(&data[pos], &custom2, 4); pos += 4;
    }
    if (mask->custom3)
    {
        memcpy(&data[pos], &custom3, 4); pos += 4;
    }
    if (mask->minDetectionProbability)
    {
        memcpy(&data[pos], &minDetectionProbability, 4); pos += 4;
    }
    if (mask->objects)
    {
        int numObjects = (int)objects.size();
        memcpy(&data[pos], &numObjects, 4); pos += 4;

        // Check how much object we can put into buffer.
        int restObjects = (bufferSize - pos) / 40;
        if (restObjects < 0)
            restObjects = 0;
        if (restObjects < numObjects)
        {
            numObjects = restObjects;
            memcpy(&data[pos - 4], &numObjects, 4); pos += 4;
        }

        for (int i = 0; i < numObjects; ++i)
        {
            memcpy(&data[pos], &objects[i].id, 4); pos += 4;
            memcpy(&data[pos], &objects[i].frameId, 4); pos += 4;
            memcpy(&data[pos], &objects[i].type, 4); pos += 4;
            memcpy(&data[pos], &objects[i].width, 4); pos += 4;
            memcpy(&data[pos], &objects[i].height, 4); pos += 4;
            memcpy(&data[pos], &objects[i].x, 4); pos += 4;
            memcpy(&data[pos], &objects[i].y, 4); pos += 4;
            memcpy(&data[pos], &objects[i].vX, 4); pos += 4;
            memcpy(&data[pos], &objects[i].vY, 4); pos += 4;
            memcpy(&data[pos], &objects[i].p, 4); pos += 4;
        }
    }


    if (mask->initString)
	{
        // Add size + 1 of init string to data to recognize it on decode with null terminator.
		memcpy(&data[pos], &initString[0], initString.size() + 1);
        pos += static_cast<int>(initString.size()) + 1;
	}
    if (mask->classNames)
    {
        // Iterate through all of the class names and add them to the data.
        for (int i = 0; i < classNames.size(); ++i)
        {
            // Add size + 1 of init string to data to recognize it on decode with null terminator.
            memcpy(&data[pos], &classNames[i][0], classNames[i].size() + 1);
            pos += static_cast<int>(classNames[i].size()) + 1;
        }
    }

    size = pos;

    return true;
}



bool cr::detector::ObjectDetectorParams::decode(uint8_t* data, int dataSize)
{
    // Check data size.
    if (dataSize < 7)
    {
        return false;
    }

    // Check header.
    if (data[0] != 0x02)
    {
        return false;
    }

    // Check version.
    if (data[1] != OBJECT_DETECTOR_MAJOR_VERSION ||
        data[2] != OBJECT_DETECTOR_MINOR_VERSION)
    {
        return false;
    }

    // Decode data.
    int pos = 7;
    if ((data[3] & static_cast<uint8_t>(128)) == static_cast<uint8_t>(128))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&logMode, &data[pos], 4); pos += 4;
    }
    else
    {
        logMode = 0;
    }
    if ((data[3] & static_cast<uint8_t>(64)) == static_cast<uint8_t>(64))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&frameBufferSize, &data[pos], 4); pos += 4;
    }
    else
    {
        frameBufferSize = 0;
    }
    if ((data[3] & static_cast<uint8_t>(32)) == static_cast<uint8_t>(32))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&minObjectWidth, &data[pos], 4); pos += 4;
    }
    else
    {
        minObjectWidth = 0;
    }
    if ((data[3] & static_cast<uint8_t>(16)) == static_cast<uint8_t>(16))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&maxObjectWidth, &data[pos], 4); pos += 4;
    }
    else
    {
        maxObjectWidth = 0;
    }
    if ((data[3] & static_cast<uint8_t>(8)) == static_cast<uint8_t>(8))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&minObjectHeight, &data[pos], 4); pos += 4;
    }
    else
    {
        minObjectHeight = 0;
    }
    if ((data[3] & static_cast<uint8_t>(4)) == static_cast<uint8_t>(4))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&maxObjectHeight, &data[pos], 4); pos += 4;
    }
    else
    {
        maxObjectHeight = 0;
    }
    if ((data[3] & static_cast<uint8_t>(2)) == static_cast<uint8_t>(2))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&minXSpeed, &data[pos], 4); pos += 4;
    }
    else
    {
        minXSpeed = 0.0f;
    }
    if ((data[3] & static_cast<uint8_t>(1)) == static_cast<uint8_t>(1))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&maxXSpeed, &data[pos], 4); pos += 4;
    }
    else
    {
        maxXSpeed = 0.0f;
    }


    if ((data[4] & static_cast<uint8_t>(128)) == static_cast<uint8_t>(128))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&minYSpeed, &data[pos], 4); pos += 4;
    }
    else
    {
        minYSpeed = 0.0f;
    }
    if ((data[4] & static_cast<uint8_t>(64)) == static_cast<uint8_t>(64))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&maxYSpeed, &data[pos], 4); pos += 4;
    }
    else
    {
        maxYSpeed = 0.0f;
    }
    if ((data[4] & static_cast<uint8_t>(32)) == static_cast<uint8_t>(32))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&xDetectionCriteria, &data[pos], 4); pos += 4;
    }
    else
    {
        xDetectionCriteria = 0;
    }
    if ((data[4] & static_cast<uint8_t>(16)) == static_cast<uint8_t>(16))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&yDetectionCriteria, &data[pos], 4); pos += 4;
    }
    else
    {
        yDetectionCriteria = 0;
    }
    if ((data[4] & static_cast<uint8_t>(8)) == static_cast<uint8_t>(8))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&resetCriteria, &data[pos], 4); pos += 4;
    }
    else
    {
        resetCriteria = 0;
    }
    if ((data[4] & static_cast<uint8_t>(4)) == static_cast<uint8_t>(4))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&sensitivity, &data[pos], 4); pos += 4;
    }
    else
    {
        sensitivity = 0.0f;
    }
    if ((data[4] & static_cast<uint8_t>(2)) == static_cast<uint8_t>(2))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&scaleFactor, &data[pos], 4); pos += 4;
    }
    else
    {
        scaleFactor = 0;
    }
    if ((data[4] & static_cast<uint8_t>(1)) == static_cast<uint8_t>(1))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&numThreads, &data[pos], 4); pos += 4;
    }
    else
    {
        numThreads = 0;
    }


    if ((data[5] & static_cast<uint8_t>(128)) == static_cast<uint8_t>(128))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&processingTimeMks, &data[pos], 4); pos += 4;
    }
    else
    {
        processingTimeMks = 0;
    }
    if ((data[5] & static_cast<uint8_t>(64)) == static_cast<uint8_t>(64))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&type, &data[pos], 4); pos += 4;
    }
    else
    {
        type = 0;
    }
    if ((data[5] & static_cast<uint8_t>(32)) == static_cast<uint8_t>(32))
    {
        if (dataSize < pos + 1)
            return false;
        enable = data[pos] == 0x00 ? false : true; pos += 1;
    }
    else
    {
        enable = false;
    }
    if ((data[5] & static_cast<uint8_t>(16)) == static_cast<uint8_t>(16))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&custom1, &data[pos], 4); pos += 4;
    }
    else
    {
        custom1 = 0.0f;
    }
    if ((data[5] & static_cast<uint8_t>(8)) == static_cast<uint8_t>(8))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&custom2, &data[pos], 4); pos += 4;
    }
    else
    {
        custom2 = 0.0f;
    }
    if ((data[5] & static_cast<uint8_t>(4)) == static_cast<uint8_t>(4))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&custom3, &data[pos], 4); pos += 4;
    }
    else
    {
        custom3 = 0.0f;
    }
    if ((data[5] & static_cast<uint8_t>(2)) == static_cast<uint8_t>(2))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&minDetectionProbability, &data[pos], 4); pos += 4;
    }
    else
    {
        minDetectionProbability = 0.0f;
    }
    if ((data[5] & static_cast<uint8_t>(1)) == static_cast<uint8_t>(1))
    {
        int numObjects = 0;
        memcpy(&numObjects, &data[pos], 4); pos += 4;
        objects.clear();
        for (int i = 0; i < numObjects; ++i)
        {
            cr::detector::Object obj;
            memcpy(&obj.id, &data[pos], 4); pos += 4;
            memcpy(&obj.frameId, &data[pos], 4); pos += 4;
            memcpy(&obj.type, &data[pos], 4); pos += 4;
            memcpy(&obj.width, &data[pos], 4); pos += 4;
            memcpy(&obj.height, &data[pos], 4); pos += 4;
            memcpy(&obj.x, &data[pos], 4); pos += 4;
            memcpy(&obj.y, &data[pos], 4); pos += 4;
            memcpy(&obj.vX, &data[pos], 4); pos += 4;
            memcpy(&obj.vY, &data[pos], 4); pos += 4;
            memcpy(&obj.p, &data[pos], 4); pos += 4;
            objects.push_back(obj);
        }
    }
    else
    {
        objects.clear();
    }

    if ((data[6] & static_cast<uint8_t>(128)) == static_cast<uint8_t>(128))
	{
		initString.clear(); 
        // Use strcpy to copy string with null terminator.
        std::array<char, 512> initStringArray;
        std::strcpy(initStringArray.data(), reinterpret_cast<char*>(&data[pos]));
        pos += static_cast<int>(std::strlen(initStringArray.data())) + 1;
        initString = initStringArray.data();
	}
	else
	{
		initString.clear();
	}
    if ((data[6] & static_cast<uint8_t>(64)) == static_cast<uint8_t>(64))
	{
		classNames.clear();
		while (pos < dataSize)
		{
			std::array<char, 512> classNameArray;
			std::strcpy(classNameArray.data(), reinterpret_cast<char*>(&data[pos]));
			pos += static_cast<int>(std::strlen(classNameArray.data())) + 1;
			classNames.push_back(classNameArray.data());
		}
	}
	else
	{
		classNames.clear();
	}

    return true;
}



void cr::detector::ObjectDetector::encodeSetParamCommand(
        uint8_t* data, int& size, ObjectDetectorParam id, float value)
{
    // Fill header.
    data[0] = 0x01;
    data[1] = OBJECT_DETECTOR_MAJOR_VERSION;
    data[2] = OBJECT_DETECTOR_MINOR_VERSION;

    // Fill data.
    int paramId = (int)id;
    memcpy(&data[3], &paramId, 4);
    memcpy(&data[7], &value, 4);
    size = 11;
}



void cr::detector::ObjectDetector::encodeCommand(uint8_t* data,
                                   int& size,
                                   cr::detector::ObjectDetectorCommand id)
{
    // Fill header.
    data[0] = 0x00;
    data[1] = OBJECT_DETECTOR_MAJOR_VERSION;
    data[2] = OBJECT_DETECTOR_MINOR_VERSION;

    // Fill data.
    int commandId = (int)id;
    memcpy(&data[3], &commandId, 4);
    size = 7;
}



int cr::detector::ObjectDetector::decodeCommand(uint8_t* data,
                                  int size,
                                  cr::detector::ObjectDetectorParam& paramId,
                                  cr::detector::ObjectDetectorCommand& commandId,
                                  float& value)
{
    // Check size.
    if (size < 7)
    {
        return -1;
    }

    // Check version.
    if (data[1] != OBJECT_DETECTOR_MAJOR_VERSION ||
        data[2] != OBJECT_DETECTOR_MINOR_VERSION)
    {
        return -1;
    }

    // Extract data.
    int id = 0;
    memcpy(&id, &data[3], 4);
    value = 0.0f;

    // Check command type.
    if (data[0] == 0x00)
    {
        commandId = (ObjectDetectorCommand)id;
        return 0;
    }
    else if (data[0] == 0x01)
    {
        // Check size.
        if (size != 11)
            return false;

        paramId = (ObjectDetectorParam)id;
        memcpy(&value, &data[7], 4);
        return 1;
    }

    return -1;
}