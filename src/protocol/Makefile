all: synthserv.pb.cc

# synthserv.pb.cc: synthserv.proto
#	protoc -I. --cpp_out=. ./synthserv.proto

%.pb.cc: %.proto
	protoc -I. --cpp_out=. $*.proto

clean:
	rm -rf *.cc *.h
