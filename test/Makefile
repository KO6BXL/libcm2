#define output files
test_o := test
3cubed_o := 3cubed

#all target
all: test 3cubed

#simple test
test: test.c
	cc test.c -g -L../build -lcm2 -g -o ${test_o}
#more complex test: 100x100x100 cube
3cubed: 3cubed.c
	cc 3cubed.c -g -L../build -lcm2 -g -o ${3cubed_o}


#clean
clean:
	rm -f ${test_o}
	rm -f ${3cubed_o}