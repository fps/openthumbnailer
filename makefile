.PHONY: all

all: openthumbnailer

README.md: openthumbnailer README.md.in
	cat README.md.in | regexec -e "HELP_OUTPUT" -c "./openthumbnailer -h" -n 1 > README.md

openthumbnailer: main.cc
	g++ -std=c++11 -Wall -Werror -fPIC -o openthumbnailer main.cc -lboost_program_options `pkg-config opencv --libs`
