include_directories(src)

add_executable(atc-compressed-template src/compressed-template.cc)
target_link_libraries(atc-compressed-template
  PUBLIC
  PRIVATE
  FairMQ::FairMQ
  AliceO2::Framework
  AliceO2::Headers
  AliceO2::TOFReconstruction
  ROOT::Core
  ROOT::Hist
  ms_gsl::ms_gsl
  arrow::arrow_shared
  arrow::gandiva_shared
  )
install(TARGETS atc-compressed-template RUNTIME DESTINATION bin)
