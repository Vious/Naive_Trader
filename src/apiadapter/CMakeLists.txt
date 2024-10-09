
include_directories(${CMAKE_INCLUDE_PATH})
link_directories(${CMAKE_LIBRARY_PATH})

aux_source_directory(market MARKET_DIR)
aux_source_directory(trade TRADER_DIR)

if(UNIX)
    add_definitions("-fPIC")
    # set(LINK_LIBS dl)
endif()


