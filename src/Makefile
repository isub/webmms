web_mms: web_mms.o
	gcc web_mms.o \
    -ggdb \
    -L/usr/local/lib \
    -lmmsbox \
    -lmms \
    -o web_mms
web_mms.o: web_mms.c
	gcc -c web_mms.c \
    -ggdb \
    -I/home/mmsc/distr/mbuni/mbuni-1.6.0 \
    -I/usr/local/include/kannel \
    -I/usr/include/libxml2 \
    -o web_mms.o
