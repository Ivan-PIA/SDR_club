cmake_minimum_required(VERSION 3.16)

project(soapy_pluto_sdr_timestamp)

find_package(SoapySDR REQUIRED)

if (SoapySDR_FOUND)
    message(STATUS "SoapySDR_INCLUDE_DIRS: ${SoapySDR_INCLUDE_DIRS}")
    message(STATUS "SoapySDR_LIBRARIES: ${SoapySDR_LIBRARIES}")
    message(STATUS "SoapySDR_DEFINITIONS: ${SoapySDR_DEFINITIONS}")
endif()

set(TX
    # audio_trx.cpp
    src/device/soapy_api.cpp 
    src/phy/filter.cpp
    src/phy/mapper.cpp
    # test/test_bpsk_trx.cpp
    test/tx.cpp
    # test/test_upsample.cpp
)

set(RX
    # audio_trx.cpp
    src/device/soapy_api.cpp 
    src/phy/filter.cpp
    src/phy/mapper.cpp
    # test/test_bpsk_trx.cpp
    test/rx.cpp
    # test/test_upsample.cpp
)

set(TRX
    # audio_trx.cpp
    src/device/soapy_api.cpp 
    src/phy/filter.cpp
    src/phy/mapper.cpp
    test/test_bpsk_trx.cpp
    # test/rx.cpp
    # test/test_upsample.cpp
)


add_executable(tx ${TX})
add_executable(rx ${RX})
add_executable(trx ${TRX})

target_link_libraries(tx ${SoapySDR_LIBRARIES})
target_link_libraries(trx ${SoapySDR_LIBRARIES})
target_link_libraries(rx ${SoapySDR_LIBRARIES})