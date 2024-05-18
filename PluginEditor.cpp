/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CarsonCP04ModYouLaterAudioProcessorEditor::CarsonCP04ModYouLaterAudioProcessorEditor (CarsonCP04ModYouLaterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    setSize(450, 300);
    // Setup your sliders and other gui components - - - -
    auto& params = processor.getParameters();


    // Center Frequency Slider
    /*juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(0);
    CenterFreqSlider.setColour(CenterFreqSlider.textBoxTextColourId, juce::Colours::black);
    CenterFreqSlider.setBounds(260, 50, 140, 110);
    CenterFreqSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    CenterFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    CenterFreqSlider.setRange(audioParam->range.start, audioParam->range.end);
    CenterFreqSlider.setValue(*audioParam);
    CenterFreqSlider.addListener(this);*/
    //addAndMakeVisible(CenterFreqSlider);

    // LFO depth Slider
    juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(0);
    LfoDepthSlider.setColour(LfoDepthSlider.textBoxTextColourId, juce::Colours::black);
    LfoDepthSlider.setBounds(325, 165, 140, 110);
    LfoDepthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    LfoDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    LfoDepthSlider.setRange(audioParam->range.start, audioParam->range.end);
    LfoDepthSlider.setValue(*audioParam);
    LfoDepthSlider.addListener(this);
    addAndMakeVisible(LfoDepthSlider);

    // LFO speed slider
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(1);
    LfoSpeedSlider.setColour(LfoSpeedSlider.textBoxTextColourId, juce::Colours::black);
    LfoSpeedSlider.setBounds(200, 165, 140, 110);
    LfoSpeedSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    LfoSpeedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    LfoSpeedSlider.setRange(audioParam->range.start, audioParam->range.end);
    LfoSpeedSlider.setValue(*audioParam);
    LfoSpeedSlider.addListener(this);
    addAndMakeVisible(LfoSpeedSlider);

    // Delay Time Slider
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(2);
    DelayTimeSlider.setColour(DelayTimeSlider.textBoxTextColourId, juce::Colours::black);
    DelayTimeSlider.setBounds(80, 75, 150, 200);
    DelayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    DelayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    DelayTimeSlider.setRange(audioParam->range.start, audioParam->range.end);
    DelayTimeSlider.setValue(*audioParam);
    DelayTimeSlider.addListener(this);
    addAndMakeVisible(DelayTimeSlider);

    // Effect Volume Slider
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(3);
    DelayVolumeSlider.setColour(DelayVolumeSlider.textBoxTextColourId, juce::Colours::black);
    DelayVolumeSlider.setBounds(-20, 75, 150, 200);
    DelayVolumeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    DelayVolumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    DelayVolumeSlider.setRange(audioParam->range.start, audioParam->range.end);
    DelayVolumeSlider.setValue(*audioParam);
    DelayVolumeSlider.addListener(this);
    addAndMakeVisible(DelayVolumeSlider);
}
void CarsonCP04ModYouLaterAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    auto& params = processor.getParameters();

   
    if (slider == &LfoDepthSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(0);
        *audioParam = LfoDepthSlider.getValue();
        DBG("Depth Slider Changed");
    }
    else if (slider == &LfoSpeedSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(1);
        *audioParam = LfoSpeedSlider.getValue();
        DBG("Speed Slider Changed");
    }
    else if (slider == &DelayTimeSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(2);
        *audioParam = DelayTimeSlider.getValue();
        DBG("Delay Time Slider Changed");
    }
    else if (slider == &DelayVolumeSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(3);
        *audioParam = DelayVolumeSlider.getValue();
        DBG("Delay Volume Slider Changed");
    }
}
CarsonCP04ModYouLaterAudioProcessorEditor::~CarsonCP04ModYouLaterAudioProcessorEditor()
{
}

//==============================================================================
void CarsonCP04ModYouLaterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll(juce::Colours::white);

    g.setColour (juce::Colours::black);
    g.setFont (30.0f);
    g.drawFittedText ("Mod-You-Later", getLocalBounds(), juce::Justification::centredTop, 1);

    g.setFont(15.0f);
    g.drawSingleLineText("Delay Base", 121, 70, juce::Justification::left);
    g.drawSingleLineText("Speed", 250, 164, juce::Justification::left);
    g.drawSingleLineText("Effect Mix", 25, 70, juce::Justification::left);
    g.drawSingleLineText("Depth", 377, 164, juce::Justification::left);

}

void CarsonCP04ModYouLaterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
