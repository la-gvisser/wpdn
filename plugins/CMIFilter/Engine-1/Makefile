PLUGIN       = CMIFilterOE
PLUGIN_DIR   = CMIFilter
OMNI_DIR     = ~/p4/wp/branches/omni/8.0.0/src/omni/bin
OUTPUT_DIR   = bin
OUTPUT       = $(OUTPUT_DIR)/$(PLUGIN).so
CPP          = g++
FLAGS        = -c -pthread -Wall -fPIC -std=c++0x -Wno-non-virtual-dtor -Wno-multichar \
               -march=nocona
FLAGS_DBG    = -c -pthread -Wall -fPIC -std=c++0x -Wno-non-virtual-dtor -Wno-multichar \
               -g3 -ggdb3 -O0 -fno-inline -Wno-unused-function
MACROS       = -D_UNICODE -DUNICODE -DTARGET_OS_LINUX -D_NPROTOSPECS
INCLUDES     = -I$(PLUGIN_DIR)/Helium \
               -I$(PLUGIN_DIR)/Framework \
               -I$(PLUGIN_DIR)/Plugin
CPPFLAGS     = $(FLAGS) $(MACROS) $(INCLUDES)
LDFLAGS      = $(LFLAGS) -module -pthread -Wl,-rpath,. -Wl,--version-script=module.map \
               -Llib -lheruntime lib/peekcore.so

# -shared -Wl -Llib -lheruntime -lz -lpthread -Wl,-soname,$(PLUGIN).so

PLUG_SRC     = $(PLUGIN_DIR)/Plugin
FRAME_SRC    = $(PLUGIN_DIR)/Framework
OBJ_DIR      = obj
PLUG_OBJS    = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(subst $(PLUG_SRC)/,,$(wildcard $(PLUG_SRC)/*.cpp))) 
FRAME_OBJS   = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(subst $(FRAME_SRC)/,,$(wildcard $(FRAME_SRC)/*.cpp))) 
VPATH        = $(PLUG_SRC) $(FRAME_SRC)

all: $(PLUGIN)


$(PLUG_OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CPP) $(CPPFLAGS) -o $@ $<

$(FRAME_OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CPP) $(CPPFLAGS) -o $@ $<


$(PLUG_OBJS) $(FRAME_OBJS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OUTPUT): | $(OUTPUT_DIR)

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR)

$(PLUGIN): $(PLUG_OBJS) $(FRAME_OBJS) $(OUTPUT)
	$(CPP) -shared -o $(OUTPUT) $(LDFLAGS) $(PLUG_OBJS) $(FRAME_OBJS)


.PHONY: clean
clean:
	rm -f $(PLUG_OBJS) $(FRAME_OBJS) $(DEPENDENCIES) $(OUTPUT)
	rmdir $(OUTPUT_DIR)
	rmdir $(OBJ_DIR)


.PHONY: debug
debug: CPPFLAGS = $(FLAGS_DBG) -D_DEBUG $(MACROS) $(INCLUDES)
debug: $(PLUGIN)
	

.PHONY: install
install:
ifneq '$(wildcard $(OMNI_DIR)/$(PLUGIN).so)' ''
	rm $(OMNI_DIR)/$(PLUGIN).so
endif
ifneq '$(wildcard $(OMNI_DIR)/$(PLUGIN).he)' ''
	rm $(OMNI_DIR)/$(PLUGIN).he
endif
	cp $(OUTPUT) $(OMNI_DIR)
	cp ./$(PLUGIN_DIR)/$(PLUGIN).he $(OMNI_DIR)
	chmod 755 $(OMNI_DIR)/$(PLUGIN).he

