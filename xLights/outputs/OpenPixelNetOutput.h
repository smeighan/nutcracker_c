#pragma once

#include "SerialOutput.h"

// Generic FTDI-based USB to RS485 dongle transmitting Pixelnet

#pragma region Open PixelNet Constants
#define OPENPIXELNET_MAX_CHANNELS 4096
#pragma endregion Open PixelNet Constants

class OpenPixelNetOutput : public SerialOutput
{
    #pragma region Member Variables
    int _datalen = 0;
    uint8_t _data[OPENPIXELNET_MAX_CHANNELS];
    uint8_t _serialBuffer[OPENPIXELNET_MAX_CHANNELS + 8];
    #pragma endregion

public:

    #pragma region Constructors and Destructors
    OpenPixelNetOutput(wxXmlNode* node);
    OpenPixelNetOutput(SerialOutput* output);
    OpenPixelNetOutput();
    virtual ~OpenPixelNetOutput() override {};
    #pragma endregion 

    #pragma region Getters and Setters
    
    virtual std::string GetType() const override { return OUTPUT_OPENPIXELNET; }
    
    virtual int GetMaxChannels() const override { return OPENPIXELNET_MAX_CHANNELS; }
    virtual bool IsValidChannelCount(int32_t channelCount) const override { return channelCount > 0 && channelCount <= OPENPIXELNET_MAX_CHANNELS; }
    
    virtual bool AllowsBaudRateSetting() const override { return false; }
    #pragma endregion 

    #pragma region Start and Stop
    virtual bool Open() override;
    #pragma endregion 
    
    #pragma region Frame Handling
    virtual void EndFrame(int suppressFrames) override;
    #pragma endregion 

    #pragma region Data Setting
    virtual void SetOneChannel(int32_t channel, unsigned char data) override;
    virtual void AllOff() override;
    #pragma endregion 
};
