web_mms: web_mms.o web_mms_oper.o
	gcc web_mms.o web_mms_oper.o \
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
web_mms_oper.o: web_mms_oper.c
	gcc -c web_mms_oper.c \
    -ggdb \
    -I/home/mmsc/distr/mbuni/mbuni-1.6.0 \
    -I/usr/local/include/kannel \
    -I/usr/include/libxml2 \
    -o web_mms_oper.o
install:
	cp -f web_mms /usr/local/bin