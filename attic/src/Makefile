PROJECT_ROOT = $(abspath ..)

all:
	make -C api
	make -C server
	make -C manager

clean:
	make -k -C api clean
	make -k -C server clean
	make -k -C manager clean
