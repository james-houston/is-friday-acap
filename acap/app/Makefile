PROG1	= friday_event
OBJS1	= $(PROG1).c
PROGS	= $(PROG1)

# Include ACAP native packages glib and axevent
PKGS = glib-2.0 axevent
CFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags $(PKGS))
LDLIBS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs $(PKGS))
# Build the application with the libraries previously built using the -l
# option and point out the library directory under the application directory
# with -L option.
# Setting rpath is required for the installed application to search the
# libraries in the application directory and not in AXIS OS standard path.
LDLIBS += -lssl -lcrypto -lcurl
LDFLAGS = -Llib -Wl,--no-as-needed,-rpath,'$$ORIGIN/lib'
SHLIBS += -lcustom

# Since OpenSSL build files are installed to the SDK library path, those header
# files are found from the SDK environment variables, but curl header files
# need to get pointed out
CFLAGS += -I/opt/build/curl/install/include/curl

# Include custom cJSON build
CFLAGS += -Icjson

# Set warning flags
CFLAGS += -Wall \
          -Wextra \
          -Wformat=2 \
          -Wpointer-arith \
          -Wbad-function-cast \
          -Wstrict-prototypes \
          -Wmissing-prototypes \
          -Winline \
          -Wdisabled-optimization \
          -Wfloat-equal \
          -W \
          #-Werror TODO: For now we don't want to treat warnings as errors
          # the included cJSON src uses a different C standard and throws errors
          # next step would be to update cJSON to follow the strict C standard used

# Pre-process debug setting if it's set in the Dockerfile
ifneq ($(strip $(APP_DEBUG)),)
CFLAGS += -DAPP_DEBUG="\"$(APP_DEBUG)\""
endif

# Pre-process proxy setting if it's set in the Dockerfile
ifneq ($(strip $(APP_PROXY)),)
CFLAGS += -DAPP_PROXY="\"$(APP_PROXY)\""
endif

all:	$(PROGS)

$(PROG1): $(OBJS1)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS)  $^ $(LIBS)  $(SHLIBS) -o $@

clean:
	rm -f $(PROGS) *.o *.eap *_LICENSE.txt package.conf* param.conf
