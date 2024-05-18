/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class CarsonCP04ModYouLaterAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    CarsonCP04ModYouLaterAudioProcessorEditor (CarsonCP04ModYouLaterAudioProcessor&);
    ~CarsonCP04ModYouLaterAudioProcessorEditor() override;
    void sliderValueChanged(juce::Slider* slider) override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CarsonCP04ModYouLaterAudioProcessor& audioProcessor;

    juce::Slider DelayVolumeSlider;
    juce::Slider LfoDepthSlider;
    juce::Slider LfoSpeedSlider;
    juce::Slider DelayTimeSlider;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CarsonCP04ModYouLaterAudioProcessorEditor)
};
