all: nano_module_descriptor.pb.c

nano_module_descriptor.pb.c : ../protobuf/module_descriptor.proto
	protoc -I../protobuf -o nano_module_descriptor.pb ../protobuf/module_descriptor.proto
	python ../nanopb/generator/nanopb_generator.py nano_module_descriptor.pb

clean:
	rm -f *~ *.c *.h *.pb
