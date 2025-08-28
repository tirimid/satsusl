.PHONY: all

TTF_FILES := $(shell find . -name *.ttf)
TTF_HEADERS := $(patsubst %.ttf,%_ttf.h,$(TTF_FILES))

all: $(TTF_HEADERS)

%_ttf.h: %.ttf
	xxd -i -n $< $< $@
