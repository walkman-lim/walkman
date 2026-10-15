
#pragma once
#include <JuceHeader.h>
#include "dsp/nam/Model.h"

class NAM_A2_AudioProcessor : public juce::AudioProcessor
{
public:
    NAM_A2_AudioProcessor();
    ~NAM_A2_AudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "NAM A2"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // API for UI
    void loadNAMModel(const juce::File& file, int slot);
    void loadIR(const juce::File& file);
    bool isModelLoaded(int slot) const;

    juce::AudioProcessorValueTreeState apvts;
    std::atomic<float>* paramInput = nullptr;
    std::atomic<float>* paramGate = nullptr;
    std::atomic<float>* paramDrive = nullptr;
    std::atomic<float>* paramModelBlend = nullptr;
    std::atomic<float>* paramOutput = nullptr;
    std::atomic<float>* paramIRMix = nullptr;

    // For metering
    float inputLevel = 0.f;
    float outputLevel = 0.f;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    // DSP
    nam::Model modelA, modelB;
    juce::dsp::Oversampling<float> oversampler { 2, 4, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true };

    // Effects chain
    juce::dsp::NoiseGate<float> gate;
    juce::dsp::IIR::Filter<float> toneStack;
    juce::dsp::Convolution irLoader;

    double lastSampleRate = 48000.0;
    juce::File currentNAMFileA, currentNAMFileB;
    juce::File currentIRFile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NAM_A2_AudioProcessor)
};
