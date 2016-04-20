CC = gcc
CFLAGS = -L$(LDIR) -I$(IDIR) -g `pkg-config --cflags gtk+-3.0` -DNDEBUG 
LDFLAGS = -lpthread -lircredes -lircinterface -lsoundredes -lredes2-G-2301-01-P2 -lsoundredes -lpulse -lpulse-simple `pkg-config --libs gtk+-3.0` -rdynamic
AR = ar 

TAR_FILE= G-2301-01-P2.tar.gz
SDIR = src
SLDIR = srclib
IDIR = includes
LDIR = lib
ODIR = obj
MDIR = man
DDIR = doc
BDIR = .

_LIB = libredes2-G-2301-01-P2.a
LIB = $(patsubst %,$(LDIR)/%,$(_LIB))

_LOBJ = G-2301-01-P1-daemon.o G-2301-01-P1-tcp.o G-2301-01-P1-udp.o G-2301-01-P2-rtp.o G-2301-01-P1-tools.o G-2301-01-P2-ucomm.o G-2301-01-P2-ccomm.o
LOBJ = $(patsubst %,$(ODIR)/%,$(_LOBJ))

_DEPS = 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = xchat2.o soundtest.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_BIN = xchat2 soundtest
BIN = $(patsubst %,$(BDIR)/%,$(_BIN))

all: $(BIN) readme printopt

printopt:
	@echo ">>>> Opciones del makefile <<<<"
	@echo "'make' para construir"
	@echo "'make run' para ejecutar el programa"
	@echo "'make clean' para limpiar el entorno"
	@echo "'make doc' para generar la documentacion"
	@echo "'make compress' para comprimir la práctica"
$(LIB): $(LOBJ)
	$(AR) rcv $@ $^

$(LOBJ):$(ODIR)/%.o: $(SLDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ):$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

$(BIN):%: $(ODIR)/%.o $(LIB)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

compress: clean doc
	rm -rf $(TAR_FILE)
	rm -rf G-2301-01-P2
	tar -zcvf ../$(TAR_FILE) ../G-2301-01-P2
	mv ../$(TAR_FILE) $(TAR_FILE)

doc: clean 
	doxygen
	@cat Readme

run: xchat2
	./xchat2	

log: 
	tail -F /var/log/syslog | grep irc

readme: 
	@cat Readme
.PHONY: clean
clean:
	@rm -frv $(BIN) $(LIB) $(OBJ) $(LOBJ) 
	@mkdir -p obj lib
	@rm -fv $(TAR_FILE)
	@rm -fv core vgcore*  
