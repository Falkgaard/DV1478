add_library(myawesomelib
  source/myawesomelib.cpp
  source/implementation-details.h
  include/myawesomelib.h
)
target_include_directories(myawesomelib PUBLIC inc)
target_compile_features(myawesomelib PRIVATE cxx_std_20)
target_link_libraries(my_executable PRIVATE myawesomelib)

