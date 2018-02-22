PROJECT_ROOT = $(abspath ..)

all:
	make -C protocol
	make -C server
	make -C manager

clean:
	make -k -C protocol clean
	make -k -C server clean
	make -k -C manager clean
