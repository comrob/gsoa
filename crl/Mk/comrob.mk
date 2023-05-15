#
# Date:      2014/07/25 10:17
# Author:    Jan Faigl
#

uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

CXXFLAGS:=$(call uniq,$(CXXFLAGS))
CPPFLAGS:=$(call uniq,$(CPPFLAGS))
LDFLAGS:=$(call uniq,$(LDFLAGS))

LIBRARY_PREFIX=$(INSTALL_PREFIX)/lib$(INSTALL_SUFFIX)
INCLUDE_PREFIX=$(INSTALL_PREFIX)/include

dist: install

lib: $(LIBRARY)

$(LIBRARY_OBJS): %.o: %.cc
	$(CXX) -c $< $(CXXFLAGS) $(CPPFLAGS) -o $@

$(LIBRARY): $(LIBRARY_OBJS)
	mkdir -p $(LIBRARY_PREFIX)
	$(AR) rcs $(LIBRARY_PREFIX)/$@ $(LIBRARY_OBJS)

clean:
	$(RM) $(LIBRARY_OBJS)

