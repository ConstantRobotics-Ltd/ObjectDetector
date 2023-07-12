#include "VSource.h"
#include "VSourceVersion.h"



// Link namespaces.
using namespace std;
using namespace cr::video;



/// Get string of current library version.
std::string VSource::getVersion()
{
    return VSOURCE_VERSION;
}



/// Copy operator.
VSourceParams &VSourceParams::operator= (const VSourceParams &src)
{
    // Check yourself.
    if (this == &src)
        return *this;

    // Copy params.
    logLevel = src.logLevel;
    source = src.source;
    fourcc = src.fourcc;
    width = src.width;
    height = src.height;
    gainMode = src.gainMode;
    gain = src.gain;
    exposureMode = src.exposureMode;
    exposure = src.exposure;
    focusMode = src.focusMode;
    focusPos = src.focusPos;
    cycleTimeMks = src.cycleTimeMks;
    fps = src.fps;
    isOpen = src.isOpen;
    custom1 = src.custom1;
    custom2 = src.custom2;
    custom3 = src.custom3;

    return *this;
}



/// Encode params.
void VSourceParams::encode(uint8_t* data, int& size, VSourceParamsMask* mask)
{
    // Encode version.
    int pos = 0;
    data[pos] = 0x02; pos += 1;
    data[pos] = VSOURCE_MAJOR_VERSION; pos += 1;
    data[pos] = VSOURCE_MINOR_VERSION; pos += 1;

    if (mask == nullptr)
    {
        // Prepare mask.
        data[pos] = 0xFF; pos += 1;
        data[pos] = 0xFF; pos += 1;

        // Encode data.
        memcpy(&data[pos], &logLevel, 4); pos += 4;
        memcpy(&data[pos], &width, 4); pos += 4;
        memcpy(&data[pos], &height, 4); pos += 4;
        memcpy(&data[pos], &gainMode, 4); pos += 4;
        memcpy(&data[pos], &gain, 4); pos += 4;
        memcpy(&data[pos], &exposureMode, 4); pos += 4;
        memcpy(&data[pos], &exposure, 4); pos += 4;
        memcpy(&data[pos], &focusMode, 4); pos += 4;
        memcpy(&data[pos], &focusPos, 4); pos += 4;
        memcpy(&data[pos], &cycleTimeMks, 4); pos += 4;
        memcpy(&data[pos], &fps, 4); pos += 4;
        data[pos] = isOpen == true ? 0x01 : 0x00; pos += 1;
        memcpy(&data[pos], &custom1, 4); pos += 4;
        memcpy(&data[pos], &custom2, 4); pos += 4;
        memcpy(&data[pos], &custom3, 4); pos += 4;

        size = pos;

        return;
    }

    // Prepare mask.
    data[pos] = 0;
    data[pos] = data[pos] | (mask->logLevel ? (uint8_t)128 : (uint8_t)0);
    data[pos] = data[pos] | (mask->width ? (uint8_t)64 : (uint8_t)0);
    data[pos] = data[pos] | (mask->height ? (uint8_t)32 : (uint8_t)0);
    data[pos] = data[pos] | (mask->gainMode ? (uint8_t)16 : (uint8_t)0);
    data[pos] = data[pos] | (mask->gain ? (uint8_t)8 : (uint8_t)0);
    data[pos] = data[pos] | (mask->exposureMode ? (uint8_t)4 : (uint8_t)0);
    data[pos] = data[pos] | (mask->exposure ? (uint8_t)2 : (uint8_t)0);
    data[pos] = data[pos] | (mask->focusMode ? (uint8_t)1 : (uint8_t)0);
    pos += 1;
    data[pos] = 0;
    data[pos] = data[pos] | (mask->focusPos ? (uint8_t)128 :(uint8_t)0);
    data[pos] = data[pos] | (mask->cycleTimeMks ? (uint8_t)64 : (uint8_t)0);
    data[pos] = data[pos] | (mask->fps ? (uint8_t)32 : (uint8_t)0);
    data[pos] = data[pos] | (mask->isOpen ? (uint8_t)16 : (uint8_t)0);
    data[pos] = data[pos] | (mask->custom1 ? (uint8_t)8 : (uint8_t)0);
    data[pos] = data[pos] | (mask->custom2 ? (uint8_t)4 : (uint8_t)0);
    data[pos] = data[pos] | (mask->custom3 ? (uint8_t)2 : (uint8_t)0);
    pos += 1;

    if (mask->logLevel)
    {
        memcpy(&data[pos], &logLevel, 4); pos += 4;
    }
    if (mask->width)
    {
        memcpy(&data[pos], &width, 4); pos += 4;
    }
    if (mask->height)
    {
        memcpy(&data[pos], &height, 4); pos += 4;
    }
    if (mask->gainMode)
    {
        memcpy(&data[pos], &gainMode, 4); pos += 4;
    }
    if (mask->gain)
    {
        memcpy(&data[pos], &gain, 4); pos += 4;
    }
    if (mask->exposureMode)
    {
        memcpy(&data[pos], &exposureMode, 4); pos += 4;
    }
    if (mask->exposure)
    {
        memcpy(&data[pos], &exposure, 4); pos += 4;
    }
    if (mask->focusMode)
    {
        memcpy(&data[pos], &focusMode, 4); pos += 4;
    }
    if (mask->focusPos)
    {
        memcpy(&data[pos], &focusPos, 4); pos += 4;
    }
    if (mask->cycleTimeMks)
    {
        memcpy(&data[pos], &cycleTimeMks, 4); pos += 4;
    }
    if (mask->fps)
    {
        memcpy(&data[pos], &fps, 4); pos += 4;
    }
    if (mask->isOpen)
    {
        data[pos] = isOpen == true ? 0x01 : 0x00; pos += 1;
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
        memcpy(&data[pos], &custom3, 4);
    }
}



/// Decode params.
bool VSourceParams::decode(uint8_t* data)
{
    // Check header.
    if (data[0] != 0x02)
        return false;

    // Decode version.
    if (data[1] != VSOURCE_MAJOR_VERSION)
        return false;
    if (data[2] != VSOURCE_MINOR_VERSION)
        return false;

    int pos = 5;
    if ((data[3] & (uint8_t)128) == (uint8_t)128)
    {
        memcpy(&logLevel, &data[pos], 4); pos += 4;
    }
    else
    {
        logLevel = 0;
    }
    if ((data[3] & (uint8_t)64) == (uint8_t)64)
    {
        memcpy(&width, &data[pos], 4); pos += 4;
    }
    else
    {
        width = 0;
    }
    if ((data[3] & (uint8_t)32) == (uint8_t)32)
    {
        memcpy(&height, &data[pos], 4); pos += 4;
    }
    else
    {
        height = 0;
    }
    if ((data[3] & (uint8_t)16) == (uint8_t)16)
    {
        memcpy(&gainMode, &data[pos], 4); pos += 4;
    }
    else
    {
        gainMode = 0;
    }
    if ((data[3] & (uint8_t)8) == (uint8_t)8)
    {
        memcpy(&gain, &data[pos], 4); pos += 4;
    }
    else
    {
        gain = 0;
    }
    if ((data[3] & (uint8_t)4) == (uint8_t)4)
    {
        memcpy(&exposureMode, &data[pos], 4); pos += 4;
    }
    else
    {
        exposureMode = 0;
    }
    if ((data[3] & (uint8_t)2) == (uint8_t)2)
    {
        memcpy(&exposure, &data[pos], 4); pos += 4;
    }
    else
    {
        exposure = 0;
    }
    if ((data[3] & (uint8_t)1) == (uint8_t)1)
    {
        memcpy(&focusMode, &data[pos], 4); pos += 4;
    }
    else
    {
        focusMode = 0;
    }


    if ((data[4] & (uint8_t)128) == (uint8_t)128)
    {
        memcpy(&focusPos, &data[pos], 4); pos += 4;
    }
    else
    {
        focusPos = 0;
    }
    if ((data[4] & (uint8_t)64) == (uint8_t)64)
    {
        memcpy(&cycleTimeMks, &data[pos], 4); pos += 4;
    }
    else
    {
        cycleTimeMks = 0;
    }
    if ((data[4] & (uint8_t)32) == (uint8_t)32)
    {
        memcpy(&fps, &data[pos], 4); pos += 4;
    }
    else
    {
        fps = 0.0f;
    }
    if ((data[4] & (uint8_t)16) == (uint8_t)16)
    {
        isOpen = data[pos] == 0x00 ? false : true; pos += 1;
    }
    else
    {
        isOpen = false;
    }
    if ((data[4] & (uint8_t)8) == (uint8_t)8)
    {
        memcpy(&custom1, &data[pos], 4); pos += 4;
    }
    else
    {
        custom1 = 0.0f;
    }
    if ((data[4] & (uint8_t)4) == (uint8_t)4)
    {
        memcpy(&custom2, &data[pos], 4); pos += 4;
    }
    else
    {
        custom2 = 0.0f;
    }
    if ((data[4] & (uint8_t)2) == (uint8_t)2)
    {
        memcpy(&custom3, &data[pos], 4);
    }
    else
    {
        custom3 = 0.0f;
    }

    source = "";
    fourcc = "";

    return true;
}



/// Encode set param command.
void cr::video::VSource::encodeSetParamCommand(
        uint8_t* data, int& size, VSourceParam id, float value)
{
    // Fill header.
    data[0] = 0x01;
    data[1] = VSOURCE_MAJOR_VERSION;
    data[2] = VSOURCE_MINOR_VERSION;

    // Fill data.
    int paramId = (int)id;
    memcpy(&data[3], &paramId, 4);
    memcpy(&data[7], &value, 4);
    size = 11;
}



/// Encode command.
void cr::video::VSource::encodeCommand(uint8_t* data,
                                   int& size,
                                   cr::video::VSourceCommand id)
{
    // Fill header.
    data[0] = 0x00;
    data[1] = VSOURCE_MAJOR_VERSION;
    data[2] = VSOURCE_MINOR_VERSION;

    // Fill data.
    int commandId = (int)id;
    memcpy(&data[3], &commandId, 4);
    size = 7;
}



/// Decode command.
int cr::video::VSource::decodeCommand(uint8_t* data,
                                  int size,
                                  cr::video::VSourceParam& paramId,
                                  cr::video::VSourceCommand& commandId,
                                  float& value)
{
    // Check size.
    if (size < 7)
        return -1;

    // Check version.
    if (data[1] != VSOURCE_MAJOR_VERSION || data[2] != VSOURCE_MINOR_VERSION)
        return -1;

    // Extract data.
    int id = 0;
    memcpy(&id, &data[3], 4);
    value = 0.0f;


    // Check command type.
    if (data[0] == 0x00)
    {
        commandId = (VSourceCommand)id;
        return 0;
    }
    else if (data[0] == 0x01)
    {
        // Check size.
        if (size != 11)
            return false;

        paramId = (VSourceParam)id;
        memcpy(&value, &data[7], 4);
        return 1;
    }

    return -1;
}


