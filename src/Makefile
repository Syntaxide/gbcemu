all:
	gcc main.cc -o emu

test:
	for i in *_test.cc; do echo "making $$i";rm a.out;g++ --std=c++11 $$i; ./a.out; done
