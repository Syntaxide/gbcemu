all:
	g++ main.cc rom.cc inst_decode.cc cpu.cc -g --std=c++11 -o emu

test_decoder :
	g++ inst_decode_test.cc inst_decode.cc -g --std=c++11;
	./a.out

test : test_decoder
	echo done

