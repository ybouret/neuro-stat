all:

clean:
	@echo "**** Cleaning v3"
	@$(MAKE) -s -C v3 clean
	@echo "**** Cleaning neurocorr"
	@$(MAKE) -s -C neurocorr clean

