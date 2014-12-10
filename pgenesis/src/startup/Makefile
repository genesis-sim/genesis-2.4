
INSTALL			=	$(INST_DIR)
INSTALL_STARTUPS	=	$(INSTALL)/startup

OBJS = pschedule.g minpschedule.g pvm-cshrc.stub

#==========================================================================
default: 
	echo "nothing to really make in startup"

clean:


install:
	-chmod +x configrc configdir
	-(for i in $(OBJS); do cat $$i > $(INSTALL_STARTUPS)/$$i; done )
	-(for i in psimrc nxpsimrc minpsimrc; do \
		./configrc $$i $(INSTALL) $(GEN_ROOT) $(PVM_ROOT) > $(INSTALL_STARTUPS)/.$$i; done)
	-( if [ -f $(INSTALL)/bin/pgenesis ]; then \
		mv $(INSTALL)/bin/pgenesis $(INSTALL)/bin/pgenesis.prev; \
	   fi)
	-( cat pgenesis.dist | \
		sed -e '/MPI_CMD/r mpicmd' | \
		sed -e '/MPI_DEBUG_CMD/r mpidebugcmd' | \
		./configdir _pgenesis-src_	$(PARSRC_DIR) | \
		./configdir _pgenesis-inst_	$(INSTALL) | \
		./configdir _genesis_		$(GEN_ROOT) | \
		./replace   _use_mpi_		0$(USE_MPI) | \
		./replace   _use_pvm_		0$(USE_PVM) | \
		./configdir _pvm_default_	$(PVM_ROOT) | \
		./replace   _pvm_arch_		$(PVM_ARCH) \
		> $(INSTALL)/bin/pgenesis )
	-( chmod +x $(INSTALL)/bin/pgenesis)
	-( cp $(INSTALL)/bin/pgenesis $(INSTALL)/lib/pgenesis.ext )
	-( ./configrc ttydebugger $(INSTALL) $(GEN_ROOT) $(PVM_ROOT) > $(INSTALL)/lib/ttydebugger)
	-( ./configrc dbxdebugger $(INSTALL) $(GEN_ROOT) $(PVM_ROOT) > $(INSTALL)/lib/dbxdebugger)
	-( cat gdbdebugger | \
		./configdir _pgenesis-src_	$(PARSRC_DIR)/src/par | \
		./configdir _genesis_		$(GEN_ROOT) \
		> $(INSTALL)/lib/gdbdebugger)
	-( chmod +x $(INSTALL)/lib/*debugger)
	-( cp configdir $(INSTALL)/lib/configdir )
	-( chmod +x $(INSTALL)/lib/configdir )
	-( cat gdbdebugger | \
		./configdir _pgenesis-src_	_extsrc_$(PARSRC_DIR)/src/par | \
		./configdir _genesis_		$(GEN_ROOT) \
		> $(INSTALL)/lib/gdbdebugger.ext)

