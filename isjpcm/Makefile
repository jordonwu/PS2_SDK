
all: build-ee build-iop

clean:
	$(MAKE) -C ee clean
	$(MAKE) -C iop clean

build-ee:
	$(MAKE) -C ee

build-iop:
	$(MAKE) -C iop

install:
	$(MAKE) -C ee install
	$(MAKE) -C iop install
