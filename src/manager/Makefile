PROJECT_ROOT = $(abspath ../..)
COMPONENT_ROOT = $(abspath .)
-include $(PROJECT_ROOT)/project.mk

TARGET = manager

OBJS = \
	app.o

all:
	make $(TARGET)
#	make -w -C $(COMPONENT_ROOT)/test

manager: main.o $(OBJS) $(API)/liba3api.a
	$(LD) $(LDFLAGS) -o manager main.o $(OBJS) $(APP_LIBS)

-include $(OBJS:.o=.d)

clean:
	rm -rf $(CLEANFILES)

cleanest:
	rm -rf $(CLEANESTFILES)
