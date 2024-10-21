PROG     = cJSON
OBJS     = $(PROG).c
MAJORVER = 1
MINORVER = 0
PATCH    = 0
TARGET   = libcustom.so.$(MAJORVER).$(MINORVER).$(PATCH)

CFLAGS += -I.
CFLAGS += -Wall \
	  -Wformat=2 \
	  -Wpointer-arith \
	  -Wbad-function-cast \
	  -Wstrict-prototypes \
	  -Wmissing-prototypes \
	  -Winline \
	  -Wdisabled-optimization \
	  -Wfloat-equal \
	  -W \
	  -fPIC
	  # -Werror \ TODO: for now don't treat warnings as errors
	  # the included cJSON src uses a different C standard and throws errors
      # next step would be to update cJSON to follow the strict C standard used

all: $(TARGET) custom_link

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -shared $^ -o $@

custom_link:
	ln -sf libcustom.so.$(MAJORVER).$(MINORVER).$(PATCH) libcustom.so.$(MAJORVER)
	ln -sf libcustom.so.$(MAJORVER) libcustom.so

clean:
	rm -rf libcustom*