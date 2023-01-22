target_sources(nealog PRIVATE Logger.cpp Sink.cpp Severity.cpp Formatter.cpp Error.cpp)
target_precompile_headers(nealog PUBLIC "${CMAKE_CURRENT_LIST_DIR}/pch.h")
