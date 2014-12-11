ifeq ($(shell uname), Linux)
	TARGET_DIR = aufg25_linux
else
	TARGET_DIR = aufg25_windows
endif

all: aufg25

.PHONY aufg25:
	cd $(TARGET_DIR) && make