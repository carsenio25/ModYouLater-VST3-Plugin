/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CarsonCP04ModYouLaterAudioProcessor::CarsonCP04ModYouLaterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

    addParameter(mLfoDepthParam = new juce::AudioParameterFloat("Depth", // parameterID,
        "LFO Depth", // parameterName,
        0.0f, // minValue,
        10.0f, // maxValue,
        5.0f)); // defaultValue

    addParameter(mLfoSpeedParam = new juce::AudioParameterFloat("Speed", // parameterID,
        "LFO speed", // parameterName,
        0.0f, // minValue,
        1.0f, // maxValue,
        0.5f)); // defaultValue
    addParameter(mDelayMsecParam = new juce::AudioParameterFloat("Delay ms", // parameterID,
        "Delay Center", // parameterName,
        22.0f, // minValue,
        28.0f, // maxValue,
        22.0f)); // defaultValue

    addParameter(mDelayVolumeParam = new juce::AudioParameterFloat("Effect Mix", // parameterID,
        "Effect Mix", // parameterName,
        0.0f, // minValue,
        100.0f, // maxValue,
        50.0f)); // defaultValue
}




void CarsonCP04ModYouLaterAudioProcessor::calcAlgorithmParams()
{
    // set the rate of the LFO
    mLFO.setFreq(mLfoSpeedParam->get(), mFs/mControlN);

    mWetGain = mDelayVolumeParam->get() / 100.0;
    mDryGain = 1.0 - mWetGain;
}

int CarsonCP04ModYouLaterAudioProcessor::calcMsecToSamps(float msec)
{
    //: DelayInSamps = DelayInSec * SamplePerSec
    return (msec / 1000.0f) * getSampleRate();
}
CarsonCP04ModYouLaterAudioProcessor::~CarsonCP04ModYouLaterAudioProcessor()
{
}

//==============================================================================
const juce::String CarsonCP04ModYouLaterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CarsonCP04ModYouLaterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CarsonCP04ModYouLaterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CarsonCP04ModYouLaterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CarsonCP04ModYouLaterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CarsonCP04ModYouLaterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CarsonCP04ModYouLaterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CarsonCP04ModYouLaterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CarsonCP04ModYouLaterAudioProcessor::getProgramName (int index)
{
    return {};
}

void CarsonCP04ModYouLaterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CarsonCP04ModYouLaterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    mFs = sampleRate;
    int maxSamps = calcMsecToSamps(MAXDELAYMSEC);
    mDelay1L.setMaximumDelay(maxSamps);
    mDelay1R.setMaximumDelay(maxSamps);
}

void CarsonCP04ModYouLaterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CarsonCP04ModYouLaterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CarsonCP04ModYouLaterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
     auto totalNumInputChannels = getTotalNumInputChannels();
     auto totalNumOutputChannels = getTotalNumOutputChannels();

     for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
         buffer.clear (i, 0, buffer.getNumSamples());

     auto* channelDataLeft = buffer.getWritePointer(0);
     auto* channelDataRight = buffer.getWritePointer(1);
 
     // Update the parameter values
     calcAlgorithmParams();
 
     float lfoOut, centerMidi, centerFreq, outL, outR, tempL, tempR, newDelaySamp, DelayMsec;
     float coeffs[5];

     // Process each audio sample. I.e. the "inner loop"

     for (int samp = 0; samp < buffer.getNumSamples(); samp++)
     {
         if (mControlCounter % mControlN == 0) {
             // Update the LFO and use it's output to calc the new center freq
             lfoOut = mLFO.tick(); // output of LFO varies between -1.0 and +1.0
            

             DelayMsec = mLfoDepthParam->get() * lfoOut + mDelayMsecParam->get();
             newDelaySamp = calcMsecToSamps(DelayMsec);

             mDelay1L.setDelay(newDelaySamp);
             mDelay1R.setDelay(newDelaySamp);

            
             mControlCounter = 0;
         }
         mControlCounter++;

         outL = channelDataLeft[samp] *mDryGain;
         outR = channelDataRight[samp] *mDryGain;

         tempL = mDelay1L.tick(channelDataLeft[samp]);
         tempR = mDelay1R.tick(channelDataRight[samp]);
 
         

         outL += mWetGain * tempL;
         outR += mWetGain * tempR;

         channelDataLeft[samp] = outL;
         channelDataRight[samp] = outR;
     }
 }

//==============================================================================
bool CarsonCP04ModYouLaterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CarsonCP04ModYouLaterAudioProcessor::createEditor()
{
    return new CarsonCP04ModYouLaterAudioProcessorEditor (*this);
}

//==============================================================================
void CarsonCP04ModYouLaterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CarsonCP04ModYouLaterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CarsonCP04ModYouLaterAudioProcessor();
}
