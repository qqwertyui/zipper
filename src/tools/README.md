## Tools directory
This directory holds tools for developing zipcxx library. It may contain various subprojects that may speed up debugging.

Contents:
- zipviewer - utility for reading LFH and CDFH fields, reduces need of using hexeditor while inspecting various header fields. Without it one would need to: manually find LFH and calculate offset to needed values. Zip saves values as little endian so programmer would need to take endianess into account to read values correctly.