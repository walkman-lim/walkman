
cmake_minimum_required(VERSION 3.22)
project(NAM_A2_Plugin VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 20)

# JUCE - 需自行下載 JUCE 7 到 ../JUCE 或修改路徑
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../JUCE juce_build)

juce_add_plugin(NAM_A2_Plugin
    COMPANY_NAME "LittleThomasStudio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Ltts
    PLUGIN_CODE NaA2
    FORMATS VST3 Standalone
    PRODUCT_NAME "NAM A2 - Neural Amp Modeler"
)

juce_generate_juce_header(NAM_A2_Plugin)

target_sources(NAM_A2_Plugin PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/dsp/nam/Model.cpp
)

target_include_directories(NAM_A2_Plugin PRIVATE Source Source/dsp)

target_compile_definitions(NAM_A2_Plugin PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(NAM_A2_Plugin PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)

# 優化 - NAM 推論需要
if(MSVC)
    target_compile_options(NAM_A2_Plugin PRIVATE /arch:AVX2 /O2 /fp:fast)
else()
    target_compile_options(NAM_A2_Plugin PRIVATE -O3 -mavx2 -mfma)
endif()
