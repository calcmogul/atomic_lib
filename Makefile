# The following targets are supported by this makefile.
#
# all
# clean

.PHONY: all
all:
	@$(MAKE) -C atomic_forward_list
	@$(MAKE) -C atomic_stack

.PHONY: clean
clean:
	@$(MAKE) -C atomic_forward_list clean
	@$(MAKE) -C atomic_stack clean
