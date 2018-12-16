SUBDIRS=./Lionel ./McTavish ./McGruder

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: $(SUBDIRS)
