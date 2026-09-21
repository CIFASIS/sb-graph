add_executable(
    install-consumer-test
    consumer.cpp
)

target_link_libraries(
    install-consumer-test
    PRIVATE
      SBG::sbgraph
)

add_custom_target(run-install-consumer-test
    DEPENDS install-consumer-test
)
