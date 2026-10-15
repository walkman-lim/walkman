
#pragma once
#include <JuceHeader.h>
#include <vector>
#include <string>

namespace nam {

// 簡化的 WaveNet NAM 推論器 - 相容 NAM v0.7.3 標準架構
struct WaveNetLayer {
    std::vector<float> weight, bias;
    std::vector<float> gate_weight, gate_bias;
    int dilation = 1;
    int kernel_size = 3;
};

class Model {
public:
    Model();
    void prepare(double sampleRate);
    bool loadFromFile(const juce::File& file);
    float processSample(float input) noexcept;
    bool isLoaded() const { return loaded; }
    juce::String getName() const { return modelName; }

private:
    bool loaded = false;
    juce::String modelName = "Empty";
    double sampleRate = 48000.0;

    // 架構參數
    int receptiveField = 0;
    std::vector<WaveNetLayer> layers;
    std::vector<float> headWeight;
    float inputScale = 1.0f;

    // 狀態緩衝 (Dilated Conv 的歷史)
    struct DelayLine {
        std::vector<float> buf;
        int pos = 0;
        int dilation = 1;
        float get(int offset) const;
        void push(float v);
        void resize(int size);
    };
    std::vector<DelayLine> delayLines;
    std::vector<float> layerOutputs;

    float conv1d(const WaveNetLayer& layer, const DelayLine& dl) const;
    bool parseJson(const juce::String& jsonContent);
};
}
