mkdir build 

cd build 

cmake ..

make

cmake -DEXECUTE_FILE=test.c .. && cmake --build . --target run
***
test/test.c为原代码，test/test.c.ll为中间代码
