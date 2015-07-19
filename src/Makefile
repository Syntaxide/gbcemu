all:
	g++ util.cc io.cc main.cc disas.cc memory.cc rom.cc inst_decode.cc cpu.cc -g -lSDL2 --std=c++11 -o emu

test_decoder :
	g++ util.cc inst_decode_test.cc inst_decode.cc -g --std=c++11
	./a.out
test_cpu:
	g++ -std=c++11 util.cc disas.cc rom.cc inst_decode.cc cpu_test.cc cpu.cc memory.cc -g
	./a.out

test : test_decoder test_cpu
	echo done

