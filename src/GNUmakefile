CPPFLAGS += -I/opt/mbedtls/include
CXXFLAGS += -Wall -W -std=c++11 -g -O2
LDFLAGS += -L/opt/mbedtls/lib
LDLIBS += -lmbedcrypto -lmbedtls

TARGETS = aes128gcm

all:: $(TARGETS)

clean::
	$(RM) -r $(TARGETS) *.dSYM
