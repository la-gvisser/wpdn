PLUGIN       = CefPreferencesOE
PLUGIN_DIR   = CefPreferencesOE
TARGZINDEX   = 0
TARGZGUID    = 3$(TARGZINDEX)3$(TARGZINDEX)3$(TARGZINDEX)3$(TARGZINDEX)3$(TARGZINDEX)3$(TARGZINDEX)
OMNI_DIR     = /usr/lib/omni
OUTPUT_DIR   = bin
OUTPUT       = $(OUTPUT_DIR)/$(PLUGIN).so
CPP          = g++
FLAGS        = -c -pthread -flto -fPIC -std=c++11 -pthread -Wall -Wno-non-virtual-dtor -Wno-multichar \
               -Wno-address-of-packed-member -g -march=nocona -O2
FLAGS_DBG    = -c -pthread -flto -fPIC -std=c++11 -pthread -Wall -Wno-non-virtual-dtor -Wno-multichar \
               -Wno-address-of-packed-member -g3 -ggdb3 -O0 -fno-inline -Wno-unused-function
MACROS       = -D_UNICODE -DUNICODE -DTARGET_OS_LINUX -D_NPROTOSPECS
INCLUDES     = -I./Helium \
               -I./Framework \
               -I./Plugin
LFLAGS       = -shared -flto -fPIC -pthread -Wl,-rpath,. -Wl,--version-script=module.map \
               -march=nocona -O2 -Llib -lheruntime
LFLAGS_DBG   = -shared -flto -fPIC -pthread -Wl,-rpath,. -Wl,--version-script=module.map \
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


zips:
	rm -f ./$(PLUGIN)-About.zip
	cd $(PLUG_SRC)/About/About$(ZIPINDEX); zip ../../../$(PLUGIN)-About.zip *
	rm -f ./$(PLUGIN)-CaptureTab.zip
	cd $(PLUG_SRC)/CaptureTab/CaptureTab$(ZIPINDEX); zip ../../../$(PLUGIN)-CaptureTab.zip *
	rm -f ./$(PLUGIN)-EngineTab.zip
	cd $(PLUG_SRC)/EngineTab/EngineTab$(ZIPINDEX); zip ../../../$(PLUGIN)-EngineTab.zip *
	rm -f ../$(PLUGIN)-Options.zip
	cd $(PLUG_SRC)/Options/Options$(ZIPINDEX); zip ../../../$(PLUGIN)-Options.zip *
	rm -f ./$(PLUGIN).tar.gz
	cd $(PLUG_SRC)/web; tar -zcf ../../C0E0F020-2468-1357-C1C4-$(TARGZGUID).tar.gz ./C0E0F020-2468-1357-C1C4-$(TARGZGUID)


.PHONY: prefs-1
prefs-1: PLUGIN = CefPrefsOE-1
prefs-1: PLUGIN_DIR = CefPrefsOE-1
prefs-1: CPPFLAGS = $(FLAGS) -DOE_SERIAL=1 $(MACROS) $(INCLUDES)
prefs-1: MAKEZIPS = makezips-1
prefs-1: ZIPINDEX = -1
prefs-1: TARGZINDEX = 1
prefs-1: zips
prefs-1: $(PLUGIN)


.PHONY: prefs-2
prefs-2: PLUGIN = CefPrefsOE-2
prefs-2: PLUGIN_DIR = CefPrefsOE-2
prefs-2: CPPFLAGS = $(FLAGS) -DOE_SERIAL=2 -DNO_ENGINE_TAB $(MACROS) $(INCLUDES)
prefs-2: MAKEZIPS = makezips-2
prefs-2: ZIPINDEX = -2
prefs-2: TARGZINDEX = 2
prefs-2: zips
prefs-2: $(PLUGIN)


.PHONY: prefs-3
prefs-3: PLUGIN = CefPrefsOE-3
prefs-3: PLUGIN_DIR = CefPrefsOE-3
prefs-3: CPPFLAGS = $(FLAGS) -DOE_SERIAL=3 $(MACROS) $(INCLUDES)
prefs-3: MAKEZIPS = makezips-3
prefs-3: ZIPINDEX = -3
prefs-3: TARGZINDEX = 3
prefs-3: zips
prefs-3: $(PLUGIN)


.PHONY: prefs-4
prefs-4: PLUGIN = CefPrefsOE-4
prefs-4: PLUGIN_DIR = CefPrefsOE-4
prefs-4: CPPFLAGS = $(FLAGS) -DOE_SERIAL=4 -DNO_ENGINE_TAB $(MACROS) $(INCLUDES)
prefs-4: MAKEZIPS = makezips-4
prefs-4: ZIPINDEX = -4
prefs-4: TARGZINDEX = 4
prefs-4: zips
prefs-4: $(PLUGIN)


.PHONY: clean
clean:
	rm -f $(PLUG_OBJS) $(FRAME_OBJS) $(DEPENDENCIES) $(OUTPUT)
	rm -f $(PLUG_SRC)/About.zip $(PLUG_SRC)/CaptureTab.zip \
		$(PLUG_SRC)/EngineTab.zip $(PLUG_SRC)/Options.zip 
	if [ -d "$(OUTPUT_DIR)" ]; then rmdir $(OUTPUT_DIR); fi
	if [ -d "$(OBJ_DIR)" ]; then rmdir $(OBJ_DIR); fi


.PHONY: debug
debug: CPPFLAGS = $(FLAGS_DBG) -D_DEBUG $(MACROS) $(INCLUDES)
debug: LDFLAGS = $(LFLAGS_DBG)
debug: $(PLUGIN)
	

.PHONY: install
install:
ifneq '$(wildcard $(OMNI_DIR)/plugins/$(PLUGIN).so)' ''
	rm $(OMNI_DIR)/plugins/$(PLUGIN).so
endif
ifneq '$(wildcard $(OMNI_DIR)/plugins/$(PLUGIN).he)' ''
	rm $(OMNI_DIR)/plugins/$(PLUGIN).he
endif
ifneq '$(wildcard $(OMNI_DIR)/plugins/$(PLUGIN)-About.zip)' ''
	rm $(OMNI_DIR)/plugins/$(PLUGIN)-About.zip
endif
ifneq '$(wildcard $(OMNI_DIR)/plugins/$(PLUGIN)-CaptureTab.zip)' ''
	rm $(OMNI_DIR)/plugins/$(PLUGIN)-CaptureTab.zip
endif
ifneq '$(wildcard $(OMNI_DIR)/plugins/$(PLUGIN)-EngineTab.zip)' ''
	rm $(OMNI_DIR)/plugins/$(PLUGIN)-EngineTab.zip
endif
ifneq '$(wildcard $(OMNI_DIR)/plugins/$(PLUGIN)-Options.zip)' ''
	rm $(OMNI_DIR)/plugins/$(PLUGIN)-Options.zip
endif
ifneq '$(wildcard $(OMNI_DIR)/plugins/$(PLUGIN).tar.gz)' ''
	rm $(OMNI_DIR)/plugins/$(PLUGIN).tar.gz
endif
	cp $(OUTPUT) $(OMNI_DIR)/plugins
	cp ./$(PLUGIN).he $(OMNI_DIR)/plugins
	chmod 755 $(OMNI_DIR)/plugins/$(PLUGIN).he
ifneq '$(wildcard ./Plugin/About.zip)' ''
	cp ./Plugin/About.zip $(OMNI_DIR)/plugins/$(PLUGIN)-About.zip
endif
ifneq '$(wildcard ./Plugin/CaptureTab.zip)' ''
	cp ./Plugin/CaptureTab.zip $(OMNI_DIR)/plugins/$(PLUGIN)-CaptureTab.zip
endif
ifneq '$(wildcard ./Plugin/EngineTab.zip)' ''
	cp ./Plugin/EngineTab.zip $(OMNI_DIR)/plugins/$(PLUGIN)-EngineTab.zip
endif
ifneq '$(wildcard ./Plugin/Options.zip)' ''
	cp ./Plugin/Options.zip $(OMNI_DIR)/plugins/$(PLUGIN)-Options.zip
endif
