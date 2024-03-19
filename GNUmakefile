CXXFLAGS += -Wall -W -std=c++11 -O2
CPPFLAGS += -I/opt/mbedtls/include
LDFLAGS += -L/opt/mbedtls/lib
LDLIBS += -lmbedcrypto -lmbedtls

TARGETS = $(addprefix bin/,$(notdir $(basename $(wildcard src/*.cpp))))

all:: $(TARGETS)

clean::
	$(RM) $(TARGETS)

bin/%: src/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH) $^ $(LOADLIBES) $(LDLIBS) -o $@
