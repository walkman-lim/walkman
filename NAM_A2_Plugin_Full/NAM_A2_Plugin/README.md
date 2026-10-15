
# NAM A2 - 完整吉他音箱模擬 VST3 插件

## 功能 (A2 功能齊全版)
- **NAM 核心**: 完整相容 .nam 模型 (Neural Amp Modeler)，支援 WaveNet / LSTM 架構
- **效果鏈**: Input Gate -> Tube Screamer -> NAM Amp -> IR Cabinet -> 3-Band EQ -> Output
- **IR 箱體模擬**: 支援載入 .wav IR, 內建 4x12 經典箱體，1024-2048 samples 零延遲卷積
- **雙模型**: A/B 快速切換，支援 Blend
- **超低延遲**: 64 samples 預處理, 支援雙精度
- **預設系統**: 支援 NAM 官方模型

## 支援宿主
Windows 10 64-bit / VST3
測試通過: Cubase 12/13, REAPER 7, Ableton Live 11/12

## 編譯 (Windows 10)

1. 安裝: Visual Studio 2022, CMake 3.22+
2. 下載 JUCE 7: `git clone https://github.com/juce-framework/JUCE.git` 放到本專案上一層
3. 
```bat
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```
4. 編譯後 VST3 在 `build/NAM_A2_Plugin_artefacts/Release/VST3/`

## 使用
1. 把 .nam 模型拖入介面，或點 LOAD NAM
2. 載入 .wav IR 檔案
3. 模型下載: https://tonehunt.org / https://neuralampmodeler.com

## 架構說明
- `Model.cpp`: 手寫 WaveNet 前向推論，無依賴 PyTorch，AVX2 加速
- `PluginProcessor`: 管理參數、過採樣 2x/4x、線程安全模型切換
- `PluginEditor`: 現代化 UI，OpenGL 加速示波器

作者: Thomas Lim / LittleThomasStudio
