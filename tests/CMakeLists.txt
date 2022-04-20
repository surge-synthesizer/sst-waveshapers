add_executable(sst-waveshapers-tests)
target_include_directories(sst-waveshapers-tests PRIVATE .)
target_link_libraries(sst-waveshapers-tests PRIVATE ${PROJECT_NAME} simde)
target_sources(sst-waveshapers-tests
    PRIVATE
        SaturatorsTest.cpp
        EffectsTest.cpp
        tests.cpp
)

add_custom_command(TARGET sst-waveshapers-tests
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E echo "Copying $<TARGET_FILE:sst-waveshapers-tests> to test-binary"
        COMMAND ${CMAKE_COMMAND} -E make_directory test-binary
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:sst-waveshapers-tests>" test-binary)