cmake_minimum_required(VERSION 3.20)
project(HolmSimplifiedBCC)


set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -flto ")


set(CMAKE_CXX_STANDARD 20)


set(Src BasicTree.h
        TreeNode.h
        TreeNode.cpp
        BasicTree.cpp
        NeighbourPartition.cpp
        NeighbourPartition.h
        my_types.h
        commons.cpp
        commons.h
        EdgeInfo.cpp
        EdgeInfo.h
        LevelInfo.cpp
        LevelInfo.h
        NotYetMergeSet.cpp
        NotYetMergeSet.h
        SimHolmBcc.cpp
        SimHolmBcc.h
        HopcroftTarjan.cpp
        HopcroftTarjan.h
        DisjointSet.cpp
        DisjointSet.h
        BCCInfo.cpp
        BCCInfo.h
        tree.cpp
        tree.cpp
        top_tree.cpp
        top_tree.h
        SimpleTree.cpp
        SimpleTree.h
)

add_library(HolmLib ${Src})

target_include_directories(HolmLib PUBLIC .)


target_compile_definitions(HolmLib PRIVATE NDEBUG)


add_executable(TestHolm
        test_holm.cpp
        test_utils.cpp
        test_utils.h
        ${Src}
)


add_executable(TestTopTree ${Src}
        TestTopTree.cpp
)

























