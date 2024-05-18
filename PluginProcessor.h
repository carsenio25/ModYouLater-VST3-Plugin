/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StkLite-4.6.1/BiQuad.h"
#include "StkLite-4.6.1/DelayL.h"
#include "Mu45FilterCalc/Mu45FilterCalc.h"
#include "Mu45LFO/Mu45LFO.h"

#define MAXDELAYMSEC 70.0

//==============================================================================
/**
*/
class CarsonCP04ModYouLaterAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    CarsonCP04ModYouLaterAudioProcessor();
    ~CarsonCP04ModYouLaterAudioProcessor() override;



    // convert MIDI Note Number to Hz
    float calcMiditoHz(float midiNote) { return 440.0 * powf(2, (midiNote - 60.0) / 12.0); }

    // convert Hz to MIDI Note Number
    float calcHzToMidi(float Hz) { return 12 * log2f(Hz / 440.0) + 60.0; }

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    long mControlCounter = 0; // The control rate counter
    int mControlN = 100; // How often cntrl rate variables get updated

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    void calcAlgorithmParams();
    int calcMsecToSamps(float msec);

    juce::AudioParameterFloat* mDelayVolumeParam;
    juce::AudioParameterFloat* mCenterFreqParam;
    juce::AudioParameterFloat* mLfoDepthParam;
    juce::AudioParameterFloat* mLfoSpeedParam;
    juce::AudioParameterFloat* mDelayMsecParam;

    // Member objects we need
    Mu45LFO mLFO;
    stk::BiQuad mFilterL, mFilterR;
    stk::DelayL mDelay1L, mDelay1R;

    // Algorithm Parameters
    float mFilterQ = 2;
    float mFs; // Sampling rate
    float mMinFc;
    float mMaxFc;
    float mWetGain, mDryGain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CarsonCP04ModYouLaterAudioProcessor)
};
