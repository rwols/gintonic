cmake_minimum_required(VERSION 2.8)

install(
	DIRECTORY .
	DESTINATION include/gintonic 
	FILES_MATCHING 
		PATTERN "*.hpp"
		PATTERN "*.h"
		PATTERN "*.vert" 
		PATTERN "*.geom"
		PATTERN "*.frag"
)
