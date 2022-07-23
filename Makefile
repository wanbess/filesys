all:
	mkdir -p build && cd build && cmake .. && make

clean:
	 rm src/default.fs && rm test/test.log && rm test/default.fs &&rm -r build/ 
