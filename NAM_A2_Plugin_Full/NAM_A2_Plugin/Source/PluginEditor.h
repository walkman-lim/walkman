
#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class NAM_A2_AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    NAM_A2_AudioProcessorEditor(NAM_A2_AudioProcessor&);
    ~NAM_A2_AudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    NAM_A2_AudioProcessor& processor;

    juce::TextButton loadA{"Load NAM A"}, loadB{"Load NAM B"}, loadIRBtn{"Load IR CAB"};
    juce::Label infoA, infoB, infoIR;
    juce::Slider inputKnob, gateKnob, driveKnob, blendKnob, outputKnob;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attIn, attGate, attDrive, attBlend, attOut;

    void setupKnob(juce::Slider& s, const juce::String& name);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NAM_A2_AudioProcessorEditor)
};
