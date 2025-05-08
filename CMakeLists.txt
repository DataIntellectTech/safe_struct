cmake_minimum_required(VERSION 3.12)
project(safe_struct)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# pull SBE
# -> unfortunatelly need to make long string, seems like cmake doesn't support split lines
set(cust_cmd "git clone git@github.com:aeron-io/simple-binary-encoding.git &&")
set(cust_cmd "${cust_cmd} cd simple-binary-encoding &&")
set(cust_cmd "${cust_cmd} git checkout 1.33.2 &&")
set(cust_cmd "${cust_cmd} ./gradlew &&")
set(cust_cmd "${cust_cmd} java -Dsbe.generate.ir=true -Dsbe.target.language=Cpp -Dsbe.output.dir=../../include -Dsbe.errorLog=yes -jar ./sbe-all/build/libs/sbe-all-1.33.2.jar ../../sbeXml/safe_struct.xml")
execute_process (
    COMMAND bash -c ${cust_cmd}
)

include_directories(.)
include_directories(./include)

add_executable(safe_struct_writer
                safe_struct_writer.cc)

add_executable(safe_struct_reader
                safe_struct_reader.cc)
