CC = g++
BIN_DIR = bin
OUT_CLIENT = bin/client
OUT_SERVER = bin/server
OBJ_DIR = obj
INC_DIR = include
INCLUDE = -iquote $(INC_DIR)
SRC_DIR = src
DEP_DIR = .dep
DEBUG = -g
DEFS = -D dDOUBLE
GUILE_CFLAGS = `guile-config compile`
GUILE_LFLAGS = `guile-config link`
CFLAGS = -Wall -std=c++11 $(DEBUG) $(DEFS) $(GUILE_CFLAGS)
LFLAGS = $(GUILE_LFLAGS)
LIBS_CLIENT = -l boost_system -l boost_timer -l ode -l osg -l osgDB -l osgGA -l osgViewer -l pthread
LIBS_SERVER = -l boost_regex -l boost_system -l boost_timer -l ode -lpthread

MAKEDEPEND = gcc -MM -std=c++11 $(INCLUDE)
_OBJS_CLIENT = client.o ClientApp.o ClientScm.o Object.o ObjectNode.o TcpClient.o World.o WorldNode.o
_OBJS_SERVER = server.o App.o Object.o ServerScm.o TcpServer.o World.o
SUBDIRS =
OBJS_CLIENT = $(patsubst %,$(OBJ_DIR)/%,$(_OBJS_CLIENT))
OBJS_SERVER = $(patsubst %,$(OBJ_DIR)/%,$(_OBJS_SERVER))
DEPS_CLIENT = $(patsubst %.o,$(DEP_DIR)/%.d,$(_OBJS_CLIENT))
DEPS_SERVER = $(patsubst %.o,$(DEP_DIR)/%.d,$(_OBJS_SERVER))

.PHONY: all
all: directories client server

.PHONY: directories
directories:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(DEP_DIR)
ifdef SUBDIRS
	@mkdir -p $(foreach subdir,$(SUBDIRS),$(OBJ_DIR)/$(subdir))
	@mkdir -p $(foreach subdir,$(SUBDIRS),$(DEP_DIR)/$(subdir))
endif

$(OUT_CLIENT): $(OBJS_CLIENT)
	$(CC) -o $@ $^ $(LFLAGS) $(LIBS_CLIENT)

$(OUT_SERVER): $(OBJS_SERVER)
	$(CC) -o $@ $^ $(LFLAGS) $(LIBS_SERVER)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MAKEDEPEND) -MF $(DEP_DIR)/$*.d -MT $(OBJ_DIR)/$*.o -MP $(SRC_DIR)/$*.cpp $(DEFS)
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

-include $(DEPS)

.PHONY: client
client: directories $(OUT_CLIENT)

.PHONY: server
server: directories $(OUT_SERVER)

.PHONY: run-client
run-client: $(OUT_CLIENT)
	$(OUT_CLIENT)

.PHONY: run-server
run-server: $(OUT_SERVER)
	$(OUT_SERVER)

.PHONY: clean
clean:
	rm -f $(OUT_CLIENT)
	rm -f $(OUT_SERVER)
	rm -rf $(OBJ_DIR)/* $(DEP_DIR)/*

.PHONY: cleanbackups
cleanbackups:
	find . -name '*~' -delete

.PHONY: cleanall
cleanall: clean cleanbackups
