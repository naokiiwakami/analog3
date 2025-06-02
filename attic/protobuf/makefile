all: nano_compact_descriptor.pb.c

nano_compact_descriptor.pb.c : compact_descriptor.proto
	protoc -I. -o nano_compact_descriptor.pb compact_descriptor.proto
	python ../nanopb/generator/nanopb_generator.py nano_compact_descriptor.pb

clean:
	rm -f *~ *.c *.h *.pb
