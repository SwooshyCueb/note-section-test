add_library(note-section-module
	MODULE
	"${CMAKE_CURRENT_SOURCE_DIR}/src/thing.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/dummy.cpp")

target_link_options(note-section-module
	PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/src/note_section.ld"
	"-Wl,-z,global")

set_property(TARGET
	note-section-module
	APPEND
	PROPERTY LINK_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/note_section.ld")
