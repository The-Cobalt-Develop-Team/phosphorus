enable_testing()

find_package(GTest REQUIRED)

set(PHOSPHORUS_TEST_SOURCE
        "${PHOSPHORUS_TEST_DIR}/FieldTest.cpp"
        "${PHOSPHORUS_TEST_DIR}/VerletIntegratorTest.cpp")

message(STATUS "PHOSPHORUS_TEST_SOURCE: ${PHOSPHORUS_TEST_SOURCE}")

add_executable(PhosphorusTest ${PHOSPHORUS_TEST_SOURCE})
target_link_libraries(PhosphorusTest
        PRIVATE
        phosphorus
        GTest::GTest
        GTest::Main
)