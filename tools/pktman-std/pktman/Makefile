# Makefile for pktman

executable      = pktman
bin_dir         = bin
bin             = $(bin_dir)/$(executable)
cpp             = g++

src_dir         = .
capture_src_dir = $(src_dir)/CaptureFile
obj_dir         = obj
main_objs       = $(patsubst %.cpp,$(obj_dir)/%.o,$(subst $(src_dir)/,,$(wildcard $(src_dir)/*.cpp))) 
capture_objs    = $(patsubst %.cpp,$(obj_dir)/%.o,$(subst $(capture_src_dir)/,,$(wildcard $(capture_src_dir)/*.cpp))) 
VPATH           = $(src_dir) $(capture_src_dir)

c_flags         = -c -lstdc++fs -std=c++17 -Wall -Wno-multichar -g -O2 
c_flags_dbg     = -c -lstdc++fs -std=c++17 -Wall -Wno-multichar -g3 -ggdb3 -O0 
macros          = -D_LINUX -D_UNICODE -DUNICODE
includes        = -I$(src_dir) \
                  -I$(capture_src_dir)
l_flags         = -O2
l_flags_dbg     = -O0
cpp_flags       = $(c_flags) $(macros) $(includes)
ld_flags        = $(l_flags)

all: $(executable) 

$(main_objs): $(obj_dir)/%.o: %.cpp
	$(cpp) $(cpp_flags) -o $@ $<

$(capture_objs): $(obj_dir)/%.o: %.cpp
	$(cpp) $(cpp_flags) -o $@ $<

$(main_objs) $(capture_objs): | $(obj_dir)

$(obj_dir):
	mkdir $(obj_dir)

$(bin): | $(bin_dir)

$(bin_dir):
	mkdir $(bin_dir)

$(executable): $(capture_objs) $(main_objs) $(bin)
	$(cpp) -o $(bin) $(ld_flags) $(main_objs) $(capture_objs)

.PHONY: clean
clean:
	rm -f $(main_objs) $(capture_objs) $(output)
	rm -f $(bin_dir)/$(executable)
	if [ -d "$(obj_dir)" ]; then rmdir $(obj_dir); fi
	if [ -d "$(bin_dir)" ]; then rmdir $(bin_dir); fi

.PHONY: debug
debug:
debug: cpp_flags = $(c_flags_dbg) -D_DEBUG $(macros) $(includes)
debug: ld_flags = $(l_flags_dbg)
debug: $(executable)
