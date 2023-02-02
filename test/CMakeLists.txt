add_executable(nealog_test)

target_link_libraries(nealog_test PRIVATE Catch2::Catch2WithMain trompeloeil::trompeloeil)
if(NOT WIN32)
    target_link_libraries(nealog_test PRIVATE gcov)
endif()

if(nealog_HEADERONLY)
    target_link_libraries(nealog_test PRIVATE nealog::headeronly)
else()
    target_link_libraries(nealog_test PRIVATE nealog)
endif()

target_sources(nealog_test PRIVATE LoggerTest.cpp LoggerTreeRegistryTest.cpp NoopSinkTest.cpp SeverityTest.cpp
                                   StreamSinkTest.cpp)

include(CTest)
include(Catch)
catch_discover_tests(nealog_test)

if(NOT WIN32)
    target_compile_options(nealog_test PRIVATE -fprofile-arcs -ftest-coverage)
endif()
