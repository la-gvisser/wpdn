PLUGIN       = FolderAdapterOE
PLUGIN_DIR   = FolderAdapter
OMNI_DIR     = /usr/lib/omni
OUTPUT_DIR   = bin
OUTPUT       = $(OUTPUT_DIR)/$(PLUGIN).so
CPP          = g++
FLAGS        = -c -pthread -flto -fPIC -std=c++11 -pthread -Wall -Wno-non-virtual-dtor -Wno-multichar \
               -g -march=nocona -O2
FLAGS_DBG    = -c -pthread -flto -fPIC -std=c++11 -pthread -Wall -Wno-non-virtual-dtor -Wno-multichar \
               -g3 -ggdb3 -O0 -fno-inline -Wno-unused-function
MACROS       = -D_UNICODE -DUNICODE -DTARGET_OS_LINUX -D_NPROTOSPECS
INCLUDES     = -I./Helium \
               -I./Framework \
               -I./Plugin
LFLAGS       = -shared -flto -fPIC -pthread -Wl,-rpath,. -Wl,--version-script=module.map \
               -march=nocona -O2 -Llib -lheruntime
LFLAGS_DBG   = -g -shared -flto -fPIC -pthread -Wl,-rpath,. -Wl,--version-script=module.map \
               -Llib -lheruntime
CPPFLAGS     = $(FLAGS) $(MACROS) $(INCLUDES)
LDFLAGS      = $(LFLAGS)

# -shared -Wl -Llib -lheruntime -lz -lpthread -Wl,-soname,$(PLUGIN).so

PLUG_SRC     = ./Plugin
FRAME_SRC    = ./Framework
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
	$(CPP) -o $(OUTPUT) $(LDFLAGS) $(PLUG_OBJS) $(FRAME_OBJS)


.PHONY: clean
clean:
	rm -f $(PLUG_OBJS) $(FRAME_OBJS) $(DEPENDENCIES) $(OUTPUT)
	rm -f $(PLUG_SRC)/Options.zip 
	if [ -d "$(OUTPUT_DIR)" ]; then rmdir $(OUTPUT_DIR); fi
	if [ -d "$(OBJ_DIR)" ]; then rmdir $(OBJ_DIR); fi


.PHONY: debug
debug: CPPFLAGS = $(FLAGS_DBG) -D_DEBUG $(MACROS) $(INCLUDES)
debug: LDFLAGS = $(LFLAGS_DBG)
debug: $(PLUGIN)
	

.PHONY: zips
zips:
	cd $(PLUG_SRC)/CaptureTab; rm -f ../$(PLUGIN)-CaptureTab.zip; zip ../$(PLUGIN)-CaptureTab.zip *
	cd $(PLUG_SRC)/Options; rm -f ../$(PLUGIN)-Options.zip; zip ../$(PLUGIN)-Options.zip *


.PHONY: install
install:
ifneq '$(wildcard $(OMNI_DIR)/plugins/$(PLUGIN).so)' ''
	rm $(OMNI_DIR)/plugins/$(PLUGIN).so
endif
ifneq '$(wildcard $(OMNI_DIR)/plugins/$(PLUGIN).he)' ''
	rm $(OMNI_DIR)/plugins/$(PLUGIN).he
endif
ifneq '$(wildcard $(OMNI_DIR)/plugins/$(PLUGIN)-CaptureTab.zip)' ''
	rm $(OMNI_DIR)/plugins/$(PLUGIN)-CaptureTab.zip
endif
ifneq '$(wildcard $(OMNI_DIR)/plugins/$(PLUGIN)-Options.zip)' ''
	rm $(OMNI_DIR)/plugins/$(PLUGIN)-Options.zip
endif
	cp $(OUTPUT) $(OMNI_DIR)/plugins
	cp ./$(PLUGIN).he $(OMNI_DIR)/plugins
	chmod 755 $(OMNI_DIR)/plugins/$(PLUGIN).he
ifneq '$(wildcard ./Plugin/$(PLUGIN)-CaptureTab.zip)' ''
	cp ./Plugin/$(PLUGIN)-CaptureTab.zip $(OMNI_DIR)/plugins/$(PLUGIN)-CaptureTab.zip
endif
ifneq '$(wildcard ./Plugin/$(PLUGIN)-Options.zip)' ''
	cp ./Plugin/$(PLUGIN)-Options.zip $(OMNI_DIR)/plugins/$(PLUGIN)-Options.zip
endif

