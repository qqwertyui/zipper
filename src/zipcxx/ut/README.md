## Unit test directory
This directory holds tests for zipcxx parts. Every test should have its directory which should fulfill following requirements:
- its name has to begin with Test*
- file containing test entry point must have the same name as directory followed with .cpp extension

## How to add unit test
- add test subdirectory in CMakeLists.txt
- copy template from TemplateTest and rename it correctly (rules above)
- adjust CMakeLists.txt to your needs