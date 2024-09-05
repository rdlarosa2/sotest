cc ./src/sotest.c -o ./bin/sotest
gcc -c -fPIC ./src/functionsOneAndTwo.c -o ./bin/functionsOneAndTwo.o
gcc -c -fPIC ./src/functionsThree.c -o ./bin/functionsThree.o
gcc -c -fPIC ./src/functionsFour.c -o ./bin/functionsFour.o
gcc ./bin/functionsOneAndTwo.o -shared -o ./bin/libfunctionsOneAndTwo.so
gcc ./bin/functionsThree.o -shared -o ./bin/libfunctionsThree.so
