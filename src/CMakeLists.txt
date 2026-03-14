cmake_minimum_required(VERSION 3.22)
project(Cycle_Based_Sliding_Window_BCC)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -flto ")

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-inline -fsanitize=address -fno-omit-frame-pointer")

include_directories(..)

add_subdirectory(holm_lichtenberg_fdbcc)

set(Src
        utils.cpp
        utils.h
        EdgeMap.cpp
        EdgeMap.h
        PVec.cpp
        PVec.h
        BlockCutTree.cpp
        BlockCutTree.h
        HopcroftTarjan.cpp
        HopcroftTarjan.h
        BlockCutTreeNode.cpp
        BlockCutTreeNode.h
        BcTreeQ.cpp
        BcTreeQ.h
        experiment_utils.cpp
        experiment_utils.h
        Timer.cpp
        Timer.h
        MXT.cpp
        MXT.h
        AUF.cpp
        AUF.h
        BBF.cpp
        BBF.h
        BBFQ.cpp
        BBFQ.h
        NodePool.cpp
        NodePool.h
        BbfNode.cpp
        BbfNode.h
)

add_executable(exp ${Src} experiment_main.cpp)

target_link_libraries(exp PRIVATE HolmLib)

target_compile_definitions(exp PRIVATE NDEBUG)