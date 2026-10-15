
#include "PluginEditor.h"
#include "PluginProcessor.h"

NAM_A2_AudioProcessorEditor::NAM_A2_AudioProcessorEditor(NAM_A2_AudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(800, 500);

    addAndMakeVisible(loadA); addAndMakeVisible(loadB); addAndMakeVisible(loadIRBtn);
    addAndMakeVisible(infoA); addAndMakeVisible(infoB); addAndMakeVisible(infoIR);
    addAndMakeVisible(inputKnob); addAndMakeVisible(gateKnob); addAndMakeVisible(driveKnob);
    addAndMakeVisible(blendKnob); addAndMakeVisible(outputKnob);

    setupKnob(inputKnob, "INPUT"); setupKnob(gateKnob, "GATE");
    setupKnob(driveKnob, "DRIVE"); setupKnob(blendKnob, "BLEND");
    setupKnob(outputKnob, "OUTPUT");

    infoA.setText("No Model A", juce::dontSendNotification);
    infoB.setText("No Model B", juce::dontSendNotification);
    infoIR.setText("No IR - Direct", juce::dontSendNotification);

    loadA.onClick = [this]{
        juce::FileChooser fc("Select .nam model", {}, "*.nam;*.json");
        if(fc.browseForFileToOpen()) { processor.loadNAMModel(fc.getResult(),0); infoA.setText(fc.getResult().getFileName(), juce::dontSendNotification); }
    };
    loadB.onClick = [this]{
        juce::FileChooser fc("Select .nam model B", {}, "*.nam;*.json");
        if(fc.browseForFileToOpen()) { processor.loadNAMModel(fc.getResult(),1); infoB.setText(fc.getResult().getFileName(), juce::dontSendNotification); }
    };
    loadIRBtn.onClick = [this]{
        juce::FileChooser fc("Select IR .wav", {}, "*.wav");
        if(fc.browseForFileToOpen()) { processor.loadIR(fc.getResult()); infoIR.setText(fc.getResult().getFileName(), juce::dontSendNotification); }
    };

    attIn = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts,"INPUT",inputKnob);
    attGate = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts,"GATE",gateKnob);
    attDrive = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts,"DRIVE",driveKnob);
    attBlend = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts,"BLEND",blendKnob);
    attOut = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts,"OUTPUT",outputKnob);
}

NAM_A2_AudioProcessorEditor::~NAM_A2_AudioProcessorEditor(){}

void NAM_A2_AudioProcessorEditor::setupKnob(juce::Slider& s, const juce::String& name)
{
    s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
}

void NAM_A2_AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colours::white);
    g.setFont(24.f);
    g.drawText("NAM A2 - Neural Amp Modeler | LittleThomasStudio", 20, 10, 760, 30, juce::Justification::centredLeft);
    g.setFont(12.f);
    g.setColour(juce::Colours::grey);
    g.drawText("VST3 | Windows 10 | Cubase / Reaper / Live | Supports .nam & IR .wav", 20, 40, 760, 20, juce::Justification::centredLeft);
}

void NAM_A2_AudioProcessorEditor::resized()
{
    loadA.setBounds(20, 70, 120, 30); infoA.setBounds(150, 70, 200, 30);
    loadB.setBounds(20, 110, 120, 30); infoB.setBounds(150, 110, 200, 30);
    loadIRBtn.setBounds(20, 150, 120, 30); infoIR.setBounds(150, 150, 200, 30);

    inputKnob.setBounds(400, 80, 80, 90);
    gateKnob.setBounds(490, 80, 80, 90);
    driveKnob.setBounds(580, 80, 80, 90);
    blendKnob.setBounds(400, 190, 80, 90);
    outputKnob.setBounds(490, 190, 80, 90);
}
