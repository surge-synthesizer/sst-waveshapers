juce_add_plugin(WaveshapersPlugin
        BUNDLE_ID org.surge-synth-team.sst-filters.WaveshapersPlugin
        PRODUCT_NAME "WaveshapersPlugin"
        COMPANY_NAME "Surge Synth Team"
        COMPANY_WEBSITE "https://surge-synth-team.org/"
        PLUGIN_MANUFACTURER_CODE VmbA
        PLUGIN_CODE SgWv
        FORMATS VST3 AU Standalone
)

target_sources(WaveshapersPlugin PRIVATE
    WaveshapersPlugin.cpp
#    FiltersPluginEditor.cpp
#    FilterPlotComponent.cpp
)

target_compile_definitions(WaveshapersPlugin
        PUBLIC

        JUCE_ALLOW_STATIC_NULL_VARIABLES=0
        JUCE_STRICT_REFCOUNTEDPOINTER=1

        JUCE_USE_CURL=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CAMERA=disabled

        # These are purposefully not overridden.
        # JUCE_DISPLAY_SPLASH_SCREEN=0
        # JUCE_REPORT_APP_USAGE=0

        JUCE_COREGRAPHICS_DRAW_ASYNC=1
        JUCE_WASAPI=1
        JUCE_DIRECTSOUND=1

        JUCE_CATCH_UNHANDLED_EXCEPTIONS=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(WaveshapersPlugin PUBLIC
        juce::juce_audio_utils
        juce::juce_dsp
        simde
        sst-waveshapers
#        sst-filters-extras
)
