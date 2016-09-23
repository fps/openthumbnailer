.PHONY: all

all: openthumbnailer

openthumbnailer: main.cc
	g++ -Wall -Werror -fPIC -o openthumbnailer main.cc -lboost_program_options `pkg-config opencv --libs`
