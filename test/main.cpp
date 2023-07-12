#include <iostream>
#include "VSource.h"



/// Link namesapces.
using namespace cr::video;
using namespace std;



// Copy test.
bool copyTest();

// Encode/decode test.
bool encodeDecodeTest();

/// Encode/decode test with params mask.
bool encodeDecodeWithMaskTest();

/// Encode/decode commands test.
bool encodeDecodeCommandsTest();

/// JSON read/write test.
bool jsonReadWriteTest();



// Entry point.
int main(void)
{
    cout << "#####################################" << endl;
    cout << "#                                   #" << endl;
    cout << "# VSourceParams test                #" << endl;
    cout << "#                                   #" << endl;
    cout << "#####################################" << endl;
    cout << endl;

    cout << "Copy test:" << endl;
    if (copyTest())
        cout << "OK" << endl;
    else
        cout << "ERROR" << endl;
    cout << endl;

    cout << "Encode/Decode test:" << endl;
    if (encodeDecodeTest())
        cout << "OK" << endl;
    else
        cout << "ERROR" << endl;
    cout << endl;

    cout << "Encode/Decode test with params mask:" << endl;
    if (encodeDecodeWithMaskTest())
        cout << "OK" << endl;
    else
        cout << "ERROR" << endl;
    cout << endl;

    cout << "Encode/Decode commands test:" << endl;
    if (encodeDecodeCommandsTest())
        cout << "OK" << endl;
    else
        cout << "ERROR" << endl;
    cout << endl;

    cout << "JSON read/write test:" << endl;
    if (jsonReadWriteTest())
        cout << "OK" << endl;
    else
        cout << "ERROR" << endl;
    cout << endl;

    return 1;
}



// Copy test.
bool copyTest()
{
    // Prepare random params.
    VSourceParams in;
    in.source = "alsfghljb";
    in.fourcc = "skdfjhvk";
    in.logLevel = rand() % 255;
    in.cycleTimeMks = rand() % 255;
    in.exposure = rand() % 255;
    in.exposureMode = rand() % 255;
    in.gainMode = rand() % 255;
    in.gain = rand() % 255;
    in.focusMode = rand() % 255;
    in.focusPos = rand() % 255;
    in.fps = rand() % 255;
    in.width = rand() % 255;
    in.height = rand() % 255;
    in.isOpen = true;
    in.custom1 = rand() % 255;
    in.custom2 = rand() % 255;
    in.custom3 = rand() % 255;

    // Copy params.
    VSourceParams out = in;

    // Compare params.
    if (in.source != out.source)
    {
        cout << "in.source" << endl;
        return false;
    }
    if (in.fourcc != out.fourcc)
    {
        cout << "in.fourcc" << endl;
        return false;
    }
    if (in.logLevel != out.logLevel)
    {
        cout << "in.logLevel" << endl;
        return false;
    }
    if (in.cycleTimeMks != out.cycleTimeMks)
    {
        cout << "in.cycleTimeMks" << endl;
        return false;
    }
    if (in.exposure != out.exposure)
    {
        cout << "in.exposure" << endl;
        return false;
    }
    if (in.exposureMode != out.exposureMode)
    {
        cout << "in.exposureMode" << endl;
        return false;
    }
    if (in.gainMode != out.gainMode)
    {
        cout << "in.gainMode" << endl;
        return false;
    }
    if (in.gain != out.gain)
    {
        cout << "in.gain" << endl;
        return false;
    }
    if (in.focusMode != out.focusMode)
    {
        cout << "in.focusMode" << endl;
        return false;
    }
    if (in.focusPos != out.focusPos)
    {
        cout << "in.focusPos" << endl;
        return false;
    }
    if (in.fps != out.fps)
    {
        cout << "in.fps" << endl;
        return false;
    }
    if (in.width != out.width)
    {
        cout << "in.width" << endl;
        return false;
    }
    if (in.height != out.height)
    {
        cout << "in.height" << endl;
        return false;
    }
    if (in.isOpen != out.isOpen)
    {
        cout << "in.isOpen" << endl;
        return false;
    }
    if (in.custom1 != out.custom1)
    {
        cout << "in.custom1" << endl;
        return false;
    }
    if (in.custom2 != out.custom2)
    {
        cout << "in.custom2" << endl;
        return false;
    }
    if (in.custom3 != out.custom3)
    {
        cout << "in.custom3" << endl;
        return false;
    }

    return true;
}



// Encode/decode test.
bool encodeDecodeTest()
{
    // Prepare random params.
    VSourceParams in;
    in.source = "source";
    in.fourcc = "skdfjhvk";
    in.logLevel = rand() % 255;
    in.cycleTimeMks = rand() % 255;
    in.exposure = rand() % 255;
    in.exposureMode = rand() % 255;
    in.gainMode = rand() % 255;
    in.gain = rand() % 255;
    in.focusMode = rand() % 255;
    in.focusPos = rand() % 255;
    in.fps = rand() % 255;
    in.width = rand() % 255;
    in.height = rand() % 255;
    in.isOpen = true;
    in.custom1 = rand() % 255;
    in.custom2 = rand() % 255;
    in.custom3 = rand() % 255;

    // Encode data.
    uint8_t data[1024];
    int size = 0;
    in.encode(data, size);

    cout << "Encoded data size: " << size << " bytes" << endl;

    // Decode data.
    VSourceParams out;
    if (!out.decode(data))
    {
        cout << "Can't decode data" << endl;
        return false;
    }

    // Compare params.
    if (out.source != "")
    {
        cout << "in.source" << endl;
        return false;
    }
    if (out.fourcc != "")
    {
        cout << "in.fourcc" << endl;
        return false;
    }
    if (in.logLevel != out.logLevel)
    {
        cout << "in.logLevel" << endl;
        return false;
    }
    if (in.cycleTimeMks != out.cycleTimeMks)
    {
        cout << "in.cycleTimeMks" << endl;
        return false;
    }
    if (in.exposure != out.exposure)
    {
        cout << "in.exposure" << endl;
        return false;
    }
    if (in.exposureMode != out.exposureMode)
    {
        cout << "in.exposureMode" << endl;
        return false;
    }
    if (in.gainMode != out.gainMode)
    {
        cout << "in.gainMode" << endl;
        return false;
    }
    if (in.gain != out.gain)
    {
        cout << "in.gain" << endl;
        return false;
    }
    if (in.focusMode != out.focusMode)
    {
        cout << "in.focusMode" << endl;
        return false;
    }
    if (in.focusPos != out.focusPos)
    {
        cout << "in.focusPos" << endl;
        return false;
    }
    if (in.fps != out.fps)
    {
        cout << "in.fps" << endl;
        return false;
    }
    if (in.width != out.width)
    {
        cout << "in.width" << endl;
        return false;
    }
    if (in.height != out.height)
    {
        cout << "in.height" << endl;
        return false;
    }
    if (in.isOpen != out.isOpen)
    {
        cout << "in.isOpen" << endl;
        return false;
    }
    if (in.custom1 != out.custom1)
    {
        cout << "in.custom1" << endl;
        return false;
    }
    if (in.custom2 != out.custom2)
    {
        cout << "in.custom2" << endl;
        return false;
    }
    if (in.custom3 != out.custom3)
    {
        cout << "in.custom3" << endl;
        return false;
    }

    return true;
}



// Encode/decode commands test.
bool encodeDecodeCommandsTest()
{
    // Encode command.
    uint8_t data[1024];
    int size = 0;
    float outValue = (float)(rand() % 20);
    VSource::encodeCommand(data, size, VSourceCommand::RESTART);

    // Decode command.
    VSourceCommand commandId;
    VSourceParam paramId;
    float value = 0.0f;
    if (VSource::decodeCommand(data, size, paramId, commandId, value) != 0)
    {
        cout << "Command not decoded" << endl;
        return false;
    }

    // Checkk ID and value.
    if (commandId != VSourceCommand::RESTART)
    {
        cout << "not a VSourceCommand::RESTART" << endl;
        return false;
    }

    // Encode param.
    outValue = (float)(rand() % 20);
    VSource::encodeSetParamCommand(data, size, VSourceParam::EXPOSURE,outValue);

    // Decode command.
    value = 0.0f;
    if (VSource::decodeCommand(data, size, paramId, commandId, value) != 1)
    {
        cout << "Set param command not decoded" << endl;
        return false;
    }

    // Checkk ID and value.
    if (paramId != VSourceParam::EXPOSURE)
    {
        cout << "not a VSourceParam::EXPOSURE" << endl;
        return false;
    }
    if (value != outValue)
    {
        cout << "not equal value" << endl;
        return false;
    }

    return true;
}



/// JSON read/write test.
bool jsonReadWriteTest()
{
    // Prepare random params.
    VSourceParams in;
    in.source = "alsfghljb";
    in.fourcc = "skdfjhvk";
    in.logLevel = rand() % 255;
    in.cycleTimeMks = rand() % 255;
    in.exposure = rand() % 255;
    in.exposureMode = rand() % 255;
    in.gainMode = rand() % 255;
    in.gain = rand() % 255;
    in.focusMode = rand() % 255;
    in.focusPos = rand() % 255;
    in.fps = rand() % 255;
    in.width = rand() % 255;
    in.height = rand() % 255;
    in.isOpen = true;
    in.custom1 = rand() % 255;
    in.custom2 = rand() % 255;
    in.custom3 = rand() % 255;

    // Write params to file.
    cr::utils::ConfigReader inConfig;
    inConfig.set(in, "vSourceParams");
    inConfig.writeToFile("TestVSourceParams.json");

    // Read params from file.
    cr::utils::ConfigReader outConfig;
    if(!outConfig.readFromFile("TestVSourceParams.json"))
    {
        cout << "Can't open config file" << endl;
        return false;
    }

    VSourceParams out;
    if(!outConfig.get(out, "vSourceParams"))
    {
        cout << "Can't read params from file" << endl;
        return false;
    }

    // Compare params.
    bool result = true;
    if (out.source != in.source)
    {
        cout << "in.source" << endl;
        result = false;
    }
    if (out.fourcc != in.fourcc)
    {
        cout << "in.fourcc" << endl;
        result = false;
    }
    if (in.logLevel != out.logLevel)
    {
        cout << "in.logLevel" << endl;
        result = false;
    }
    if (in.exposureMode != out.exposureMode)
    {
        cout << "in.exposureMode" << endl;
        result = false;
    }
    if (in.gainMode != out.gainMode)
    {
        cout << "in.gainMode" << endl;
        result = false;
    }
    if (in.focusMode != out.focusMode)
    {
        cout << "in.focusMode" << endl;
        result = false;
    }
    if (in.fps != out.fps)
    {
        cout << "in.fps" << endl;
        result = false;
    }
    if (in.width != out.width)
    {
        cout << "in.width" << endl;
        result = false;
    }
    if (in.height != out.height)
    {
        cout << "in.height" << endl;
        result = false;
    }
    if (in.custom1 != out.custom1)
    {
        cout << "in.custom1" << endl;
        result = false;
    }
    if (in.custom2 != out.custom2)
    {
        cout << "in.custom2" << endl;
        result = false;
    }
    if (in.custom3 != out.custom3)
    {
        cout << "in.custom3" << endl;
        result = false;
    }

    return result;
}



/// Encode/decode test with params mask.
bool encodeDecodeWithMaskTest()
{
    // Prepare random params.
    VSourceParams in;
    in.source = "source";
    in.fourcc = "skdfjhvk";
    in.logLevel = rand() % 255;
    in.cycleTimeMks = rand() % 255;
    in.exposure = rand() % 255;
    in.exposureMode = rand() % 255;
    in.gainMode = rand() % 255;
    in.gain = rand() % 255;
    in.focusMode = rand() % 255;
    in.focusPos = rand() % 255;
    in.fps = rand() % 255;
    in.width = rand() % 255;
    in.height = rand() % 255;
    in.isOpen = true;
    in.custom1 = rand() % 255;
    in.custom2 = rand() % 255;
    in.custom3 = rand() % 255;

    // Prepare params mask.
    VSourceParamsMask mask;
    mask.logLevel = true;
    mask.cycleTimeMks = false;
    mask.exposure = true;
    mask.exposureMode = false;
    mask.gainMode = true;
    mask.gain = false;
    mask.focusMode = true;
    mask.focusPos = false;
    mask.fps = true;
    mask.width = false;
    mask.height = true;
    mask.isOpen = false;
    mask.custom1 = true;
    mask.custom2 = false;
    mask.custom3 = true;

    // Encode data.
    uint8_t data[1024];
    int size = 0;
    in.encode(data, size, &mask);

    cout << "Encoded data size: " << size << " bytes" << endl;

    // Decode data.
    VSourceParams out;
    if (!out.decode(data))
    {
        cout << "Can't decode data" << endl;
        return false;
    }

    // Compare params.
    if (out.source != "")
    {
        cout << "in.source" << endl;
        return false;
    }
    if (out.fourcc != "")
    {
        cout << "in.fourcc" << endl;
        return false;
    }
    if (in.logLevel != out.logLevel)
    {
        cout << "in.logLevel" << endl;
        return false;
    }
    if (0 != out.cycleTimeMks)
    {
        cout << "in.cycleTimeMks" << endl;
        return false;
    }
    if (in.exposure != out.exposure)
    {
        cout << "in.exposure" << endl;
        return false;
    }
    if (0 != out.exposureMode)
    {
        cout << "in.exposureMode" << endl;
        return false;
    }
    if (in.gainMode != out.gainMode)
    {
        cout << "in.gainMode" << endl;
        return false;
    }
    if (0 != out.gain)
    {
        cout << "in.gain" << endl;
        return false;
    }
    if (in.focusMode != out.focusMode)
    {
        cout << "in.focusMode" << endl;
        return false;
    }
    if (0 != out.focusPos)
    {
        cout << "in.focusPos" << endl;
        return false;
    }
    if (in.fps != out.fps)
    {
        cout << "in.fps" << endl;
        return false;
    }
    if (0 != out.width)
    {
        cout << "in.width" << endl;
        return false;
    }
    if (in.height != out.height)
    {
        cout << "in.height" << endl;
        return false;
    }
    if (false != out.isOpen)
    {
        cout << "in.isOpen" << endl;
        return false;
    }
    if (in.custom1 != out.custom1)
    {
        cout << "in.custom1" << endl;
        return false;
    }
    if (0.0f != out.custom2)
    {
        cout << "in.custom2" << endl;
        return false;
    }
    if (in.custom3 != out.custom3)
    {
        cout << "in.custom3" << endl;
        return false;
    }

    return true;
}














