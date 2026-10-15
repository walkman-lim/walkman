
#include "PluginProcessor.h"
#include "PluginEditor.h"

NAM_A2_AudioProcessor::NAM_A2_AudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createLayout()),
      irLoader(juce::dsp::Convolution::NonUniform::headSizeInSamples)
{
    paramInput = apvts.getRawParameterValue("INPUT");
    paramGate = apvts.getRawParameterValue("GATE");
    paramDrive = apvts.getRawParameterValue("DRIVE");
    paramModelBlend = apvts.getRawParameterValue("BLEND");
    paramOutput = apvts.getRawParameterValue("OUTPUT");
    paramIRMix = apvts.getRawParameterValue("IRMIX");
}

NAM_A2_AudioProcessor::~NAM_A2_AudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout NAM_A2_AudioProcessor::createLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("INPUT","Input Gain", juce::NormalisableRange<float>(-20.f, 20.f, 0.1f), 0.f, "dB"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GATE","Noise Gate", juce::NormalisableRange<float>(-90.f, -20.f, 1.f), -70.f, "dB"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DRIVE","Drive", juce::NormalisableRange<float>(0.f, 1.f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("BLEND","A/B Blend", juce::NormalisableRange<float>(0.f, 1.f, 0.01f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("OUTPUT","Output", juce::NormalisableRange<float>(-20.f, 20.f, 0.1f), 0.f, "dB"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("IRMIX","IR Mix", juce::NormalisableRange<float>(0.f, 1.f, 0.01f), 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("BASS","Bass", juce::NormalisableRange<float>(-12.f, 12.f, 0.1f), 0.f, "dB"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MID","Mid", juce::NormalisableRange<float>(-12.f, 12.f, 0.1f), 0.f, "dB"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("TREBLE","Treble", juce::NormalisableRange<float>(-12.f, 12.f, 0.1f), 0.f, "dB"));
    return { params.begin(), params.end() };
}

void NAM_A2_AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    lastSampleRate = sampleRate;
    juce::dsp::ProcessSpec spec{ sampleRate, (juce::uint32)samplesPerBlock, 2 };

    oversampler.initProcessing(samplesPerBlock);
    oversampler.reset();

    gate.prepare(spec);
    gate.setThreshold(*paramGate);

    irLoader.prepare(spec);

    modelA.prepare(sampleRate);
    modelB.prepare(sampleRate);
}

void NAM_A2_AudioProcessor::releaseResources() {}

bool NAM_A2_AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo() || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono();
}

void NAM_A2_AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const float inGain = juce::Decibels::decibelsToGain(paramInput->load());
    const float outGain = juce::Decibels::decibelsToGain(paramOutput->load());
    const float blend = paramModelBlend->load();
    const float drive = paramDrive->load() * 2.0f + 1.0f;

    // Gate update
    gate.setThreshold(*paramGate);

    // Oversampling block
    juce::dsp::AudioBlock<float> block(buffer);
    auto osBlock = oversampler.processSamplesUp(block);

    for(int ch=0; ch < (int)osBlock.getNumChannels(); ++ch)
    {
        auto* data = osBlock.getChannelPointer(ch);
        for(int i=0; i < (int)osBlock.getNumSamples(); ++i)
        {
            float x = data[i] * inGain;
            x = std::tanh(x * drive); // Tube Screamer soft clip pre

            // Noise Gate (simple)
            // NAM inference
            float yA = modelA.processSample(x);
            float yB = modelB.isLoaded() ? modelB.processSample(x) : yA;
            float y = yA * (1.0f - blend) + yB * blend;

            data[i] = y;
        }
    }

    oversampler.processSamplesDown(block);

    // IR Cabinet - after downsampling
    if(irLoader.getCurrentIRSize() > 0)
    {
        juce::dsp::AudioBlock<float> b2(buffer);
        juce::dsp::ProcessContextReplacing<float> ctx(b2);
        irLoader.process(ctx);
    }

    buffer.applyGain(outGain);

    // metering
    inputLevel = buffer.getMagnitude(0, buffer.getNumSamples());
    outputLevel = inputLevel; // simplified
}

void NAM_A2_AudioProcessor::loadNAMModel(const juce::File& file, int slot)
{
    if(slot==0) { modelA.loadFromFile(file); currentNAMFileA = file; }
    else { modelB.loadFromFile(file); currentNAMFileB = file; }
}

void NAM_A2_AudioProcessor::loadIR(const juce::File& file)
{
    // juce Convolution load
    juce::dsp::ProcessSpec spec{ lastSampleRate, 512, 2 };
    irLoader.reset();
    irLoader.loadImpulseResponse(file, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, 0);
    currentIRFile = file;
}

bool NAM_A2_AudioProcessor::isModelLoaded(int slot) const { return slot==0 ? modelA.isLoaded() : modelB.isLoaded(); }

void NAM_A2_AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty("NAM_A", currentNAMFileA.getFullPathName(), nullptr);
    state.setProperty("NAM_B", currentNAMFileB.getFullPathName(), nullptr);
    state.setProperty("IR", currentIRFile.getFullPathName(), nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void NAM_A2_AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if(xmlState)
    {
        auto state = juce::ValueTree::fromXml(*xmlState);
        apvts.replaceState(state);
        juce::File fA(state.getProperty("NAM_A").toString());
        juce::File fB(state.getProperty("NAM_B").toString());
        juce::File fIR(state.getProperty("IR").toString());
        if(fA.existsAsFile()) loadNAMModel(fA,0);
        if(fB.existsAsFile()) loadNAMModel(fB,1);
        if(fIR.existsAsFile()) loadIR(fIR);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new NAM_A2_AudioProcessor(); }
juce::AudioProcessorEditor* NAM_A2_AudioProcessor::createEditor() { return new NAM_A2_AudioProcessorEditor(*this); }
