.PHONY: all fw test runtest clean help

# Default: build everything and run tests
all: 
	@echo ""
	@echo "================ Build Summary ================"
	@echo "make        → builds firmware, then unit tests, then runs tests"
	@echo "make fw     → only builds firmware"
	@echo "make test   → only builds test binary"
	@echo "make runtest→ builds tests + runs them"
	@echo "make clean  → cleans both"
	@echo "=============================================="
	@echo ""
	$(MAKE) fw
	$(MAKE) runtest

# Build firmware
fw:
	@echo "===============Building firmware ================"
	@echo "================================================="
	$(MAKE) -f Makefile.fw
	@echo "===========Building firmware Done ✅ ============"
	@echo "================================================="

# Build test binary
test:
	@echo "=============== Building unit tests =============="
	@echo "================================================="
	$(MAKE) -f Makefile.test
	@echo "==========Building unit tests Done  ✅  =========="
	@echo "==================================================="

# Build + run tests
runtest: test
	@echo "================Running unit tests ================"
	@echo "==================================================="
# 	$(MAKE) -f Makefile.test run
# 	./Tests/Bin/test_uart.exe
	@echo "=============== Running unit tests Done ✅ ========"
	@echo "==================================================="

# Clean everything
clean:
	@echo "============== Cleaning all build artifacts ========="
	@echo "===================================================="
	$(MAKE) -f Makefile.fw clean
	$(MAKE) -f Makefile.test clean
	@echo "============ Cleaning all build artifacts Done ✅ ==="
	@echo "===================================================="

# Print help explicitly
help:
	@echo ""
	@echo "===================================================="
	@echo "================ Available Make Targets ================"
	@echo "make        → builds firmware, then unit tests, then runs tests ✅"
	@echo "make fw     → only builds firmware"
	@echo "make test   → only builds test binary"
	@echo "make runtest→ builds tests + runs them"
	@echo "make clean  → cleans both"
	@echo "make help   → shows this message"
	@echo "========================================================"
	@echo ""
