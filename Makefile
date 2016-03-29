CC = gcc
CFLAGS = -L$(LDIR) -I$(IDIR) -g `pkg-config --cflags gtk+-3.0`  
LDFLAGS = -lpthread -lircredes -lircinterface -lsoundredes -lredes2-G-2301-01-P1 `pkg-config --libs gtk+-3.0` -rdynamic
AR = ar 

TAR_FILE= G-2301-01-P1.tar.gz
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

_LOBJ = G-2301-01-P1-daemon.o G-2301-01-P1-tcp.o G-2301-01-P1-udp.o G-2301-01-P1-server.o G-2301-01-P1-irc.o G-2301-01-P1-tools.o G-2301-01-P2-ucomm.o
LOBJ = $(patsubst %,$(ODIR)/%,$(_LOBJ))

_DEPS = 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = xchat2.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_BIN = xchat2
BIN = $(patsubst %,$(BDIR)/%,$(_BIN))

all: $(BIN)

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
	rm -rf G-2301-01-P1
	tar -zcvf ../$(TAR_FILE) ../G-2301-01-P1
	mv ../$(TAR_FILE) $(TAR_FILE)

doc: 
	doxygen

run: xchat2
	./xchat2	

log: 
	tail -F /var/log/syslog | grep irc

.PHONY: clean
clean:
	@rm -frv $(BIN) $(LIB) $(OBJ) $(LOBJ) 
	@mkdir -p obj lib
	@rm -fv $(TAR_FILE)
	@rm -fv core vgcore*  
