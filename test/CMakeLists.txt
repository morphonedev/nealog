add_executable(nealog_test)

target_link_libraries(nealog_test PRIVATE gcov Catch2::Catch2WithMain trompeloeil::trompeloeil)

if(nealog_HEADERONLY)
    target_link_libraries(nealog_test PRIVATE nealog::headeronly)
else()
    target_link_libraries(nealog_test PRIVATE nealog)
endif()

target_sources(nealog_test PRIVATE LoggerTest.cpp LoggerRegistryTest.cpp NoopSinkTest.cpp SeverityTest.cpp
                                   StreamSinkTest.cpp)

include(CTest)
include(Catch)
catch_discover_tests(nealog_test)

target_compile_options(nealog_test PRIVATE -fprofile-arcs -ftest-coverage)
