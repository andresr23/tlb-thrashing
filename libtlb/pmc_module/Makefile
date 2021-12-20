obj-m += pmc_module.o

.PHONY: intel clean

intel:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	@echo "=============================="
	@echo "Built pmc_module.ko for Intel."
	@echo "=============================="

clean:
	@$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
